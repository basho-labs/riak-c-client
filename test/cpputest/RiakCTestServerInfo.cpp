/*********************************************************************
 *
 * RiakCTestServerInfo.cpp: Riak C Integration testing for Server Information message
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

#include <CppUTest/TestHarness.h>
#include <pthread.h>
#include "test_logging.h"
#include "riak.h"
#include "RiakCTestConnection.h"
#include "RiakCTestError.h"

TEST_GROUP(ServerInfo)
{
    void setup()
    {
        _conn = NULL;
        try {
            _conn = new RiakCTestConnection(_test);
            if (_conn == NULL) {
                FAIL("Could not create a RiakTestConnection");
            }
        } catch(RiakCTestError* err) {
            FAIL(err->getMessage());
        }
    }

    void teardown()
    {
        delete _conn;
        _conn = NULL;
    }
protected:
    RiakCTest            _test;
    RiakCTestConnection *_conn;
};

/**
 * @brief Callback for Ping message
 * @param response Ping response
 * @parm ptr Riak Operation
 */
void
riak_test_ping_cb(riak_ping_response *response,
                  void               *ptr) {
    riak_operation   *rop = (riak_operation*)ptr;
    riak_connection  *cxn = riak_operation_get_connection(rop);
    riak_config      *cfg = riak_connection_get_config(cxn);
    riak_log_notice(cxn, "%s", "Asynchronous PONG");
    riak_free_ping_response(cfg, &response);
}

/**
 * @brief Encode and Send a Ping request
 * @param args Parameters required to create Ping request
 */
void*
riak_test_ping_thread(void *ptr) {
    RiakCTestAsyncArgs *args = (RiakCTestAsyncArgs*)ptr;
    RiakCTestConnection *conn = args->conn;
    riak_error err = conn->asyncCreateEvent();
    if (err) {
        FAIL("Could not create asynchronous libevent event");
    }
     err = riak_async_register_ping(conn->getOperation(), (riak_response_callback)riak_test_ping_cb);
    if (err) {
        FAIL(riak_strerror(err));
    }
    err = conn->asyncSendMessage();
    if (err) {
        FAIL("Could not send request");
    }
    return NULL;
}

TEST(ServerInfo, TestSyncPing) {
    riak_error err = riak_ping(_conn->getConnection());
    if (err) {
        FAIL(riak_strerror(err));
    }
    riak_log_notice(_conn->getConnection(), "%s", "Synchronous PONG");
}

TEST(ServerInfo, TestAsyncPing) {
    RiakCTestAsyncArgs args;
    _test.threadRunner(riak_test_ping_thread, &args);
}

/**
 * @brief Callback for Server Info message
 * @param response Server Info response
 * @parm ptr Riak Operation
 */
void
riak_test_serverinfo_cb(riak_serverinfo_response *response,
                        void                     *ptr) {
    riak_operation   *rop = (riak_operation*)ptr;
    riak_connection  *cxn = riak_operation_get_connection(rop);
    riak_config      *cfg = riak_connection_get_config(cxn);
    char buffer[1024];
    riak_serverinfo_response_print(response, buffer, sizeof(buffer));
    riak_log_notice(cxn, "ASYNCHRONOUS %s", buffer);
    riak_serverinfo_response_free(cfg, &response);
}

/**
 * @brief Encode and Send a Server Info request
 * @param args Parameters required to create Server Info request
 */
void*
riak_test_serverinfo_thread(void *ptr) {
    RiakCTestAsyncArgs *args = (RiakCTestAsyncArgs*)ptr;
    RiakCTestConnection *conn = args->conn;
    riak_error err = conn->asyncCreateEvent();
    if (err) {
        FAIL("Could not create asynchronous libevent event");
    }
     err = riak_async_register_serverinfo(conn->getOperation(), (riak_response_callback)riak_test_serverinfo_cb);
    if (err) {
        FAIL(riak_strerror(err));
    }
    err = conn->asyncSendMessage();
    if (err) {
        FAIL("Could not send request");
    }
    return NULL;
}

TEST(ServerInfo, TestServerInfo) {
    riak_serverinfo_response *serverinfo_response = NULL;
    riak_error err = riak_serverinfo(_conn->getConnection(), &serverinfo_response);
    if (err) {
        FAIL(riak_strerror(err));
    }
    char output[1024];
    riak_serverinfo_response_print(serverinfo_response, output, sizeof(output));
    riak_log_notice(_conn->getConnection(), "%s", output);
    riak_serverinfo_response_free(_test.getConfig(), &serverinfo_response);
}

TEST(ServerInfo, TestAsyncServerInfo) {
    RiakCTestAsyncArgs args;
    _test.threadRunner(riak_test_serverinfo_thread, &args);
}

