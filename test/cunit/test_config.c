/*********************************************************************
 *
 * test_config.c: Riak C Unit testing for riak_config
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

static int
test_config_bad_resolver(const char          *nodename,
                         const char          *servname,
                         const riak_addrinfo *hints_in,
                         riak_addrinfo      **res) {
    riak_addrinfo *info = (riak_addrinfo*)malloc(sizeof(riak_addrinfo));
    if (info == NULL) {
        return 1;
    }
    *res = info;

    return 1;
}

static int
test_config_resolver(const char          *nodename,
                     const char          *servname,
                     const riak_addrinfo *hints_in,
                     riak_addrinfo      **res) {
    riak_addrinfo *info = (riak_addrinfo*)malloc(sizeof(riak_addrinfo));
    if (info == NULL) {
        return 1;
    }
    *res = info;

    return 0;
}

void
test_config_with_bad_connection() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_connection *cxn = NULL;
    err = riak_connection_new(cfg, &cxn, "localhost", "1", test_config_bad_resolver);
    CU_ASSERT_FATAL(err == ERIAK_DNS_RESOLUTION)
    riak_free(cfg, &(cxn->addrinfo));
    riak_connection_free(&cxn);
    riak_config_free(&cfg);
    CU_PASS("test_config_with_bad_connection passed")
}

void
test_config_with_connection() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_connection *cxn = NULL;
    err = riak_connection_new(cfg, &cxn, "localhost", "1", test_config_resolver);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    riak_free(cfg, &(cxn->addrinfo));
    riak_connection_free(&cxn);
    riak_config_free(&cfg);
    CU_PASS("test_config_with_connection passed")
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
test_config_with_logging() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    err = riak_config_set_logging(cfg, (void*)cfg, test_log_logger, test_log_init, test_log_cleanup);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_config_free(&cfg);
    CU_PASS("test_build_binary passed")
}
