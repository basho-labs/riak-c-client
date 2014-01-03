/*********************************************************************
 *
 * test_mapreduce.c:  Riak C Unit testing for Map/Reduce Message
 *
 * Copyright (c) 2007-2013 Basho Technologies, Inc.  All Rights Reserved.
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
test_mapreduce_response_decode() {
    riak_config     *cfg;
    riak_operation  *rop = NULL;
    riak_connection *cxn = NULL;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_uint8_t bytes0[] = { 0x18,0x08,0x00,0x12,0x0b,0x5b,0x5b,0x22,0x66,0x6f,0x6f,0x22,0x2c,0x31,0x5d,0x5d };
    riak_uint8_t bytes1[] = { 0x18,0x08,0x00,0x12,0x0b,0x5b,0x5b,0x22,0x62,0x61,0x6d,0x22,0x2c,0x33,0x5d,0x5d };
    riak_uint8_t bytes2[] = { 0x18,0x08,0x00,0x12,0x0b,0x5b,0x5b,0x22,0x62,0x61,0x7a,0x22,0x2c,0x30,0x5d,0x5d };
    riak_uint8_t bytes3[] = { 0x18,0x08,0x00,0x12,0x0b,0x5b,0x5b,0x22,0x62,0x61,0x72,0x22,0x2c,0x34,0x5d,0x5d };
    riak_uint8_t bytes4[] = { 0x18,0x18,0x01 };
    riak_uint8_t *bytes[] = { bytes0, bytes1, bytes2, bytes3, bytes4 };
    riak_int32_t len[]    = { sizeof(bytes0), sizeof(bytes1), sizeof(bytes2), sizeof(bytes3), sizeof(bytes4) };

    riak_pb_message          pb_response;
    riak_mapreduce_response *response = NULL;
    riak_boolean_t           done = RIAK_FALSE;
    for(int i = 0; i < 5; i++) {
        pb_response.data = bytes[i];
        pb_response.len  = len[i];
        err = riak_mapreduce_response_decode(rop, &pb_response, &response, &done);
        CU_ASSERT_FATAL(err == ERIAK_OK)
    }
    CU_ASSERT_EQUAL_FATAL(response->n_responses, 5)
    riak_mapreduce_message** msgs = riak_mapreduce_get_messages(response);
    riak_binary *msg = riak_mapreduce_message_get_response(msgs[0]);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(msg), "[[\"foo\",1]]", riak_binary_len(msg)),0)
    msg = riak_mapreduce_message_get_response(msgs[1]);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(msg), "[[\"bam\",3]]", riak_binary_len(msg)),0)
    msg = riak_mapreduce_message_get_response(msgs[2]);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(msg), "[[\"baz\",0]]", riak_binary_len(msg)),0)
    msg = riak_mapreduce_message_get_response(msgs[3]);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(msg), "[[\"bar\",4]]", riak_binary_len(msg)),0)
    CU_ASSERT_EQUAL_FATAL(riak_mapreduce_message_get_has_done(msgs[4]), RIAK_TRUE)
    CU_ASSERT_EQUAL_FATAL(riak_mapreduce_message_get_done(msgs[4]), RIAK_TRUE)
}
