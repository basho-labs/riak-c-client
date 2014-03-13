/*********************************************************************
 *
 * test_get.c: Riak C Unit testing for Get Message
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
test_get_options_r() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_get_options_set_r(opt, 5);
    CU_ASSERT_EQUAL(riak_get_options_get_has_r(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_get_options_get_r(opt), 5)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_r passed")
}

void
test_get_options_pr() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_get_options_set_pr(opt, 5);
    CU_ASSERT_EQUAL(riak_get_options_get_has_pr(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_get_options_get_pr(opt), 5)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_pr passed")
}

void
test_get_options_basic_quorum() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_get_options_set_basic_quorum(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_get_options_get_has_basic_quorum(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_get_options_get_basic_quorum(opt), RIAK_TRUE)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_basic_quorum passed")
}

void
test_get_options_notfound_ok() {
    riak_config     *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_get_options_set_notfound_ok(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_get_options_get_has_notfound_ok(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_get_options_get_notfound_ok(opt), RIAK_TRUE)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_notfound_ok passed")
}

void
test_get_options_if_modified() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_binary alt;
    alt.data = (riak_uint8_t*)"12345";
    alt.len  = 5;

    riak_get_options_set_if_modified(cfg, opt, &alt);
    CU_ASSERT_EQUAL(riak_get_options_get_has_if_modified(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_binary_len(riak_get_options_get_if_modified(opt)), 5)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_get_options_get_if_modified(opt)), "12345", 5), 0)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_if_modified passed")
}

void
test_get_options_head() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_get_options_set_head(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_get_options_get_has_head(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_get_options_get_head(opt), RIAK_TRUE)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_head passed")
}

void
test_get_options_deletedvclock() {
    riak_config     *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_get_options_set_deletedvclock(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_get_options_get_has_deletedvclock(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_get_options_get_deletedvclock(opt), RIAK_TRUE)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_deletedvclock passed")
}

void
test_get_options_timeout() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_get_options_set_timeout(opt, 5);
    CU_ASSERT_EQUAL(riak_get_options_get_has_timeout(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_get_options_get_timeout(opt), 5)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_timeout passed")
}

void
test_get_options_sloppy_quorum() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_get_options_set_sloppy_quorum(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_get_options_get_has_sloppy_quorum(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_get_options_get_sloppy_quorum(opt), RIAK_TRUE)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_sloppy_quorum passed")
}

void
test_get_options_n_val() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_get_options *opt = riak_get_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_get_options_set_n_val(opt, 5);
    CU_ASSERT_EQUAL(riak_get_options_get_has_n_val(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_get_options_get_n_val(opt), 5)

    riak_get_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_get_options_n_val passed")
}

void
test_get_decode_response() {
    riak_config              *cfg;
    riak_operation           *rop = NULL;
    riak_connection          *cxn = NULL;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_binary bucket;
    bucket.data = (riak_uint8_t*)"test";
    bucket.len = 4;
    riak_binary key;
    key.data = (riak_uint8_t*)"riakc";
    key.len = 5;
    riak_operation_set_bucket(rop, &bucket);
    riak_operation_set_key(rop, &key);

    riak_uint8_t bytes[] = {
        0x0a,0x0a,0x43,0x0a,0x0d,0x7b,0x22,0x62,0x61,0x72,0x22,0x3a,0x22,0x62,0x61,0x7a,
        0x22,0x7d,0x12,0x10,0x61,0x70,0x70,0x6c,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x2f,
        0x6a,0x73,0x6f,0x6e,0x2a,0x16,0x31,0x70,0x66,0x49,0x76,0x38,0x51,0x52,0x74,0x6c,
        0x31,0x36,0x65,0x64,0x4c,0x42,0x51,0x6b,0x68,0x54,0x7a,0x63,0x38,0xc2,0x86,0x9f,
        0x95,0x05,0x40,0xec,0xf5,0x31,0x0a,0x43,0x0a,0x0d,0x7b,0x22,0x62,0x61,0x72,0x22,
        0x3a,0x22,0x62,0x61,0x7a,0x22,0x7d,0x12,0x10,0x61,0x70,0x70,0x6c,0x69,0x63,0x61,
        0x74,0x69,0x6f,0x6e,0x2f,0x6a,0x73,0x6f,0x6e,0x2a,0x16,0x33,0x4b,0x5a,0x57,0x77,
        0x6a,0x31,0x69,0x54,0x63,0x51,0x70,0x6f,0x58,0x37,0x74,0x36,0x50,0x6c,0x72,0x54,
        0x53,0x38,0xbf,0x86,0x9f,0x95,0x05,0x40,0x80,0x85,0x29,0x0a,0x43,0x0a,0x0d,0x7b,
        0x22,0x62,0x61,0x72,0x22,0x3a,0x22,0x62,0x61,0x7a,0x22,0x7d,0x12,0x10,0x61,0x70,
        0x70,0x6c,0x69,0x63,0x61,0x74,0x69,0x6f,0x6e,0x2f,0x6a,0x73,0x6f,0x6e,0x2a,0x16,
        0x33,0x4c,0x7a,0x56,0x6b,0x6f,0x4e,0x4f,0x73,0x39,0x62,0x73,0x4c,0x41,0x63,0x63,
        0x6d,0x73,0x33,0x52,0x41,0x48,0x38,0xbc,0x86,0x9f,0x95,0x05,0x40,0xeb,0xdc,0x34,
        0x12,0x23,0x6b,0xce,0x61,0x60,0x60,0x60,0xcc,0x60,0xca,0x05,0x52,0x1c,0x47,0x83,
        0x36,0x72,0x07,0x65,0x9d,0xfd,0x96,0xc1,0x94,0xc8,0x9c,0xc7,0xca,0xe0,0x64,0x2f,
        0x73,0x86,0x2f,0x0b,0x00
    };
    riak_pb_message    pb_response;
    pb_response.data = bytes;
    pb_response.len  = sizeof(bytes);
    riak_get_response *response = NULL;
    riak_boolean_t     done;
    err = riak_get_response_decode(rop, &pb_response, &response, &done);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    CU_ASSERT_EQUAL(riak_get_get_deleted(response), RIAK_FALSE);
    CU_ASSERT_EQUAL(riak_get_get_has_vclock(response), RIAK_TRUE);
    riak_uint8_t vclock_bytes[] = {
        0x6b,0xce,0x61,0x60,0x60,0x60,0xcc,0x60,0xca,0x05,0x52,0x1c,0x47,0x83,
        0x36,0x72,0x07,0x65,0x9d,0xfd,0x96,0xc1,0x94,0xc8,0x9c,0xc7,0xca,0xe0,
        0x64,0x2f,0x73,0x86,0x2f,0x0b,0x00
    };
    riak_binary *vclock = riak_get_get_vclock(response);
    CU_ASSERT_FATAL(vclock != NULL)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(vclock), vclock_bytes, riak_binary_len(vclock)), 0)
    CU_ASSERT_EQUAL(riak_get_get_has_unmodified(response), RIAK_FALSE)
    CU_ASSERT_EQUAL(riak_get_get_n_content(response), 3)
    riak_object **objects = riak_get_get_content(response);
    for(int i = 0; i < riak_get_get_n_content(response); i++) {
        riak_object *obj = objects[i];
        riak_uint8_t *value = riak_binary_data(riak_object_get_value(obj));
        CU_ASSERT_EQUAL_FATAL(memcmp(value, "{\"bar\":\"baz\"}", riak_binary_len(riak_object_get_value(obj))), 0)
    }

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_get_response_free(cfg, &response);
    riak_config_free(&cfg);
    CU_PASS("test_get_decode_response passed")
}

