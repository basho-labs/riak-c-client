/*********************************************************************
 *
 * test_listkeys.c:  Riak C Unit testing for List Keys Message
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
