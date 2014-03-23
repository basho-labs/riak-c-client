/*********************************************************************
 *
 * test_server_error.c: Riak C Unit testing for Server Errors
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

/**
 * @brief Send invalid parameters to a 2i query request which will always
 * result in an error being returned from Riak.  This message will appear
 * from the call: ERR #0 - Invalid equality query undefined
 */
void
test_integration_error() {
    riak_config     *cfg;
    riak_connection *cxn = NULL;

    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = test_connect(cfg, &cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    test_bucket_key_value *db = NULL;
    err = test_load_db(cfg, cxn, &db, 1, 1);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_2index_response *response;
    err = riak_2index(cxn, db->bucket, db->key, NULL, &response);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_SERVER_ERROR)
    riak_2index_response_free(cfg, &response);

    test_bkv_free(cfg, &db);
    test_cleanup_db(cxn);
    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_error passed")
}

typedef struct _test_async_pthread_error_args {
    riak_binary         *bucket;
    riak_binary         *key;
} test_async_pthread_error_args;

void
test_error_async_cb(riak_2index_response *response,
                    void                 *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn = (test_async_connection*)state->conn;

    riak_2index_response_free(conn->cfg, &response);
    state->err = ERIAK_OK;
}

/**
 * @brief Encode and send a bad 2i request
 * @param args Parameters required to create bad 2i request
 */
void*
test_error_async_thread(void *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn  = state->conn;
    test_async_pthread_error_args *args = (test_async_pthread_error_args*)(state->args);
    riak_error err = riak_async_register_2index(conn->rop, args->bucket, args->key, NULL, (riak_response_callback)test_error_async_cb);
    if (err) {
        return (void*)riak_strerror(err);
    }
    err = test_async_send_message(conn);
    if (err) {
        return (void*)"Could not send request";
    }
    return NULL;
}

/**
 * @brief Send invalid parameters to a 2i query request which will always
 * result in an error being returned from Riak.  This message will appear
 * from the async callback: CALLBACK ERROR: Invalid equality query undefined
 */
void
test_integration_async_error() {
    riak_config           *cfg;
    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_connection *cxn = NULL;
    err = test_connect(cfg, &cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    test_bucket_key_value *db = NULL;
    err = test_load_db(cfg, cxn, &db, 1, 1);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    test_async_pthread_error_args args;
    args.bucket = db->bucket;  // Just pick a random bucket
    args.key    = db->key;
    err = test_async_thread_runner(cfg, test_error_async_thread, (void*)&args, NULL);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_SERVER_ERROR)

    test_cleanup_db(cxn);
    test_bkv_free(cfg, &db);
    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_async_error passed")
}
