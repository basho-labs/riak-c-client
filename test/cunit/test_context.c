/*********************************************************************
 *
 * test_context.c: Riak C Unit testing for riak_context
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
#include "riak_context-internal.h"

void
test_build_context() {
    riak_context *ctx;
    riak_error err = riak_context_new_default(&ctx);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_context_free(&ctx);
    CU_PASS("test_build_binary passed")
}

static int
test_context_bad_resolver(const char          *nodename,
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
test_context_resolver(const char          *nodename,
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
test_context_with_bad_connection() {
    riak_context *ctx;
    riak_error err = riak_context_new_default(&ctx);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    err = riak_context_add_connection(ctx, test_context_bad_resolver, "localhost", "1");
    CU_ASSERT_FATAL(err == ERIAK_DNS_RESOLUTION)
    // Clean up the AddrInfo so libevent won't
    free(ctx->addrinfo);
    ctx->addrinfo = NULL;
    riak_context_free(&ctx);
    CU_PASS("test_build_binary passed")
}

void
test_context_with_connection() {
    riak_context *ctx;
    riak_error err = riak_context_new_default(&ctx);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    err = riak_context_add_connection(ctx, test_context_resolver, "localhost", "1");
    CU_ASSERT_FATAL(err == ERIAK_OK)
    // Clean up the AddrInfo so libevent won't
    free(ctx->addrinfo);
    ctx->addrinfo = NULL;
    riak_context_free(&ctx);
    CU_PASS("test_build_binary passed")
}

void
test_context_with_logging() {
    riak_context *ctx;
    riak_error err = riak_context_new_default(&ctx);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    err = riak_context_add_logging(ctx, "test");
    CU_ASSERT_FATAL(err == ERIAK_OK)
    CU_ASSERT(strcmp(ctx->logging_category, "test") == 0)
    riak_context_free(&ctx);
    CU_PASS("test_build_binary passed")
}
