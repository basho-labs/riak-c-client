/*********************************************************************
 *
 * test_operation.c: Riak C Unit testing for riak_operation
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
#include "test.h"

void
test_operation_new() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_connection *cxn = NULL;
    err = riak_connection_new(cfg, &cxn, "localhost", "1", &test_connection_dummy_options);
    CU_ASSERT_EQUAL_FATAL(err, ERIAK_OK)
    riak_operation *rop;
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_config_free(&cfg);
    CU_PASS("test_operation_new passed")
}

void
test_operation_response_cb(void *response,
                           void *ptr) {
}

void
test_operation_error_cb(void *response,
                        void *ptr) {
}

void
test_operation_callbacks() {
    riak_config *cfg;
    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_EQUAL_FATAL(err, ERIAK_OK)
    riak_connection *cxn = NULL;
    err = riak_connection_new(cfg, &cxn, "localhost", "1", &test_connection_dummy_options);
    CU_ASSERT_EQUAL_FATAL(err, ERIAK_OK)
    riak_operation *rop;
    err = riak_operation_new(cxn, &rop, test_operation_response_cb, test_operation_error_cb, NULL);
    CU_ASSERT_FATAL(rop->response_cb == test_operation_response_cb)
    CU_ASSERT_FATAL(rop->error_cb == test_operation_error_cb)
    riak_operation_free(&rop);
    riak_connection_free(&cxn);
    riak_config_free(&cfg);
    CU_PASS("test_operation_callbacks passed")
}
