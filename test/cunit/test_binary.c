/*********************************************************************
 *
 * test_binary.c: Riak C Unit testing for riak_binary
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
#include "riak.pb-c.h"
#include "riak_binary-internal.h"

void
test_build_binary() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_binary  *bin = riak_binary_new(cfg, 6, (riak_uint8_t*)"abcdef");
    CU_ASSERT_FATAL(bin != NULL)
    riak_int32_t  len = riak_binary_len(bin);
    CU_ASSERT_EQUAL(len,6)
    riak_uint8_t *data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(memcmp(data, "abcdef", 6), 0)
    riak_binary_free(cfg, &bin);
    CU_PASS("test_build_binary passed")
}

void
test_build_binary_shallow() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_uint8_t *ptr = (riak_uint8_t*)"abcdef";
    riak_binary  *bin = riak_binary_new_shallow(cfg, 6, ptr);
    CU_ASSERT_FATAL(bin != NULL)
    riak_int32_t  len = riak_binary_len(bin);
    CU_ASSERT_EQUAL(len,6)
    riak_uint8_t *data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(memcmp(data, "abcdef", 6), 0)
    // Should point the same memory
    CU_ASSERT_EQUAL(data, ptr)
    riak_binary_free(cfg, &bin);
    riak_binary *shallow = riak_binary_new(cfg, 6, ptr);
    bin = riak_binary_copy_shallow(cfg, shallow);
    data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(data, ptr)
    data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(data, ptr)
    riak_binary_free(cfg, &bin);
    riak_binary_free(cfg, &shallow);
    CU_PASS("test_build_binary_shallow passed")
}

void
test_build_binary_with_null() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_binary  *bin = riak_binary_new(cfg, 0, NULL);
    CU_ASSERT_FATAL(bin != NULL)
    riak_int32_t  len = riak_binary_len(bin);
    CU_ASSERT_EQUAL(len,0);
    riak_binary_free(cfg, &bin);
    CU_PASS("test_build_binary_with_null passed")
}

void
test_build_binary_from_existing() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_binary *bin = riak_binary_new(cfg, 6, (riak_uint8_t*)"abcdef");
    CU_ASSERT_FATAL(bin != NULL)
    riak_binary *bin2 = riak_binary_copy(cfg, bin);
    CU_ASSERT_FATAL(bin2 != NULL)
    riak_int32_t  len = riak_binary_len(bin2);
    CU_ASSERT_EQUAL(len,6);
    riak_uint8_t *data = riak_binary_data(bin2);
    CU_ASSERT_EQUAL(memcmp(data, "abcdef", 6), 0);
    riak_binary_free(cfg, &bin);
    riak_binary_free(cfg, &bin2);
    CU_PASS("test_build_binary_from_existing passed")
}

void
test_build_binary_from_pb() {
    riak_config       *cfg;
    riak_error          err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    ProtobufCBinaryData pb_bin;
    pb_bin.len  = 6;
    pb_bin.data = (riak_uint8_t *)"abcdef";
    riak_binary *bin = riak_binary_copy_from_pb(cfg, &pb_bin);
    CU_ASSERT_FATAL(bin != NULL)
    riak_int32_t len = riak_binary_len(bin);
    CU_ASSERT_EQUAL(len,6);
    riak_uint8_t *data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(memcmp(data, "abcdef", 6), 0);
    riak_binary_free(cfg, &bin);
    CU_PASS("test_build_binary_from_pb passed")
}

void
test_build_binary_to_pb() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    ProtobufCBinaryData pb_bin;
    riak_binary *bin = riak_binary_new(cfg, 6, (riak_uint8_t*)"abcdef");
    CU_ASSERT_FATAL(bin != NULL)
    riak_binary_copy_to_pb(&pb_bin, bin);
    CU_ASSERT_EQUAL(pb_bin.len,6);
    CU_ASSERT_EQUAL(memcmp(pb_bin.data, "abcdef", 6), 0);
    riak_binary_free(cfg, &bin);
    CU_PASS("test_build_binary_from_pb passed")
}

void
test_binary_new_from_string() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_binary  *bin = riak_binary_copy_from_string(cfg, "abcdef");
    CU_ASSERT_FATAL(bin != NULL)
    riak_int32_t  len = riak_binary_len(bin);
    CU_ASSERT_EQUAL(len,6);
    riak_uint8_t *data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(memcmp(data, "abcdef", 6), 0);
    riak_binary_free(cfg, &bin);
    CU_PASS("test_build_binary passed")
}

void
test_binary_hex_print() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_binary  *bin = riak_binary_new(cfg, 6, (riak_uint8_t*)"abcdef");
    char buffer[128];
    riak_binary_hex_print(bin, buffer, sizeof(buffer));
    CU_ASSERT_EQUAL(memcmp(buffer, "616263646566", 12), 0);
    riak_binary_free(cfg, &bin);
    CU_PASS("test_build_binary passed")
}
