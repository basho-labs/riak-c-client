/*********************************************************************
 *
 * test_delete.c: Riak C Unit testing for Deletemessage
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
test_delete_encode_request() {
    riak_config              *cfg;
    riak_operation           *rop = NULL;
    riak_connection          *cxn = NULL;
    riak_pb_message          *request;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_delete_options *opt = riak_delete_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary vclock;
    vclock.data = (riak_uint8_t*)"12345678";
    vclock.len  = 8;
    riak_delete_options_set_vclock(cfg, opt, &vclock);
    riak_delete_options_set_w(opt, 5);
    riak_delete_options_set_dw(opt, 1);
    riak_delete_options_set_pw(opt, 3);
    riak_delete_options_set_timeout(opt, 3000);
    riak_delete_options_set_sloppy_quorum(opt, RIAK_TRUE);
    riak_delete_options_set_n_val(opt, 3);

    riak_binary bucket;
    bucket.data = (riak_uint8_t*)"bucket";
    bucket.len  = 6;

    riak_binary key;
    key.data = (riak_uint8_t*)"key";
    key.len  = 3;

    err = riak_delete_request_encode(rop, &bucket, &key, opt, &request);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_uint8_t bytes[] = { 0x0a,0x06,0x62,0x75,0x63,0x6b,0x65,0x74,0x12,0x03,0x6b,
                             0x65,0x79,0x22,0x08,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
                             0x38,0x30,0x05,0x40,0x03,0x48,0x01,0x50,0xb8,0x17,0x58,
                             0x01,0x60,0x03 };
    CU_ASSERT_EQUAL(memcmp(request->data, bytes, sizeof(bytes)), 0)

    riak_delete_options_free(cfg, &opt);
    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_pb_message_free(cfg, &request);
    riak_config_free(&cfg);
    CU_PASS("test_delete_request_encoding passed")
}

void
test_integration_delete() {
    riak_config     *cfg;
    riak_connection *cxn = NULL;

    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = test_connect(cfg, &cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    test_bucket_key_value *db = NULL;
    err = test_load_db(cfg, cxn, &db, 1, 1);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    err = riak_delete(cxn, db->bucket, db->key, NULL);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    riak_get_response *response = NULL;
    err = riak_get(cxn, db->bucket, db->key, NULL, &response);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    riak_uint32_t num = riak_get_get_n_content(response);
    CU_ASSERT_EQUAL_FATAL(num, 0)
    riak_get_response_free(cfg, &response);

    test_bkv_free(cfg, &db);
    test_cleanup_db(cxn);
    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_delete passed")
}

typedef struct _test_async_pthread_delete_args {
    riak_binary         *bucket;
    riak_binary         *key;
    riak_delete_options *opts;
} test_async_pthread_delete_args;

void
test_delete_async_cb(riak_delete_response *del_response,
                     void                 *ptr) {
    test_async_pthread            *state = (test_async_pthread*)ptr;
    test_async_connection          *conn = (test_async_connection*)state->conn;
    test_async_pthread_delete_args *args = (test_async_pthread_delete_args*)state->args;

    riak_get_response *response;
    riak_error err = riak_get(conn->cxn, args->bucket, args->key, NULL, &response);
    if (err) {
        state->err = err;
        riak_get_response_free(conn->cfg, &response);
        return;
    }

    riak_uint32_t num = riak_get_get_n_content(response);
    if (num != 0) {
        state->err = ERIAK_OUT_OF_RANGE;
        snprintf(state->err_msg, sizeof(state->err_msg), "Expected 0 result but got %d", num);
        riak_get_response_free(conn->cfg, &response);
        return;
    }
    riak_get_response_free(conn->cfg, &response);
    state->err = ERIAK_OK;
}

/**
 * @brief Encode and Send a Get request
 * @param args Parameters required to create Delete request
 */
void*
test_delete_async_thread(void *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn  = state->conn;
    test_async_pthread_delete_args *args = (test_async_pthread_delete_args*)(state->args);
    riak_error err = riak_async_register_delete(conn->rop, args->bucket, args->key, args->opts, (riak_response_callback)test_delete_async_cb);
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
test_integration_async_delete() {
    riak_config           *cfg;
    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_connection *cxn = NULL;
    err = test_connect(cfg, &cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    test_bucket_key_value *db = NULL;
    err = test_load_db(cfg, cxn, &db, 1, 1);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    test_async_pthread_delete_args args;
    args.bucket = db->bucket;  // Just pick a random bucket
    args.key    = db->key;
    args.opts   = NULL;
    err = test_async_thread_runner(cfg, test_delete_async_thread, (void*)&args, NULL);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    test_cleanup_db(cxn);
    test_bkv_free(cfg, &db);
    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_async_delete passed")
}
