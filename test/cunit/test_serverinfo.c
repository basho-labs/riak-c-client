/*********************************************************************
 *
 * test_server_info.c: Riak C Unit testing for Server Information message
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
test_server_info_encode_request() {
    riak_config              *cfg;
    riak_operation           *rop = NULL;
    riak_connection          *cxn = NULL;
    riak_pb_message          *request;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);

    err = riak_serverinfo_request_encode(rop, &request);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_pb_message_free(cfg, &request);
    riak_config_free(&cfg);
    CU_PASS("test_server_info_encode_request passed")
}


void
test_server_info_decode_good_response() {
    riak_config              *cfg;
    riak_operation           *rop = NULL;
    riak_connection          *cxn = NULL;
    riak_boolean_t            done;
    riak_serverinfo_response *resp = NULL;
    riak_uint8_t              bytes[] = { 0x08,0x0a,0x0e,0x64,0x65,0x76,0x31,0x40,0x31,0x32,0x37,0x2e,0x30,0x2e,0x30,0x2e,0x31,0x12,0x09,0x32,0x2e,0x30,0x2e,0x30,0x70,0x72,0x65,0x34 };
    riak_pb_message           pbresp = { sizeof(bytes), MSG_RPBGETSERVERINFORESP, bytes };

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);

    // riak_decode_serverinfo_response
    err = riak_serverinfo_response_decode(rop, &pbresp, &resp, &done);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    CU_ASSERT_FATAL(done == RIAK_TRUE)

    CU_ASSERT_FATAL(riak_serverinfo_get_has_node(resp) == RIAK_TRUE)
    riak_binary *node = riak_serverinfo_get_node(resp);
    CU_ASSERT_EQUAL(memcmp(node->data, "dev1@127.0.0.1", 14), 0)
    CU_ASSERT_FATAL(riak_serverinfo_get_has_server_version(resp) == RIAK_TRUE)
    riak_binary *server = riak_serverinfo_get_server_version(resp);
    CU_ASSERT_EQUAL(memcmp(server->data, "2.0.0pre4", 9), 0)

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_serverinfo_response_free(cfg, &resp);
    riak_config_free(&cfg);
    CU_PASS("test_server_info_decode_good_response passed")
}

void
test_server_info_decode_bad_response() {
    riak_config              *cfg;
    riak_operation           *rop = NULL;
    riak_connection          *cxn = NULL;
    riak_boolean_t            done;
    riak_serverinfo_response *resp = NULL;
    riak_uint8_t              bytes[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x37,0x2e,0x30,0x2e,0x30,0x2e,0x31,0x12,0x09,0x32,0x2e,0x30,0x2e,0x30,0x70,0x72,0x65,0x34 };
    riak_pb_message           pbresp = { sizeof(bytes), MSG_RPBGETSERVERINFORESP, bytes };

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);

    // riak_decode_serverinfo_response
    err = riak_serverinfo_response_decode(rop, &pbresp, &resp, &done);

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_serverinfo_response_free(cfg, &resp);
    riak_config_free(&cfg);
    CU_ASSERT_FATAL(err != ERIAK_OK)
    CU_PASS("test_server_info_decode_bad_response passed")
}

void
test_integration_server_info() {
    riak_config     *cfg;
    riak_connection *cxn = NULL;

    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = test_connect(cfg, &cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_serverinfo_response *response = NULL;
    err = riak_serverinfo(cxn, &response);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    char output[1024];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_serverinfo_response_print(&print_state, response);
    fprintf(stderr, "%s", output);
    riak_serverinfo_response_free(cfg, &response);
    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_server_info passed")
}

/**
 * @brief Callback for Server Info message
 * @param response Server Info response
 * @parm ptr Riak Operation
 */
void
test_serverinfo_async_cb(riak_serverinfo_response *response,
                         void                     *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn = (test_async_connection*)state->conn;
    char output[1024];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_serverinfo_response_print(&print_state, response);
    fprintf(stderr, "ASYNCHRONOUS %s", output);
    riak_serverinfo_response_free(conn->cfg, &response);
    state->err = ERIAK_OK;
}

/**
 * @brief Encode and Send a Server Info request
 * @param args Parameters required to create Server Info request
 */
void*
test_serverinfo_async_thread(void *ptr) {
    test_async_pthread *state = (test_async_pthread*)ptr;
    test_async_connection *conn = state->conn;
    riak_error err = riak_async_register_serverinfo(conn->rop, (riak_response_callback)test_serverinfo_async_cb);
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
test_integration_async_server_info() {
    riak_config           *cfg;
    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = test_async_thread_runner(cfg, test_serverinfo_async_thread, NULL, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    test_cleanup(&cfg);
    CU_PASS("test_integration_async_server_info passed")
}

