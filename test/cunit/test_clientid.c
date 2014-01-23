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
