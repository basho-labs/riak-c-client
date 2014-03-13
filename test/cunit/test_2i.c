/*********************************************************************
 *
 * test_2i.c: Riak C Unit testing for Secondary Index Message
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
test_2i_options_qtype() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    CU_ASSERT_EQUAL(riak_2i_options_get_is_range_query(opt), RIAK_FALSE);
    riak_2i_options_set_range_query(opt);
    CU_ASSERT_EQUAL(riak_2i_options_get_is_range_query(opt), RIAK_TRUE);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_qtype passed")
}

void
test_2i_options_key() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary key;
    key.data = (riak_uint8_t*)"some key";
    key.len = 8;
    riak_2i_options_set_key(cfg, opt, &key);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_key(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_2i_options_get_key(opt)), "some key", 8), 0);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_key passed")
}

void
test_2i_options_range_min() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary range;
    range.data = (riak_uint8_t*)"some range";
    range.len = 10;
    riak_2i_options_set_range_min(cfg, opt, &range);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_range_min(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_2i_options_get_range_min(opt)), "some range", 10), 0);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_range_min passed")
}

void
test_2i_options_range_max() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary range;
    range.data = (riak_uint8_t*)"some other range";
    range.len = 16;
    riak_2i_options_set_range_max(cfg, opt, &range);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_range_max(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_2i_options_get_range_max(opt)), "some other range", 16), 0);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_range_max passed")
}

void
test_2i_options_return_terms() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_2i_options_set_return_terms(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_return_terms(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_2i_options_get_return_terms(opt), RIAK_TRUE);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_range_max passed")
}

void
test_2i_options_stream() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_2i_options_set_stream(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_stream(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_2i_options_get_stream(opt), RIAK_TRUE);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_stream passed")
}

void
test_2i_options_max_results() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_2i_options_set_max_results(opt, 42);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_max_results(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_2i_options_get_max_results(opt), 42);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_max_results passed")
}

void
test_2i_options_continuation() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary range;
    range.data = (riak_uint8_t*)"some continuation";
    range.len = 17;
    riak_2i_options_set_continuation(cfg, opt, &range);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_continuation(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_2i_options_get_continuation(opt)), "some continuation", 17), 0);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_continuation passed")
}

void
test_2i_options_timeout() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_2i_options_set_timeout(opt, 4242);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_timeout(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_2i_options_get_timeout(opt), 4242);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_timeout passed")
}

void
test_2i_options_type() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary range;
    range.data = (riak_uint8_t*)"some type";
    range.len = 9;
    riak_2i_options_set_continuation(cfg, opt, &range);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_continuation(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_2i_options_get_continuation(opt)), "some type", 9), 0);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_type passed")
}

void
test_2i_options_term_regex() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary range;
    range.data = (riak_uint8_t*)"some regex";
    range.len = 10;
    riak_2i_options_set_continuation(cfg, opt, &range);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_continuation(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_2i_options_get_continuation(opt)), "some regex", 10), 0);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_term_regex passed")
}

void
test_2i_options_pagination_sort() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_2i_options *opt = riak_2i_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_2i_options_set_pagination_sort(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_2i_options_get_has_pagination_sort(opt), RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_2i_options_get_pagination_sort(opt), RIAK_TRUE);

    riak_2i_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_2i_options_pagination_sort passed")
}

void
test_2i_response_decode() {
    riak_config     *cfg;
    riak_operation  *rop = NULL;
    riak_connection *cxn = NULL;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_uint8_t bytes0[] = { 0x1a,0x0a,0x12,0x34,0x32,0x32,0x32,0x38,0x38,0x32,0x33,0x33,0x34,0x30,0x36,0x36,0x38,0x31,0x30,0x38,0x38 };
    riak_uint8_t bytes1[] = { 0x1a,0x0a,0x12,0x34,0x32,0x32,0x32,0x39,0x32,0x31,0x37,0x39,0x30,0x39,0x37,0x34,0x38,0x39,0x34,0x30,0x38 };
    riak_uint8_t bytes2[] = { 0x1a,0x0a,0x12,0x34,0x32,0x32,0x38,0x38,0x37,0x30,0x38,0x30,0x32,0x32,0x36,0x39,0x39,0x36,0x32,0x32,0x36 };
    riak_uint8_t bytes3[] = { 0x1a,0x20,0x01 };
    riak_uint8_t *bytes[] = { bytes0, bytes1, bytes2, bytes3 };
    riak_int32_t len[]    = { sizeof(bytes0), sizeof(bytes1), sizeof(bytes2), sizeof(bytes3) };

    riak_pb_message       pb_response;
    riak_2i_response *response = NULL;
    riak_boolean_t        done = RIAK_FALSE;
    for(int i = 0; i < 4; i++) {
        pb_response.data = bytes[i];
        pb_response.len  = len[i];
        err = riak_2i_response_decode(rop, &pb_response, &response, &done);
        CU_ASSERT_FATAL(err == ERIAK_OK)
    }
    CU_ASSERT_EQUAL_FATAL(riak_2i_get_n_keys(response), 3);
    riak_binary** keys = riak_2i_get_keys(response);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(keys[0]), "422288233406681088", riak_binary_len(keys[0])),0)
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(keys[1]), "422292179097489408", riak_binary_len(keys[1])),0)
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(keys[2]), "422887080226996226", riak_binary_len(keys[2])),0)

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_2i_response_free(cfg, &response);
    riak_config_free(&cfg);
    CU_PASS("test_2i_response_decode passed")
}
