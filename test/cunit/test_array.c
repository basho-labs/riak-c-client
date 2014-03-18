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

/**
 * @brief Create some empty arrays and make sure the length is consistent
 */
void
test_array_empty() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 0);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);

    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_empty passed")
}

/**
 * @brief Expand a array of 10 with a capacity of 10 elements
 * to 100 by pushing at the back and make sure the length makes sense
 */
void
test_array_push() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    for(riak_uint64_t i = 0; i < 100; i++) {
    err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_push passed")
}

/**
 * @brief Expand and array with a capacity of 10 elements to 100
 * by inserting at the front, making sure ordering is as expected
 */
void
test_array_insert() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    for(riak_uint64_t i = 1; i <= 100; i++) {
        err = riak_array_insert(ra, 0, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    for(riak_uint64_t i = 1; i <= 100; i++) {
        riak_uint64_t value;
        err = riak_array_pop(ra,(void**)&value);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
        CU_ASSERT_EQUAL_FATAL(i,value)
    }
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_insert passed")
}

/**
 * @brief Create an empty array
 */
void
test_array_bad_insert() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    for(riak_uint64_t i = 1; i <= 100; i++) {
    err = riak_array_insert(ra, i, (void*)i);
        CU_ASSERT_EQUAL(err,ERIAK_OUT_OF_RANGE)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_bad_insert passed")
}

/**
 * @brief Create a small array and insert 100 elements at the front.
 * Then get the existing value of each 100 elements and verify that
 * the set operation has replaced the old value with a new one.
 */
void
test_array_set_get() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    for(riak_uint64_t i = 1; i <= 100; i++) {
    err = riak_array_insert(ra, 0, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    // Overwrite 100 values with i-1
    for(riak_uint64_t i = 0; i < 100; i++) {
        riak_uint64_t old_value;
        err = riak_array_get(ra, i, (void**)&old_value);
        CU_ASSERT_EQUAL(err,ERIAK_OK)
        err = riak_array_set(ra, i, (void*)i);
        CU_ASSERT_EQUAL(err,ERIAK_OK)
        riak_uint64_t new_value;
        err = riak_array_get(ra, i, (void**)&new_value);
        CU_ASSERT_EQUAL(err,ERIAK_OK)
        CU_ASSERT_EQUAL_FATAL(100-old_value,new_value)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_set_get passed")
}

/**
 * @brief Create a small, empty array and try to set values
 * at invalid positions in the array.
 */
void
test_array_bad_set() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    // Should fail every time
    for(riak_uint64_t i = 0; i < 100; i++) {
    err = riak_array_set(ra, i, (void*)i);
        CU_ASSERT_EQUAL(err,ERIAK_OUT_OF_RANGE)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_bad_set passed")
}

/**
 * @brief Create a small, empty array and try get values
 * at invalid positions in the array.
 */
void
test_array_bad_get() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    // Should fail every time
    for(riak_uint64_t i = 0; i < 100; i++) {
        riak_uint64_t value;
        err = riak_array_get(ra, i, (void**)&value);
        CU_ASSERT_EQUAL(err,ERIAK_OUT_OF_RANGE)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_bad_get passed")
}

/**
 * @brief Insert 100 elements at the end of an array, then pop them all
 * off in order and make sure the values are as expected.
 */
void
test_array_pop() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    for(riak_uint64_t i = 1; i <= 100; i++) {
    err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    for(riak_uint64_t i = 100; i > 0; i--) {
        riak_uint64_t value;
        err = riak_array_pop(ra, (void**)&value);
        CU_ASSERT_EQUAL(err,ERIAK_OK)
        CU_ASSERT_EQUAL_FATAL(i,value)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_pop passed")
}

/**
 * @brief Try to pop a value off an empty array. No workie.
 */
void
test_array_empty_pop() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_uint64_t value;
    err = riak_array_pop(ra, (void**)&value);
    CU_ASSERT_EQUAL(err,ERIAK_OUT_OF_RANGE)
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_empty_pop passed")
}

/**
 * @brief Load up an array with 100 values then remove them all.
 * Verify you then have an empty array.
 */
void
test_array_remove() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    for(riak_uint64_t i = 0; i < 100; i++) {
    err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    for(riak_uint64_t i = 0; i < 100; i++) {
        err = riak_array_remove(ra, 0);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_remove passed")
}

/**
 * @brief Try to remove an element from an empty array, then
 * try to remove an element at an invalid position in the array.
 */
void
test_array_bad_remove() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    err = riak_array_remove(ra, 0);
    CU_ASSERT_EQUAL(err, ERIAK_OUT_OF_RANGE)
    for(riak_uint64_t i = 0; i < 10; i++) {
    err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    err = riak_array_remove(ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OUT_OF_RANGE)

    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_bad_remove passed")
}

/**
 * @brief Load up an array with 100 elements.  Verify that
 * the length returned is what is expected.
 */
void
test_array_length() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    CU_ASSERT_EQUAL(riak_array_length(ra), 0);

    for(riak_uint64_t i = 0; i < 100; i++) {
    err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    CU_ASSERT_EQUAL(riak_array_length(ra), 100);
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_length passed")
}

int
test_array_int_comparer(const void *a,
                        const void *b) {
    riak_int64_t a_int = *(riak_int64_t*)a;
    riak_int64_t b_int = *(riak_int64_t*)b;
    return a_int - b_int;
}

/**
 * @brief Load up an array with 100 elements in reverse order.
 * Sort them and make sure the new ordering is correct.
 */
void
test_array_sort() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_array *ra;
    err = riak_array_new(cfg, &ra, 10);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    for(riak_int64_t i = 99; i >= 0; i--) {
        err = riak_array_push(ra, (void*)i);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    }
    riak_array_sort(ra, test_array_int_comparer);

    for(riak_int64_t i = 0; i < 100; i++) {
        riak_int64_t value;
        err = riak_array_get(ra, i, (void**)&value);
        CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
        CU_ASSERT_EQUAL(i, value)
    }
    riak_array_free(&ra);
    riak_config_free(&cfg);
    CU_PASS("test_array_sort passed")
}
