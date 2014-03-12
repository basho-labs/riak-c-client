/*********************************************************************
 *
 * test_array.c: Riak C Unit testing for riak_array
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

void
test_array_empty() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 0);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);

    ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_push() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    for(riak_uint64_t i = 0; i < 100; i++) {
        err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_insert() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    for(riak_uint64_t i = 1; i <= 100; i++) {
        err = riak_array_insert(ra, 0, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    for(riak_uint64_t i = 1; i <= 100; i++) {
        riak_uint64_t value = (riak_uint64_t)riak_array_pop(ra);
        CU_ASSERT_EQUAL_FATAL(i,value)
    }
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_bad_insert() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    for(riak_uint64_t i = 1; i <= 100; i++) {
        err = riak_array_insert(ra, i, (void*)i);
        CU_ASSERT_EQUAL(err,ERIAK_OUT_OF_RANGE)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_set_get() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    for(riak_uint64_t i = 1; i <= 100; i++) {
        err = riak_array_insert(ra, 0, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    // Overwrite 100 values with i-1
    for(riak_uint64_t i = 0; i < 100; i++) {
        riak_uint64_t old_value = (riak_uint64_t)riak_array_get(ra, i);
        err = riak_array_set(ra, i, (void*)i);
        CU_ASSERT_EQUAL(err,ERIAK_OK)
        riak_uint64_t new_value = (riak_uint64_t)riak_array_get(ra, i);
        CU_ASSERT_EQUAL_FATAL(100-old_value,new_value)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_bad_set() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    // Should fail every time
    for(riak_uint64_t i = 0; i < 100; i++) {
        err = riak_array_set(ra, i, (void*)i);
        CU_ASSERT_EQUAL(err,ERIAK_OUT_OF_RANGE)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_bad_get() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    // Should fail every time
    for(riak_uint64_t i = 0; i < 100; i++) {
        void * value = riak_array_get(ra, i);
        CU_ASSERT_EQUAL(value,NULL)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_pop() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    for(riak_uint64_t i = 1; i <= 100; i++) {
        err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    for(riak_uint64_t i = 100; i > 0; i--) {
        riak_uint64_t value = (riak_uint64_t)riak_array_pop(ra);
        CU_ASSERT_EQUAL_FATAL(i,value)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_empty_pop() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    void *nada = riak_array_pop(ra);
    CU_ASSERT_EQUAL(nada, NULL);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_remove() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    for(riak_uint64_t i = 0; i < 100; i++) {
        err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    for(riak_uint64_t i = 0; i < 100; i++) {
        riak_uint64_t value = (riak_uint64_t)riak_array_remove(ra, 0);
        CU_ASSERT_EQUAL(i, value)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_bad_remove() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)

    void *value = riak_array_remove(ra, 0);
    CU_ASSERT_EQUAL(value, NULL)
    for(riak_uint64_t i = 0; i < 10; i++) {
        err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    value = riak_array_remove(ra, 10);
    CU_ASSERT_EQUAL(value, NULL)

    riak_array_free(&ra);
    riak_config_free(&cfg);
}

void
test_array_length() {
    riak_config *cfg;
    riak_error    err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra = riak_array_new(cfg, 10);
    CU_ASSERT_NOT_EQUAL_FATAL(ra,NULL)
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);

    for(riak_uint64_t i = 0; i < 100; i++) {
        err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    riak_array_free(&ra);
    riak_config_free(&cfg);
}
