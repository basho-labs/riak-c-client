/*********************************************************************
 *
 * test_config.c: Riak C Unit testing for riak_config
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
#include "riak.pb-c.h"
#include "riak_config-internal.h"
#include "riak_connection-internal.h"

void
test_build_config() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_config_free(&cfg);
    CU_PASS("test_build_binary passed")
}

typedef struct {
    riak_boolean_t init;
    riak_boolean_t log;
    riak_boolean_t cleanup;
} test_log_data;

riak_int32_t
test_log_init(void *ptr) {
    return 0;
}

void
test_log_cleanup(void *ptr) {
}

void
test_log_logger(void            *ptr,
                riak_log_level_t level,
                const char      *file,
                riak_size_t      filelen,
                const char      *func,
                riak_size_t      funclen,
                riak_uint32_t    line,
                const char      *format,
                va_list          args) {
}

void
test_stderr_logger(void            *ptr,
                   riak_log_level_t level,
                   const char      *file,
                   riak_size_t      filelen,
                   const char      *func,
                   riak_size_t      funclen,
                   riak_uint32_t    line,
                   const char      *format,
                   va_list          args) {
}

void
test_config_with_logging() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    err = riak_config_set_logging(cfg, (void*)cfg, test_log_logger, test_log_init, test_log_cleanup);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_config_free(&cfg);
    CU_PASS("test_config_with_logging passed")
}

//
// Internally riak_config_new() uses this allocator
// to bootstrap itself
static void*
test_config_allocator(riak_size_t bytes) {
    CU_ASSERT_FATAL(bytes == sizeof(riak_config))
    return malloc(bytes);
}

static void
test_config_freer(void* ptr) {
    riak_boolean_t *passes = (riak_boolean_t*)ptr;
    *passes = RIAK_TRUE;
}

void
test_config_allocate() {
    riak_config *cfg;
    riak_error err = riak_config_new(&cfg,
                                     test_config_allocator,
                                     NULL,
                                     NULL,
                                     NULL,
                                     NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    CU_ASSERT_PTR_EQUAL_FATAL(cfg->malloc_fn, test_config_allocator)
    CU_PASS("test_config_allocate passed")
}

void
test_config_allocate_clean() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    void *ptr = riak_config_clean_allocate(cfg, 10);
    CU_ASSERT_PTR_NOT_EQUAL_FATAL(ptr, NULL)
    int result = memcmp(ptr, "\0\0\0\0\0\0\0\0\0\0", 10);
    free(ptr);
    CU_ASSERT_FATAL(result == 0)
    CU_PASS("test_config_allocate_clean passed")
}

void
test_config_free() {
    riak_config *cfg;
    riak_error err = riak_config_new(&cfg,
                                     NULL,
                                     NULL,
                                     test_config_freer,
                                     NULL,
                                     NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_boolean_t passes = RIAK_FALSE;
    (cfg->free_fn)((void*)&passes);
    CU_ASSERT_FATAL(passes == RIAK_TRUE)
    CU_PASS("test_config_free passed")
}
