/*********************************************************************
 *
 * test_ping.c: Riak C Integration testing for Ping message
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
test_integration_ping() {
    riak_config     *cfg;
    riak_connection *cxn = NULL;

    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = test_connect(cfg, &cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_ping(cxn);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_ping passed")
}

/**
 * @brief Callback for Ping message
 * @param response Ping response
 * @parm ptr Riak Operation
 */
void
test_ping_async_cb(riak_ping_response *response,
                   void               *ptr) {
    test_async_connection *conn = (test_async_connection*)ptr;
    riak_log_notice(conn->cxn, "%s", "Asynchronous PONG");
    riak_free_ping_response(conn->cfg, &response);
}

/**
 * @brief Encode and Send a Ping request
 * @param args Parameters required to create Ping request
 */
void*
test_ping_async_thread(void *ptr) {
    test_async_pthread *state = (test_async_pthread*)ptr;
    test_async_connection *conn = state->conn;
    riak_error err = riak_async_register_ping(conn->rop, (riak_response_callback)test_ping_async_cb);
    if (err) {
        return (void*)riak_strerror(err);
    }
    err = test_async_send_message(conn);
    if (err) {
        return (void*)"Could not send request";
    }
    return NULL;
}

void
test_integration_async_ping() {
    riak_config           *cfg;
    riak_error err = test_setup(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = test_async_thread_runner(cfg, test_ping_async_thread, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    test_cleanup(&cfg);
    CU_PASS("test_integration_async_ping passed")
}
