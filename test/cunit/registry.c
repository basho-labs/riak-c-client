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
#include "test_binary.h"
#include "test_config.h"
#include "test_connection.h"
#include "test_operation.h"
#include "test_serverinfo.h"
#include "test_clientid.h"
#include "test_delete.h"

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
    CU_pSuite config_suite = CU_add_suite("riak_config", init_fn, cleanup_fn);
    CU_pSuite connection_suite = CU_add_suite("riak_connection", init_fn, cleanup_fn);
    CU_pSuite operation_suite = CU_add_suite("riak_operation", init_fn, cleanup_fn);
    CU_pSuite messages_suite = CU_add_suite("riak_messages", init_fn, cleanup_fn);
    CU_ADD_TEST(binary_suite, test_build_binary);
    CU_ADD_TEST(binary_suite, test_build_binary_with_null);
    CU_ADD_TEST(binary_suite, test_build_binary_from_pb);
    CU_ADD_TEST(binary_suite, test_build_binary_to_pb);
    CU_ADD_TEST(binary_suite, test_binary_new_from_string);
    CU_ADD_TEST(binary_suite, test_binary_hex_print);
    CU_ADD_TEST(binary_suite, test_build_binary_from_existing);
    CU_ADD_TEST(config_suite, test_build_config);
    CU_ADD_TEST(connection_suite, test_connection_with_bad_resolver);
    CU_ADD_TEST(connection_suite, test_connection_with_good_resolver);
    CU_ADD_TEST(config_suite, test_config_with_logging);
    CU_ADD_TEST(config_suite, test_config_allocate);
    CU_ADD_TEST(config_suite, test_config_allocate_clean);
    CU_ADD_TEST(config_suite, test_config_free);
    CU_ADD_TEST(operation_suite, test_operation_new);
    CU_ADD_TEST(operation_suite, test_operation_callbacks);
    CU_ADD_TEST(messages_suite, test_server_info_encode_request);
    CU_ADD_TEST(messages_suite, test_server_info_decode_good_response);
    CU_ADD_TEST(messages_suite, test_server_info_decode_bad_response);
    CU_ADD_TEST(messages_suite, test_set_clientid);
    CU_ADD_TEST(messages_suite, test_get_clientid);
    CU_ADD_TEST(messages_suite, test_delete_encode_request);

    // Run all tests using the CUnit Basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    void CU_cleanup_registry();
    return 0;
}
