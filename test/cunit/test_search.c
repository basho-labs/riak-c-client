/*********************************************************************
 *
 * test_search.c: Riak C Unit testing for Search Message
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
#include "riak_object-internal.h"
#include "test.h"

void
test_search_options_rows() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_search_options *opt = riak_search_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_search_options_set_rows(opt, 500);
    CU_ASSERT_EQUAL(riak_search_options_get_has_rows(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_search_options_get_rows(opt), 500)

    riak_search_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_search_options_rows passed")
}

void
test_search_options_start() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_search_options *opt = riak_search_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_search_options_set_start(opt, 5);
    CU_ASSERT_EQUAL(riak_search_options_get_has_start(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_search_options_get_start(opt), 5)

    riak_search_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_search_options_start passed")
}

void
test_search_options_sort() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_search_options *opt = riak_search_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary sorter;
    sorter.data = (riak_uint8_t*)"sorter";
    sorter.len = 6;
    riak_search_options_set_sort(cfg, opt, &sorter);
    CU_ASSERT_EQUAL(riak_search_options_get_has_sort(opt), RIAK_TRUE)
    riak_binary *result = riak_search_options_get_sort(opt);
    CU_ASSERT_NOT_EQUAL_FATAL(result, NULL)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "sorter", 6), 0)

    riak_search_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_search_options_start passed")
}

void
test_search_options_filter() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_search_options *opt = riak_search_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary filter;
    filter.data = (riak_uint8_t*)"filter";
    filter.len = 6;
    riak_search_options_set_filter(cfg, opt, &filter);
    CU_ASSERT_EQUAL(riak_search_options_get_has_filter(opt), RIAK_TRUE)
    riak_binary *result = riak_search_options_get_filter(opt);
    CU_ASSERT_NOT_EQUAL_FATAL(result, NULL)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "filter", 6), 0)

    riak_search_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_search_options_filter passed")
}

void
test_search_options_df() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_search_options *opt = riak_search_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary default_field;
    default_field.data = (riak_uint8_t*)"default_field";
    default_field.len = 13;
    riak_search_options_set_df(cfg, opt, &default_field);
    CU_ASSERT_EQUAL(riak_search_options_get_has_df(opt), RIAK_TRUE)
    riak_binary *result = riak_search_options_get_df(opt);
    CU_ASSERT_NOT_EQUAL_FATAL(result, NULL)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "default_field", 13), 0)

    riak_search_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_search_options_df passed")
}

void
test_search_options_op() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_search_options *opt = riak_search_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary theoperator;
    theoperator.data = (riak_uint8_t*)"theoperator";
    theoperator.len = 11;
    riak_search_options_set_op(cfg, opt, &theoperator);
    CU_ASSERT_EQUAL(riak_search_options_get_has_op(opt), RIAK_TRUE)
    riak_binary *result = riak_search_options_get_op(opt);
    CU_ASSERT_NOT_EQUAL_FATAL(result, NULL)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "theoperator", 11), 0)

    riak_search_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_search_options_op passed")
}

void
test_search_options_fl() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_search_options *opt = riak_search_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary field_limits;
    field_limits.data = (riak_uint8_t*)"field_limits1";
    field_limits.len = 13;
    riak_search_options_add_fl(cfg, opt, &field_limits);
    field_limits.data = (riak_uint8_t*)"field_limits2";
    riak_search_options_add_fl(cfg, opt, &field_limits);
    CU_ASSERT_EQUAL(riak_search_options_get_n_fl(opt), 2)
    riak_binary **results = riak_search_options_get_fl(opt);
    CU_ASSERT_NOT_EQUAL_FATAL(results, NULL)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(results[0]), "field_limits1", 12), 0)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(results[1]), "field_limits2", 12), 0)

    riak_search_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_search_options_op passed")
}


void
test_search_options_presort() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_search_options *opt = riak_search_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary presort;
    presort.data = (riak_uint8_t*)"presort";
    presort.len = 7;
    riak_search_options_set_presort(cfg, opt, &presort);
    CU_ASSERT_EQUAL(riak_search_options_get_has_presort(opt), RIAK_TRUE)
    riak_binary *result = riak_search_options_get_presort(opt);
    CU_ASSERT_NOT_EQUAL_FATAL(result, NULL)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "presort", 7), 0)

    riak_search_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_search_options_presort passed")
}

void
test_search_decode_response() {
    riak_config     *cfg;
    riak_operation  *rop = NULL;
    riak_connection *cxn = NULL;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", &test_connection_dummy_options);
    CU_ASSERT_EQUAL_FATAL(err, ERIAK_OK)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    // Canned PBC Riak Search Response
    riak_uint8_t bytes[] = { 0x1c,0x0a,0xda,0x01,0x0a,0x18,0x0a,0x02,0x69,
        0x64,0x12,0x12,0x34,0x32,0x35,0x32,0x33,0x37,0x36,0x36,0x39,0x34,0x38,
        0x30,0x36,0x35,0x32,0x38,0x30,0x30,0x0a,0x1b,0x0a,0x04,0x74,0x69,0x6d,
        0x65,0x12,0x13,0x32,0x30,0x31,0x34,0x2d,0x30,0x31,0x2d,0x32,0x30,0x54,
        0x31,0x32,0x3a,0x30,0x35,0x3a,0x33,0x35,0x0a,0x88,0x01,0x0a,0x05,0x74,
        0x77,0x65,0x65,0x74,0x12,0x7f,0x52,0x54,0x20,0x40,0x44,0x61,0x76,0x69,
        0x64,0x5f,0x54,0x65,0x6e,0x6e,0x61,0x6e,0x74,0x3a,0x20,0x44,0x61,0x76,
        0x69,0x64,0x20,0x54,0x65,0x6e,0x6e,0x61,0x6e,0x74,0x20,0x54,0x56,0x20,
        0x46,0x6f,0x72,0x20,0x54,0x6f,0x64,0x61,0x79,0x3a,0x20,0x31,0x31,0x61,
        0x6d,0x20,0x26,0x61,0x6d,0x70,0x3b,0x20,0x34,0x70,0x6d,0x20,0x57,0x61,
        0x74,0x63,0x68,0x20,0x54,0x75,0x72,0x6e,0x20,0x4c,0x65,0x66,0x74,0x20,
        0x28,0x72,0x65,0x70,0x65,0x61,0x74,0x65,0x64,0x20,0x61,0x6e,0x20,0x68,
        0x6f,0x75,0x72,0x20,0x6c,0x61,0x74,0x65,0x72,0x20,0x6f,0x6e,0x20,0x57,
        0x61,0x74,0x63,0x68,0x20,0x2b,0x20,0x31,0x29,0x0a,0x0a,0x23,0x44,0x6f,
        0x63,0x74,0x6f,0x72,0x57,0x68,0x6f,0x0a,0x16,0x0a,0x04,0x75,0x73,0x65,
        0x72,0x12,0x0e,0x42,0x72,0x61,0x6e,0x64,0x6f,0x6e,0x4d,0x48,0x61,0x6c,
        0x6c,0x61,0x6d,0x0a,0xe9,0x01,0x0a,0x18,0x0a,0x02,0x69,0x64,0x12,0x12,
        0x34,0x32,0x35,0x38,0x35,0x39,0x35,0x38,0x35,0x35,0x33,0x30,0x38,0x30,
        0x32,0x31,0x37,0x36,0x0a,0x1b,0x0a,0x04,0x74,0x69,0x6d,0x65,0x12,0x13,
        0x32,0x30,0x31,0x34,0x2d,0x30,0x31,0x2d,0x32,0x32,0x54,0x30,0x35,0x3a,
        0x31,0x36,0x3a,0x35,0x31,0x0a,0x98,0x01,0x0a,0x05,0x74,0x77,0x65,0x65,
        0x74,0x12,0x8e,0x01,0x52,0x54,0x20,0x40,0x57,0x68,0x6f,0x5f,0x4e,0x65,
        0x77,0x73,0x3a,0x20,0x4c,0x61,0x73,0x74,0x20,0x43,0x68,0x61,0x6e,0x63,
        0x65,0x20,0x74,0x6f,0x20,0x56,0x4f,0x54,0x45,0x20,0x66,0x6f,0x72,0x20,
        0x44,0x61,0x76,0x69,0x64,0x20,0x54,0x65,0x6e,0x6e,0x61,0x6e,0x74,0x20,
        0x66,0x6f,0x72,0x20,0x4e,0x61,0x74,0x69,0x6f,0x6e,0x61,0x6c,0x20,0x54,
        0x65,0x6c,0x65,0x76,0x69,0x73,0x69,0x6f,0x6e,0x20,0x41,0x77,0x61,0x72,
        0x64,0x21,0x21,0x21,0x21,0x21,0x3a,0x20,0x56,0x4f,0x54,0x49,0x4e,0x47,
        0x20,0x65,0x6e,0x64,0x73,0x20,0x74,0x6f,0x64,0x61,0x79,0x20,0x61,0x74,
        0x2e,0x2e,0x2e,0x20,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x74,0x2e,0x63,
        0x6f,0x2f,0x4f,0x66,0x4c,0x39,0x68,0x6d,0x70,0x59,0x61,0x7a,0x20,0x23,
        0x64,0x72,0x77,0xe2,0x80,0xa6,0x0a,0x15,0x0a,0x04,0x75,0x73,0x65,0x72,
        0x12,0x0d,0x4d,0x61,0x72,0x6b,0x4f,0x66,0x54,0x68,0x65,0x52,0x65,0x64,
        0x58,0x15,0xe6,0x04,0xb5,0x3e,0x18,0x02
    };

    // Values of the search result encoded in bytes[] above
    char *keys[2][4] = {
        { "id", "time", "tweet", "user" },
        { "id", "time", "tweet", "user" }
    };
    char *values[2][4] = {
        { "425237669480652800",
          "2014-01-20T12:05:35",
          "RT @David_Tennant: David Tennant TV For Today: 11am &amp; 4pm "
          "Watch Turn Left (repeated an hour later on Watch + 1)\n\n#DoctorWho",
          "BrandonMHallam" },
        { "425859585530802176",
          "2014-01-22T05:16:51",
          "RT @Who_News: Last Chance to VOTE for David Tennant for National "
          "Television Award!!!!!: VOTING ends today at... http://t.co/OfL9hmpYaz "
          "#drw…",
          "MarkOfTheRedX" }
    };

    riak_pb_message    pb_response;
    pb_response.data = bytes;
    pb_response.len  = sizeof(bytes);
    riak_search_response *response = NULL;
    riak_boolean_t     done;
    err = riak_search_response_decode(rop, &pb_response, &response, &done);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    CU_ASSERT_EQUAL(riak_search_get_n_docs(response), 2);
    CU_ASSERT_EQUAL(riak_search_get_has_max_score(response), RIAK_TRUE);
    riak_float32_t score = riak_search_get_max_score(response);
    CU_ASSERT(score > 0.35); // Value is 0.353553
    CU_ASSERT(score < 0.36);
    CU_ASSERT_EQUAL(riak_search_get_has_num_found(response), RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_search_get_num_found(response), 2);
    for(int i = 0; i < riak_search_get_n_docs(response); i++) {
        riak_search_doc *doc = riak_search_get_doc(response, i);
        for(int j = 0; j < riak_search_doc_get_n_fields(doc); j++) {
            riak_pair *pair = riak_search_doc_get_field(doc, j);
            riak_uint8_t *key = riak_binary_data(riak_pair_get_key(pair));
            riak_size_t   keylen = riak_binary_len(riak_pair_get_key(pair));
            CU_ASSERT_EQUAL(memcmp(key, keys[i][j], keylen), 0)
            riak_uint8_t *value = riak_binary_data(riak_pair_get_value(pair));
            riak_size_t   valuelen = riak_binary_len(riak_pair_get_value(pair));
            CU_ASSERT_EQUAL(memcmp(value, values[i][j], valuelen), 0)
        }
    }

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_search_response_free(cfg, &response);
    riak_config_free(&cfg);
    CU_PASS("test_search_decode_response passed")
}
