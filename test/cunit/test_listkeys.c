/*********************************************************************
 *
 * test_listkeys.c:  Riak C Unit testing for List Keys Message
 *
 * Copyright (c) 2007-2014 Basho Technologies, Inc.  All Rights Reserved.
 *
 * This file is provided to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License.  You may obtain
 * a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include "riak.h"
#include "riak_messages-internal.h"
#include "riak_operation-internal.h"
#include "test.h"

void
test_listkeys_response_decode() {
    riak_config     *cfg;
    riak_operation  *rop = NULL;
    riak_connection *cxn = NULL;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_uint8_t bytes0[] = { 0x12,0x0a,0x12,0x33,0x39,0x33,0x33,0x31,0x30,0x30,0x31,0x33,0x38,0x36,0x35,0x30,0x36,0x36,0x34,0x39,0x36 };
    riak_uint8_t bytes1[] = { 0x12,0x0a,0x03,0x62,0x61,0x6d };
    riak_uint8_t bytes2[] = { 0x12,0x10,0x01 };
    riak_uint8_t *bytes[] = { bytes0, bytes1, bytes2 };
    riak_int32_t len[]    = { sizeof(bytes0), sizeof(bytes1), sizeof(bytes2) };

    riak_pb_message            pb_response;
    riak_listkeys_response *response = NULL;
    riak_boolean_t             done = RIAK_FALSE;
    for(int i = 0; i < 3; i++) {
        pb_response.data = bytes[i];
        pb_response.len  = len[i];
        err = riak_listkeys_response_decode(rop, &pb_response, &response, &done);
        CU_ASSERT_FATAL(err == ERIAK_OK)
    }
    CU_ASSERT_EQUAL_FATAL(riak_listkeys_get_n_keys(response), 2);
    riak_binary** keys = riak_listkeys_get_keys(response);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(keys[0]), "393310013865066496", riak_binary_len(keys[0])),0)
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(keys[1]), "bam", riak_binary_len(keys[1])),0)

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_listkeys_response_free(cfg, &response);
    riak_config_free(&cfg);
    CU_PASS("test_liskeys_response_decode passed")
}

void
test_integration_listkeys() {
    riak_config     *cfg;
    riak_connection *cxn = NULL;

    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = test_connect(cfg, &cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    test_bucket_key_value *db = NULL;
    err = test_load_db(cfg, cxn, &db, 1, RIAK_TEST_MAX_KEYS);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_listkeys_response *response = NULL;
    err = riak_listkeys(cxn, db->bucket, 5000, &response);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    CU_ASSERT_EQUAL(response->n_keys, 50)

    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_listkeys_response_print(&print_state, response);
    fprintf(stderr, "%s", output);
    riak_listkeys_response_free(cfg, &response);

    test_bkv_free(cfg, &db);
    test_cleanup_db(cxn);
    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_listkeys passed")
}

void
test_listkeys_async_cb(riak_listkeys_response *response,
                       void                   *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn  = state->conn;
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_listkeys_response_print(&print_state, response);
    fprintf(stderr, "%s", output);
    riak_uint32_t num = riak_listkeys_get_n_keys(response);
    riak_uint64_t expected = (riak_uint64_t)state->expected;
    if (num < expected) {
        state->err = ERIAK_OUT_OF_RANGE;
        snprintf(state->err_msg, sizeof(state->err_msg), "Only %ul keys were returned", num);
    }
    riak_listkeys_response_free(conn->cfg, &response);
    state->err = ERIAK_OK;
}

typedef struct _test_async_pthread_listkey_args {
    riak_binary            *bucket;
    riak_uint32_t           timeout;
} test_async_pthread_listkey_args;

/**
 * @brief Encode and Send a List Buckets request
 * @param args Parameters required to create List Buckets request
 */
void*
test_listkeys_async_thread(void *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn  = state->conn;
    test_async_pthread_listkey_args *args = (test_async_pthread_listkey_args*)(state->args);
    riak_error err = riak_async_register_listkeys(conn->rop, args->bucket, args->timeout, (riak_response_callback)test_listkeys_async_cb);
    if (err) {
        return (void*)riak_strerror(err);
    }
    err = test_async_send_message(conn);
    if (err) {
        return (void*)"Could not send request";
    }
    return NULL;
}

void
test_integration_async_listkeys() {
    riak_config           *cfg;
    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_connection *cxn = NULL;
    err = test_connect(cfg, &cxn);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    test_bucket_key_value *db = NULL;
    err = test_load_db(cfg, cxn, &db, 1, RIAK_TEST_MAX_KEYS);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    test_async_pthread_listkey_args args;
    args.bucket = db->bucket;  // Just pick a random bucket
    args.timeout = 5000;
    err = test_async_thread_runner(cfg, test_listkeys_async_thread, (void*)&args, (void*)RIAK_TEST_MAX_BUCKETS);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    test_cleanup_db(cxn);
    test_bkv_free(cfg, &db);
    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_async_listkeys passed")
}
