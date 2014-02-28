/*********************************************************************
 *
 * RiakCTestLists.cpp: Riak C Integration testing for Server Information messages
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
#include "RiakCTest.h"
#include "RiakCTestConnection.h"
#include "RiakCTestError.h"
/*
 * @brief Load a pile of keys into Riak
 * @param conn Test connection to the DB
 */

TEST_GROUP(Lists)
{
    void setup()
    {
        _conn = NULL;
        try {
            _conn = new RiakCTestConnection(_test);
            if (_conn == NULL) {
                FAIL("Could not create a RiakTestConnection");
            }
            _db = new riak_test_db();
            if (_db == NULL) {
                FAIL("Could not create a riak_test_db");
            }
            _test.loadDb(_conn, _db);
        } catch(RiakCTestError* err) {
            FAIL(err->getMessage());
        }
    }

    void teardown()
    {
        _test.cleanupDb(_conn);
        delete _db;
        delete _conn;
        _conn = NULL;
        _db   = NULL;
    }
protected:
    RiakCTest            _test;
    RiakCTestConnection* _conn;
    riak_test_db*        _db;
};

/**
 * @brief Callback for Listbuckets message
 * @param response Listbuckets response
 * @parm ptr Riak Operation
 */
void
riak_test_listbuckets_cb(riak_listbuckets_response *response,
                         void                      *ptr) {
    riak_operation   *rop = (riak_operation*)ptr;
    riak_connection  *cxn = riak_operation_get_connection(rop);
    riak_config      *cfg = riak_connection_get_config(cxn);
    char buffer[10240];
    riak_listbuckets_response_print(response, buffer, sizeof(buffer));
    fprintf(stderr, "TID %llu - %s\n", (riak_uint64_t)pthread_self(), buffer);
    if (riak_listbuckets_get_n_buckets(response) < RIAK_TEST_MAX_BUCKETS) {
        FAIL("Wrong number of buckets returned");
    }
    riak_listbuckets_response_free(cfg, &response);
}

/**
 * @brief Encode and Send a Listbuckets request
 * @param args Parameters required to create Listbuckets request
 */
void*
riak_test_listbuckets_thread(void *ptr) {
    RiakCTestAsyncArgs  *args = (RiakCTestAsyncArgs*)ptr;
    RiakCTestConnection *conn = args->conn;
    riak_error err = conn->asyncCreateEvent();
    if (err) {
        FAIL("Could not create asynchronous libevent event");
    }
     err = riak_async_register_listbuckets(conn->getOperation(), (riak_response_callback)riak_test_listbuckets_cb);
    if (err) {
        FAIL(riak_strerror(err));
    }
    err = conn->asyncSendMessage();
    if (err) {
        FAIL("Could not send request");
    }
    return NULL;
}

TEST(Lists, TestListBuckets) {
    riak_error err = riak_ping(_conn->getConnection());
    if (err) {
        FAIL(riak_strerror(err));
    }
    riak_log_notice(_conn->getConnection(), "%s", "Synchronous PONG");
}

TEST(Lists, TestAsyncListBuckets) {
    RiakCTestAsyncArgs args;
    _test.threadRunner(riak_test_listbuckets_thread, &args);
}

class RiakCTestListKeysArgs : public RiakCTestAsyncArgs {
public:
    riak_binary*   bucket;
    riak_uint32_t  timeout;
} riak_test_listkeys_args;

/**
 * @brief Callback for Listkeys message
 * @param response Listkeys response
 * @parm ptr Riak Operation
 */
void
riak_test_listkeys_cb(riak_listkeys_response *response,
                      void                   *ptr) {
    riak_operation   *rop = (riak_operation*)ptr;
    riak_connection  *cxn = riak_operation_get_connection(rop);
    riak_config      *cfg = riak_connection_get_config(cxn);
    char buffer[10240];
    riak_listkeys_response_print(response, buffer, sizeof(buffer));
    //fprintf(stderr, "TID %llu - %s\n", (riak_uint64_t)pthread_self(), buffer);
    if (riak_listkeys_get_n_keys(response) != RIAK_TEST_MAX_KEYS) {
        FAIL("Wrong number of keys returned");
    }
    riak_listkeys_response_free(cfg, &response);
}

/**
 * @brief Encode and Send a Listkeys request
 * @param args Parameters required to create Listkeys request
 */
void*
riak_test_listkeys_thread(void *ptr) {
    RiakCTestListKeysArgs* args = (RiakCTestListKeysArgs*)ptr;
    RiakCTestConnection* conn = args->conn;
    riak_error err = conn->asyncCreateEvent();
    if (err) {
        FAIL("Could not create asynchronous libevent event");
    }
    fprintf(stderr, "Registered TID %llu\n", (riak_uint64_t)pthread_self());

    err = riak_async_register_listkeys(conn->getOperation(), args->bucket, args->timeout, (riak_response_callback)riak_test_listkeys_cb);
    if (err) {
        FAIL(riak_strerror(err));
    }
    err = conn->asyncSendMessage();
    if (err) {
        FAIL(riak_strerror(err));
    }
    return NULL;
}

TEST(Lists, TestListKeys) {
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

TEST(Lists, TestAsyncListKeys) {
    // Grab any ol' bucket we just added
    riak_test_db::const_iterator it = _db->begin();
    riak_binary *bucket = riak_binary_copy_from_string(_test.getConfig(), it->first.c_str());
    if (bucket == NULL) {
        FAIL("Could not allocate a list key bucket name");
    }
    RiakCTestListKeysArgs args;
    args.bucket = bucket;
    args.timeout = 5000;

    _test.threadRunner(riak_test_listkeys_thread, (RiakCTestAsyncArgs*)&args);
}

