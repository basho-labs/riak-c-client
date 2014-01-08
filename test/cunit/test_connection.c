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
#include "riak_connection-internal.h"

static int
test_connection_bad_resolver(const char          *nodename,
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
test_connection_resolver(const char          *nodename,
                         const char          *servname,
                         const riak_addrinfo *hints_in,
                         riak_addrinfo      **res) {
    riak_addrinfo *info = (riak_addrinfo*)malloc(sizeof(riak_addrinfo));
    if (info == NULL) {
        return 1;
    }
    *res = info;

    info->ai_flags     = 0;
    info->ai_family    = AF_INET;
    info->ai_socktype  = SOCK_STREAM;
    info->ai_protocol  = IPPROTO_TCP;
    info->ai_addrlen   = 16;
    info->ai_canonname = 0x0;
    info->ai_addr = (struct sockaddr*)malloc(sizeof(struct sockaddr));
    if (info->ai_addr == NULL) {
        free(info);
        return 1;
    }
    info->ai_addr->sa_family = AF_INET;
    // 127.0.0.1 port 1
    memcpy(info->ai_addr->sa_data, "\000\001\000\000\001\000\000\000\000\000\000\000", 12);
    info->ai_next = 0x0;

    return 0;
}

void
test_connection_with_bad_resolver() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_connection *cxn = NULL;
    err = riak_connection_new(cfg, &cxn, "localhost", "1", test_connection_bad_resolver);
    CU_ASSERT_FATAL(err == ERIAK_DNS_RESOLUTION)
    riak_free(cfg, &(cxn->addrinfo));
    riak_connection_free(&cxn);
    riak_config_free(&cfg);
    CU_PASS("test_config_with_bad_connection passed")
}

void
test_connection_with_good_resolver() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_connection *cxn = NULL;
    err = riak_connection_new(cfg, &cxn, "localhost", "1", test_connection_resolver);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    riak_free(cfg, &(cxn->addrinfo->ai_addr));
    riak_free(cfg, &(cxn->addrinfo));
    riak_connection_free(&cxn);
    riak_config_free(&cfg);
    CU_PASS("test_config_with_connection passed")
}
