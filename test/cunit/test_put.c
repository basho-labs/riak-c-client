/*********************************************************************
 *
 * test_put.c: Riak C Unit testing for Put Message
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
test_put_options_vclock() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)
    riak_binary vclock;
    vclock.data = (riak_uint8_t*)"12345";
    vclock.len = 5;

    riak_put_options_set_vclock(cfg, opt, &vclock);
    CU_ASSERT_EQUAL(riak_put_options_get_has_vclock(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_put_options_get_vclock(opt)), "12345", 5), 0)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_w passed")
}

void
test_put_options_w() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_w(opt, 5);
    CU_ASSERT_EQUAL(riak_put_options_get_has_w(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_w(opt), 5)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_w passed")
}

void
test_put_options_dw() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_dw(opt, 5);
    CU_ASSERT_EQUAL(riak_put_options_get_has_dw(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_dw(opt), 5)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_dw passed")
}

void
test_put_options_return_body()  {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_return_body(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_put_options_get_has_return_body(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_return_body(opt), RIAK_TRUE)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_return_body passed")
}

void
test_put_options_pw() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_pw(opt, 5);
    CU_ASSERT_EQUAL(riak_put_options_get_has_pw(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_pw(opt), 5)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_pw passed")
}

void
test_put_options_if_not_modified() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_if_not_modified(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_put_options_get_has_if_not_modified(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_if_not_modified(opt), RIAK_TRUE)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_if_not_modified passed")
}

void
test_put_options_if_none_match() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_if_none_match(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_put_options_get_has_if_none_match(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_if_none_match(opt), RIAK_TRUE)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_if_none_match passed")
}

void
test_put_options_return_head() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_return_head(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_put_options_get_has_return_head(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_return_head(opt), RIAK_TRUE)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_return_head passed")
}
void
test_put_options_timeout() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_timeout(opt, 5);
    CU_ASSERT_EQUAL(riak_put_options_get_has_timeout(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_timeout(opt), 5)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_timeout passed")
}

void
test_put_options_asis() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_asis(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_put_options_get_has_asis(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_asis(opt), RIAK_TRUE)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_asis passed")
}
void
test_put_options_sloppy_quorum() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_sloppy_quorum(opt, RIAK_TRUE);
    CU_ASSERT_EQUAL(riak_put_options_get_has_sloppy_quorum(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_sloppy_quorum(opt), RIAK_TRUE)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_sloppy_quorum passed")
}
void
test_put_options_n_val() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_put_options *opt = riak_put_options_new(cfg);
    CU_ASSERT_FATAL(opt != NULL)

    riak_put_options_set_n_val(opt, 5);
    CU_ASSERT_EQUAL(riak_put_options_get_has_n_val(opt), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_put_options_get_n_val(opt), 5)

    riak_put_options_free(cfg, &opt);
    riak_config_free(&cfg);
    CU_PASS("test_put_options_n_val passed")
}

void
test_put_decode_response() {
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
        0x0c,0x0a,0x24,0x0a,0x00,0x2a,0x16,0x31,0x66,0x56,0x72,0x65,0x67,0x49,
        0x6c,0x67,0x36,0x76,0x52,0x75,0x54,0x53,0x31,0x49,0x30,0x52,0x32,0x58,
        0x45,0x38,0xa6,0xa4,0xa8,0x95,0x05,0x40,0xb1,0xe6,0x38,0x0a,0x24,0x0a,
        0x00,0x2a,0x16,0x36,0x5a,0x45,0x64,0x42,0x62,0x4f,0x4d,0x70,0x35,0x31,
        0x44,0x41,0x76,0x77,0x4e,0x6e,0x62,0x47,0x49,0x41,0x71,0x38,0xa2,0xa4,
        0xa8,0x95,0x05,0x40,0xbf,0xff,0x25,0x12,0x23,0x6b,0xce,0x61,0x60,0x60,
        0x60,0xcc,0x60,0xca,0x05,0x52,0x1c,0x47,0x83,0x36,0x72,0x07,0x65,0x9d,
        0xfd,0x96,0xc1,0x94,0xc8,0x94,0xc7,0xca,0xa0,0xd6,0x27,0x77,0x86,0x2f,
        0x0b,0x00
    };
    riak_pb_message    pb_response;
    pb_response.data = bytes;
    pb_response.len  = sizeof(bytes);
    riak_put_response *response;
    riak_boolean_t     done;
    err = riak_put_response_decode(rop, &pb_response, &response, &done);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    CU_ASSERT_EQUAL_FATAL(riak_put_get_has_vclock(response), RIAK_TRUE);
    riak_uint8_t vclock_bytes[] = {
        0x6b,0xce,0x61,0x60,0x60,0x60,0xcc,0x60,0xca,0x05,0x52,0x1c,0x47,0x83,
        0x36,0x72,0x07,0x65,0x9d,0xfd,0x96,0xc1,0x94,0xc8,0x94,0xc7,0xca,0xa0,
        0xd6,0x27,0x77,0x86,0x2f,0x0b,0x00
    };
    riak_binary *vclock = riak_put_get_vclock(response);
    CU_ASSERT_FATAL(vclock != NULL)
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(vclock), vclock_bytes, riak_binary_len(vclock)), 0)
    CU_ASSERT_EQUAL_FATAL(riak_put_get_has_key(response), RIAK_FALSE);
    CU_ASSERT_EQUAL_FATAL(riak_put_get_n_content(response), 2);
    riak_object** objects = riak_put_get_content(response);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(riak_object_get_vtag(objects[0])), "1fVregIlg6vRuTS1I0R2XE", 22), 0)
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(riak_object_get_vtag(objects[1])), "6ZEdBbOMp51DAvwNnbGIAq", 22), 0)

    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_put_response_free(cfg, &response);
    riak_config_free(&cfg);
    CU_PASS("test_put_decode_response passed")
}

