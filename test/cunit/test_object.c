/*********************************************************************
 *
 * test_put.c: Riak C Unit testing for Riak Object
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
#include "riak_binary-internal.h"

void
test_riak_object_bucket() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_binary bucket;
    bucket.data = (riak_uint8_t *)"bucket";
    bucket.len = 6;
    err = riak_object_set_bucket(cfg, obj, &bucket);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_binary *result = riak_object_get_bucket(obj);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "bucket", 6), 0)

    riak_object_free(cfg, &obj);
}

void
test_riak_object_key() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_binary key;
    key.data = (riak_uint8_t *)"key";
    key.len = 3;
    err = riak_object_set_key(cfg, obj, &key);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    CU_ASSERT_EQUAL_FATAL(riak_object_get_has_key(obj), RIAK_TRUE)
    riak_binary *result = riak_object_get_key(obj);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "key", 3), 0)

    riak_object_free(cfg, &obj);
}

#define RIAK_TEST_LOREM_IPSUM \
        "Lorem ipsum dolor sit amet, consectetur adipiscing " \
        "elit. Maecenas vehicula quam vel nibh pretium ullamcorper. " \
        "Donec id odio a risus rutrum ultricies ut nec purus. Nulla " \
        "facilisi. Duis porta non ligula fermentum viverra. Praesent " \
        "sit amet bibendum ligula. Mauris placerat sed erat eget venenatis. " \
        "Phasellus ipsum mi, accumsan ac ligula a, imperdiet euismod mi. " \
        "Mauris interdum dignissim tortor, at tincidunt magna tempus a."
void
test_riak_object_value() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_binary value;
    value.data = (riak_uint8_t *)RIAK_TEST_LOREM_IPSUM;
    value.len = strlen(RIAK_TEST_LOREM_IPSUM);
    err = riak_object_set_value(cfg, obj, &value);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_binary *result = riak_object_get_value(obj);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), RIAK_TEST_LOREM_IPSUM, value.len), 0)

    riak_object_free(cfg, &obj);

    obj = riak_object_new(cfg);
    err = riak_object_set_value_shallow_copy(cfg, obj, &value);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    result = riak_object_get_value(obj);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), RIAK_TEST_LOREM_IPSUM, value.len), 0)
    // Should point to the same data
    CU_ASSERT(riak_binary_data(result) == value.data)

    riak_object_free(cfg, &obj);
}

void
test_riak_object_charset() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_binary charset;
    charset.data = (riak_uint8_t *)"charset";
    charset.len = 7;
    err = riak_object_set_charset(cfg, obj, &charset);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    CU_ASSERT_EQUAL_FATAL(riak_object_get_has_charset(obj), RIAK_TRUE)
    riak_binary *result = riak_object_get_charset(obj);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "charset", 7), 0)

    riak_object_free(cfg, &obj);
}

void
test_riak_object_last_mod() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_object_set_last_mod(obj, 123456789);

    CU_ASSERT_EQUAL_FATAL(riak_object_get_has_last_mod(obj), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_object_get_last_mod(obj), 123456789);

    riak_object_free(cfg, &obj);
}

void
test_riak_object_last_mod_usecs() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_object_set_last_mod_usecs(obj, 987654321);

    CU_ASSERT_EQUAL_FATAL(riak_object_get_has_last_mod_usecs(obj), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_object_get_last_mod_usecs(obj), 987654321);

    riak_object_free(cfg, &obj);
}

void
test_riak_object_content_type() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_binary content_type;
    content_type.data = (riak_uint8_t *)"content_type";
    content_type.len = 13;
    err = riak_object_set_content_type(cfg, obj, &content_type);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    CU_ASSERT_EQUAL_FATAL(riak_object_get_has_content_type(obj), RIAK_TRUE)
    riak_binary *result = riak_object_get_content_type(obj);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "content_type", 13), 0)

    riak_object_free(cfg, &obj);
}

void
test_riak_object_content_encoding() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_binary encoding;
    encoding.data = (riak_uint8_t *)"encoding";
    encoding.len = 8;
    err = riak_object_set_content_encoding(cfg, obj, &encoding);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    CU_ASSERT_EQUAL_FATAL(riak_object_get_has_content_encoding(obj), RIAK_TRUE)
    riak_binary *result = riak_object_get_content_encoding(obj);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "encoding", 8), 0)

    riak_object_free(cfg, &obj);
}

void
test_riak_object_deleted() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_object_set_deleted(obj, RIAK_TRUE);

    CU_ASSERT_EQUAL_FATAL(riak_object_get_has_deleted(obj), RIAK_TRUE)
    CU_ASSERT_EQUAL(riak_object_get_deleted(obj), RIAK_TRUE);

    riak_object_free(cfg, &obj);
}

void
test_riak_object_vtag() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)
    riak_binary vtag;
    vtag.data = (riak_uint8_t *)"vtag";
    vtag.len = 4;
    err = riak_object_set_vtag(cfg, obj, &vtag);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    CU_ASSERT_EQUAL_FATAL(riak_object_get_has_vtag(obj), RIAK_TRUE)
    riak_binary *result = riak_object_get_vtag(obj);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), "vtag", 4), 0)

    riak_object_free(cfg, &obj);
}

void
test_riak_object_links() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)

    char buffer[128];
    riak_binary *bin;
    riak_link   *link;
    for(int i = 0; i < 3; i++) {
        link = riak_object_new_link(cfg, obj);
        CU_ASSERT_FATAL(link != NULL)
        sprintf(buffer, "%s%d", "bucket", i);
        bin = riak_binary_copy_from_string(cfg, buffer);
        riak_link_set_bucket(cfg, link, bin);
        riak_free(cfg, &bin);
        sprintf(buffer, "%s%d", "key", i);
        bin = riak_binary_copy_from_string(cfg, buffer);
        riak_link_set_key(cfg, link, bin);
        riak_free(cfg, &bin);
        sprintf(buffer, "%s%d", "tag", i);
        bin = riak_binary_copy_from_string(cfg, buffer);
        riak_link_set_tag(cfg, link, bin);
        riak_free(cfg, &bin);
    }
    riak_binary *result;
    int len;
    for(int i = 0; i < 3; i++) {
        link = riak_object_get_link(obj, i);
        sprintf(buffer, "%s%d", "bucket", i);
        result = riak_link_get_bucket(link);
        len = strlen(buffer);
        CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), buffer, len), 0)
        sprintf(buffer, "%s%d", "key", i);
        result = riak_link_get_key(link);
        len = strlen(buffer);
        CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), buffer, len), 0)
        sprintf(buffer, "%s%d", "tag", i);
        result = riak_link_get_tag(link);
        len = strlen(buffer);
        CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), buffer, len), 0)
    }

    riak_object_free(cfg, &obj);
}

void
test_riak_object_usermeta() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)

    char buffer[128];
    riak_binary *bin;
    riak_pair   *pair;
    for(int i = 0; i < 3; i++) {
        pair = riak_object_new_usermeta(cfg, obj);
        CU_ASSERT_FATAL(pair != NULL)
        sprintf(buffer, "%s%d", "value", i);
        bin = riak_binary_copy_from_string(cfg, buffer);
        riak_pair_set_value(cfg, pair, bin);
        riak_free(cfg, &bin);
        sprintf(buffer, "%s%d", "key", i);
        bin = riak_binary_copy_from_string(cfg, buffer);
        riak_pair_set_key(cfg, pair, bin);
        riak_free(cfg, &bin);
    }
    riak_binary *result;
    int len;
    for(int i = 0; i < 3; i++) {
        pair = riak_object_get_usermeta(obj, i);
        sprintf(buffer, "%s%d", "value", i);
        result = riak_pair_get_value(pair);
        len = strlen(buffer);
        CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), buffer, len), 0)
        sprintf(buffer, "%s%d", "key", i);
        result = riak_pair_get_key(pair);
        len = strlen(buffer);
        CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), buffer, len), 0)
    }

    riak_object_free(cfg, &obj);
}

void
test_riak_object_indexes() {
    riak_config *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    riak_object *obj = riak_object_new(cfg);
    CU_ASSERT_FATAL(obj != NULL)

    char buffer[128];
    riak_binary *bin;
    riak_pair   *pair;
    for(int i = 0; i < 3; i++) {
        pair = riak_object_new_index(cfg, obj);
        CU_ASSERT_FATAL(pair != NULL)
        sprintf(buffer, "%s%d", "value", i);
        bin = riak_binary_copy_from_string(cfg, buffer);
        riak_pair_set_value(cfg, pair, bin);
        riak_free(cfg, &bin);
        sprintf(buffer, "%s%d", "key", i);
        bin = riak_binary_copy_from_string(cfg, buffer);
        riak_pair_set_key(cfg, pair, bin);
        riak_free(cfg, &bin);
    }
    riak_binary *result;
    int len;
    for(int i = 0; i < 3; i++) {
        pair = riak_object_get_index(obj, i);
        sprintf(buffer, "%s%d", "value", i);
        result = riak_pair_get_value(pair);
        len = strlen(buffer);
        CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), buffer, len), 0)
        sprintf(buffer, "%s%d", "key", i);
        result = riak_pair_get_key(pair);
        len = strlen(buffer);
        CU_ASSERT_EQUAL(memcmp(riak_binary_data(result), buffer, len), 0)
    }

    riak_object_free(cfg, &obj);
}

