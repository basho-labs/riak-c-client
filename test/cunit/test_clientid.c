/*********************************************************************
 *
 * test_server_info.c: Riak C Unit testing for Client Identifier messages
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
 * @brief Test decoding a fixed response from a set clientid request
 */
void
test_set_clientid() {
    riak_config     *cfg;
    riak_operation  *rop = NULL;
    riak_connection *cxn = NULL;
    riak_pb_message *request;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    riak_binary clientid;
    clientid.data = (riak_uint8_t*)"test_clientid";
    clientid.len = sizeof("test_clientid");

    // Binary Protocol Buffer message from server
    riak_uint8_t bytes[] = { 0x0a,0x0e,0x74,0x65,0x73,0x74,0x5f,0x63,0x6c,0x69,0x65,0x6e,0x74,0x69,0x64,0x00 };
    err = riak_set_clientid_request_encode(rop, &clientid, &request);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    CU_ASSERT_EQUAL(memcmp(request->data, bytes, sizeof(bytes)), 0)

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_pb_message_free(cfg, &request);
    riak_config_free(&cfg);
    CU_PASS("test_set_clientid passed")
}

/**
 * @brief Test decoding a fixed response from a get clientid request
 */
void
test_get_clientid() {
    riak_config     *cfg;
    riak_operation  *rop = NULL;
    riak_connection *cxn = NULL;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);

    // Binary Protocol Buffer message from server
    riak_uint8_t bytes[] = { 0x04,0x0a,0x04,0x00,0x00,0x00,0x00 };
    riak_pb_message pb_resp;
    pb_resp.data = bytes;
    pb_resp.len = sizeof(bytes);
    riak_get_clientid_response *response = NULL;
    riak_boolean_t done = RIAK_FALSE;
    err = riak_get_clientid_response_decode(rop, &pb_resp, &response, &done);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    CU_ASSERT_EQUAL(done, RIAK_TRUE)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(response->client_id), "\0\0\0\0", 4), 0)

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_get_clientid_response_free(cfg, &response);
    riak_config_free(&cfg);
    CU_PASS("test_server_info_encode_request passed")
}


void
test_integration_clientid() {
    riak_config     *cfg;
    riak_connection *cxn = NULL;

    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = test_connect(cfg, &cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_binary *client_id = test_random_binary(cfg, RIAK_TEST_BUCKET_KEY_LEN);
    CU_ASSERT_NOT_EQUAL_FATAL(client_id,NULL)
    err = riak_set_clientid(cxn, client_id);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    riak_get_clientid_response *response = NULL;
    err = riak_get_clientid(cxn, &response);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    int equals = riak_binary_compare(client_id, riak_get_clientid_get_clientid(response));
    CU_ASSERT_EQUAL_FATAL(equals,0)
    riak_get_clientid_response_free(cfg, &response);

    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_clientid passed")
}

typedef struct _test_async_pthread_clientid_args {
    riak_binary *clientid;
} test_async_pthread_clientid_args;

void
test_set_clientid_async_cb(riak_set_clientid_response *response,
                           void                       *ptr) {
    test_async_pthread              *state = (test_async_pthread*)ptr;
    test_async_connection            *conn = (test_async_connection*)state->conn;

    riak_set_clientid_response_free(conn->cfg, &response);
    state->err = ERIAK_OK;
}

void
test_get_clientid_async_cb(riak_get_clientid_response *response,
                           void                       *ptr) {
    test_async_pthread     *state    = (test_async_pthread*)ptr;
    test_async_connection  *conn     = (test_async_connection*)state->conn;
    riak_binary            *clientid = (riak_binary*)state->expected;
    riak_binary            *result   = riak_get_clientid_get_clientid(response);
    int                     equals   = riak_binary_compare(clientid, result);
    if (!equals) {
        state->err = ERIAK_INVALID;
        riak_print_state print_state;
        riak_print_init(&print_state, state->err_msg, sizeof(state->err_msg));
        riak_print(&print_state, "%s", "Expected Client ID of '");
        riak_print_binary(&print_state, clientid);
        riak_print(&print_state, "%s", "' but received '");
        riak_print_binary(&print_state, result);
        riak_print(&print_state, "%s", "'\n");

        riak_get_clientid_response_free(conn->cfg, &response);
        return;
    }

    riak_get_clientid_response_free(conn->cfg, &response);
    state->err = ERIAK_OK;
}

/**
 * @brief Encode and Send a Set Client ID request
 * @param args Parameters required to create set clientid request
 */
void*
test_set_clientid_async_thread(void *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn  = state->conn;
    test_async_pthread_clientid_args *args = (test_async_pthread_clientid_args*)(state->args);
    riak_error err = riak_async_register_set_clientid(conn->rop, args->clientid, (riak_response_callback)test_set_clientid_async_cb);
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
 * @brief Encode and Send a Get Client ID request
 * @param args Parameters required to create set clientid request
 */
void*
test_get_clientid_async_thread(void *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn  = state->conn;
    riak_error err = riak_async_register_get_clientid(conn->rop, (riak_response_callback)test_get_clientid_async_cb);
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
test_integration_async_clientid() {
    riak_config           *cfg;
    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_connection *cxn = NULL;
    err = test_connect(cfg, &cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    test_async_pthread_clientid_args args;
    riak_binary *client_id = test_random_binary(cfg, RIAK_TEST_BUCKET_KEY_LEN);
    CU_ASSERT_NOT_EQUAL_FATAL(client_id,NULL)
    args.clientid = client_id;  // Random String

    err = test_async_thread_runner(cfg, test_set_clientid_async_thread, (void*)&args, NULL);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    err = test_async_thread_runner(cfg, test_get_clientid_async_thread, (void*)&args, (void*)client_id);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_async_clientid passed")
}

