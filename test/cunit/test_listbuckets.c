/*********************************************************************
 *
 * test_listbuckets.c:  Riak C Unit testing for List Buckets Message
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
test_listbuckets_response_decode() {
    riak_config     *cfg;
    riak_operation  *rop = NULL;
    riak_connection *cxn = NULL;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_uint8_t bytes0[] = { 0x10,0x0a,0x07,0x74,0x77,0x69,0x74,0x74,0x65,0x72 };
    riak_uint8_t bytes1[] = { 0x10,0x0a,0x03,0x66,0x6f,0x6f };
    riak_uint8_t bytes2[] = { 0x10,0x0a,0x04,0x74,0x65,0x73,0x74 };
    riak_uint8_t bytes3[] = { 0x10,0x10,0x01 };
    riak_uint8_t *bytes[] = { bytes0, bytes1, bytes2, bytes3 };
    riak_int32_t len[]    = { sizeof(bytes0), sizeof(bytes1), sizeof(bytes2), sizeof(bytes3) };

    riak_pb_message            pb_response;
    riak_listbuckets_response *response = NULL;
    riak_boolean_t             done = RIAK_FALSE;
    for(int i = 0; i < 4; i++) {
        pb_response.data = bytes[i];
        pb_response.len  = len[i];
        err = riak_listbuckets_response_decode(rop, &pb_response, &response, &done);
        CU_ASSERT_FATAL(err == ERIAK_OK)
    }
    CU_ASSERT_EQUAL_FATAL(riak_listbuckets_get_n_buckets(response), 3);
    riak_binary* bucket0, *bucket1, *bucket2;
    err = riak_listbuckets_get_bucket(response, &bucket0, 0);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    err = riak_listbuckets_get_bucket(response, &bucket1, 1);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    err = riak_listbuckets_get_bucket(response, &bucket2, 2);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(bucket0), "twitter", riak_binary_len(bucket0)),0)
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(bucket1), "foo", riak_binary_len(bucket1)),0)
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(bucket2), "test", riak_binary_len(bucket2)),0)

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_listbuckets_response_free(cfg, &response);
    riak_config_free(&cfg);
    CU_PASS("test_listbuckets_response_decode passed")
}
