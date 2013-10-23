/*********************************************************************
 *
 * registry.c: Riak C Unit testing registration
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
#include "riak.pb-c.h"
#include "riak_binary-internal.h"

void
test_build_binary() {
    riak_context *ctx;
    riak_error    err = riak_context_new_default(&ctx, "localhost", "1");
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_binary  *bin = riak_binary_new(ctx, 6, (riak_uint8_t*)"abcdef");
    riak_int32_t  len = riak_binary_len(bin);
    CU_ASSERT_EQUAL(len,6);
    riak_uint8_t *data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(memcmp(data, "abcdef", 6), 0);
    riak_binary_free(ctx, &bin);
    CU_PASS("test_build_binary passed")
}

void
test_build_binary_with_null() {
    riak_context *ctx;
    riak_error    err = riak_context_new_default(&ctx, "localhost", "1");
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_binary  *bin = riak_binary_new(ctx, 0, NULL);
    riak_int32_t  len = riak_binary_len(bin);
    CU_ASSERT_EQUAL(len,0);
    riak_uint8_t *data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(data, NULL);
    riak_binary_free(ctx, &bin);
    CU_PASS("test_build_binary_with_null passed")
}

void
test_build_binary_from_pb() {
    riak_context       *ctx;
    riak_error          err = riak_context_new_default(&ctx, "localhost", "1");
    CU_ASSERT_FATAL(err == ERIAK_OK)
    ProtobufCBinaryData pb_bin;
    pb_bin.len  = 6;
    pb_bin.data = (riak_uint8_t *)"abcdef";
    riak_binary *bin = riak_binary_populate_from_pb(ctx, &pb_bin);
    riak_int32_t len = riak_binary_len(bin);
    CU_ASSERT_EQUAL(len,6);
    riak_uint8_t *data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(memcmp(data, "abcdef", 6), 0);
    riak_binary_free(ctx, &bin);
    CU_PASS("test_build_binary_from_pb passed")
}

void
test_build_binary_to_pb() {
    riak_context *ctx;
    riak_error    err = riak_context_new_default(&ctx, "localhost", "1");
    CU_ASSERT_FATAL(err == ERIAK_OK)
    ProtobufCBinaryData pb_bin;
    riak_binary *bin = riak_binary_new(ctx, 6, (riak_uint8_t*)"abcdef");
    err = riak_binary_to_pb_deep_copy(ctx, &pb_bin, bin);
    CU_ASSERT_EQUAL(pb_bin.len,6);
    CU_ASSERT_EQUAL(memcmp(pb_bin.data, "abcdef", 6), 0);
    riak_binary_free(ctx, &bin);
    riak_binary_deep_free_pb(ctx, &pb_bin);
    CU_PASS("test_build_binary_from_pb passed")
}

void
test_binary_new_from_string() {
    riak_context *ctx;
    riak_error    err = riak_context_new_default(&ctx, "localhost", "1");
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_binary  *bin = riak_binary_new_from_string(ctx, "abcdef");
    riak_int32_t  len = riak_binary_len(bin);
    CU_ASSERT_EQUAL(len,6);
    riak_uint8_t *data = riak_binary_data(bin);
    CU_ASSERT_EQUAL(memcmp(data, "abcdef", 6), 0);
    riak_binary_free(ctx, &bin);
    CU_PASS("test_build_binary passed")
}

int
main(int   argc,
     char *argv[])
{
    CU_ErrorCode cuerr = CU_initialize_registry();
    if (cuerr != CUE_SUCCESS) {
        fprintf(stderr, "Could not initialize cuint testing registry\n");
        exit(1);
    }
    CU_InitializeFunc init_fn = NULL;
    CU_CleanupFunc cleanup_fn = NULL;
    CU_pSuite binary_suite = CU_add_suite("riak_binary", init_fn, cleanup_fn);
    CU_pTest test = CU_add_test(binary_suite, "build_binary", test_build_binary);
    CU_pTest test2 = CU_add_test(binary_suite, "build_binary_with_null", test_build_binary_with_null);
    CU_pTest test3 = CU_add_test(binary_suite, "build_binary_from_pb", test_build_binary_from_pb);
    CU_pTest test4 = CU_add_test(binary_suite, "build_binary_to_pb", test_build_binary_to_pb);
    CU_ADD_TEST(binary_suite, test_binary_new_from_string);
    CU_basic_run_tests();

    void CU_cleanup_registry();
}
