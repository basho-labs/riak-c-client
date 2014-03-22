/*********************************************************************
 *
 * registry.c: Riak C Unit testing registration
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
#include "test.h"
#include "test_2index.h"
#include "test_array.h"
#include "test_binary.h"
#include "test_bucketprops.h"
#include "test_clientid.h"
#include "test_config.h"
#include "test_connection.h"
#include "test_delete.h"
#include "test_get.h"
#include "test_listbuckets.h"
#include "test_listkeys.h"
#include "test_mapreduce.h"
#include "test_object.h"
#include "test_operation.h"
#include "test_ping.h"
#include "test_put.h"
#include "test_search.h"
#include "test_serverinfo.h"

int
main(int   argc,
     char *argv[])
{
    event_enable_debug_mode();
    evthread_enable_lock_debuging();
    int result = evthread_use_pthreads();
    if (result < 0) {
        fprintf(stderr, "Could not use pthreads for libevent\n");
        return ERIAK_EVENT;
    }

    CU_ErrorCode cuerr = CU_initialize_registry();
    if (cuerr != CUE_SUCCESS) {
        fprintf(stderr, "Could not initialize cuint testing registry\n");
        exit(1);
    }
    CU_InitializeFunc init_fn = NULL;
    CU_CleanupFunc cleanup_fn = NULL;
    CU_pSuite array_suite = CU_add_suite("riak_array", init_fn, cleanup_fn);
    CU_pSuite binary_suite = CU_add_suite("riak_binary", init_fn, cleanup_fn);
    CU_pSuite config_suite = CU_add_suite("riak_config", init_fn, cleanup_fn);
    CU_pSuite connection_suite = CU_add_suite("riak_connection", init_fn, cleanup_fn);
    CU_pSuite operation_suite = CU_add_suite("riak_operation", init_fn, cleanup_fn);
    CU_pSuite messages_suite = CU_add_suite("riak_messages", init_fn, cleanup_fn);
    CU_pSuite integration_suite = CU_add_suite("integration_suite", init_fn, cleanup_fn);
    CU_ADD_TEST(array_suite, test_array_empty);
    CU_ADD_TEST(array_suite, test_array_push);
    CU_ADD_TEST(array_suite, test_array_insert);
    CU_ADD_TEST(array_suite, test_array_bad_insert);
    CU_ADD_TEST(array_suite, test_array_set_get);
    CU_ADD_TEST(array_suite, test_array_bad_set);
    CU_ADD_TEST(array_suite, test_array_bad_get);
    CU_ADD_TEST(array_suite, test_array_pop);
    CU_ADD_TEST(array_suite, test_array_empty_pop);
    CU_ADD_TEST(array_suite, test_array_remove);
    CU_ADD_TEST(array_suite, test_array_bad_remove);
    CU_ADD_TEST(array_suite, test_array_length);
    CU_ADD_TEST(array_suite, test_array_sort);
    CU_ADD_TEST(binary_suite, test_build_binary);
    CU_ADD_TEST(binary_suite, test_build_binary_shallow);
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
    CU_ADD_TEST(messages_suite, test_get_options_r);
    CU_ADD_TEST(messages_suite, test_get_options_pr);
    CU_ADD_TEST(messages_suite, test_get_options_basic_quorum);
    CU_ADD_TEST(messages_suite, test_get_options_notfound_ok);
    CU_ADD_TEST(messages_suite, test_get_options_if_modified);
    CU_ADD_TEST(messages_suite, test_get_options_head);
    CU_ADD_TEST(messages_suite, test_get_options_deletedvclock);
    CU_ADD_TEST(messages_suite, test_get_options_timeout);
    CU_ADD_TEST(messages_suite, test_get_options_sloppy_quorum);
    CU_ADD_TEST(messages_suite, test_get_options_n_val);
    CU_ADD_TEST(messages_suite, test_get_decode_response);
    CU_ADD_TEST(messages_suite, test_put_options_vclock);
    CU_ADD_TEST(messages_suite, test_put_options_w);
    CU_ADD_TEST(messages_suite, test_put_options_dw);
    CU_ADD_TEST(messages_suite, test_put_options_return_body);
    CU_ADD_TEST(messages_suite, test_put_options_pw);
    CU_ADD_TEST(messages_suite, test_put_options_if_not_modified);
    CU_ADD_TEST(messages_suite, test_put_options_if_none_match);
    CU_ADD_TEST(messages_suite, test_put_options_return_head);
    CU_ADD_TEST(messages_suite, test_put_options_timeout);
    CU_ADD_TEST(messages_suite, test_put_options_asis);
    CU_ADD_TEST(messages_suite, test_put_options_sloppy_quorum);
    CU_ADD_TEST(messages_suite, test_put_options_n_val);
    CU_ADD_TEST(messages_suite, test_put_decode_response);
    CU_ADD_TEST(messages_suite, test_listbuckets_response_decode);
    CU_ADD_TEST(messages_suite, test_listkeys_response_decode);
    CU_ADD_TEST(messages_suite, test_bucketprops);
    CU_ADD_TEST(messages_suite, test_mapreduce_response_decode);
    CU_ADD_TEST(messages_suite, test_2index_options_qtype);
    CU_ADD_TEST(messages_suite, test_2index_options_key);
    CU_ADD_TEST(messages_suite, test_2index_options_range_min);
    CU_ADD_TEST(messages_suite, test_2index_options_range_max);
    CU_ADD_TEST(messages_suite, test_2index_options_return_terms);
    CU_ADD_TEST(messages_suite, test_2index_options_max_results);
    CU_ADD_TEST(messages_suite, test_2index_options_continuation);
    CU_ADD_TEST(messages_suite, test_2index_options_timeout);
    CU_ADD_TEST(messages_suite, test_2index_options_type);
    CU_ADD_TEST(messages_suite, test_2index_options_term_regex);
    CU_ADD_TEST(messages_suite, test_2index_options_pagination_sort);
    CU_ADD_TEST(messages_suite, test_2index_response_decode);
    CU_ADD_TEST(messages_suite, test_search_options_rows);
    CU_ADD_TEST(messages_suite, test_search_options_start);
    CU_ADD_TEST(messages_suite, test_search_options_sort);
    CU_ADD_TEST(messages_suite, test_search_options_filter);
    CU_ADD_TEST(messages_suite, test_search_options_df);
    CU_ADD_TEST(messages_suite, test_search_options_op);
    CU_ADD_TEST(messages_suite, test_search_options_fl);
    CU_ADD_TEST(messages_suite, test_search_options_presort);
    CU_ADD_TEST(messages_suite, test_search_decode_response);
    CU_ADD_TEST(messages_suite, test_riak_object_bucket);
    CU_ADD_TEST(messages_suite, test_riak_object_key);
    CU_ADD_TEST(messages_suite, test_riak_object_value);
    CU_ADD_TEST(messages_suite, test_riak_object_charset);
    CU_ADD_TEST(messages_suite, test_riak_object_last_mod);
    CU_ADD_TEST(messages_suite, test_riak_object_last_mod_usecs);
    CU_ADD_TEST(messages_suite, test_riak_object_content_type);
    CU_ADD_TEST(messages_suite, test_riak_object_content_encoding);
    CU_ADD_TEST(messages_suite, test_riak_object_deleted);
    CU_ADD_TEST(messages_suite, test_riak_object_vtag);
    CU_ADD_TEST(messages_suite, test_riak_object_links);
    CU_ADD_TEST(messages_suite, test_riak_object_usermeta);
    CU_ADD_TEST(messages_suite, test_riak_object_indexes);
    CU_ADD_TEST(integration_suite, test_integration_ping);
    CU_ADD_TEST(integration_suite, test_integration_async_ping);
    CU_ADD_TEST(integration_suite, test_integration_server_info);
    CU_ADD_TEST(integration_suite, test_integration_async_server_info);
    CU_ADD_TEST(integration_suite, test_integration_listbuckets);
    CU_ADD_TEST(integration_suite, test_integration_async_listbuckets);
    CU_ADD_TEST(integration_suite, test_integration_listkeys);
    CU_ADD_TEST(integration_suite, test_integration_async_listkeys);
    CU_ADD_TEST(integration_suite, test_integration_get_value);
    CU_ADD_TEST(integration_suite, test_integration_async_get_value);
    CU_ADD_TEST(integration_suite, test_integration_async_get_bad_value);
    CU_ADD_TEST(integration_suite, test_integration_delete);
    CU_ADD_TEST(integration_suite, test_integration_async_delete);

    // Only run integration tests if an argument is passed in
    if (argc < 2) {
        CU_set_suite_active(integration_suite, CU_FALSE);
    }

    // Run all tests using the CUnit Basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    return failures;
}
