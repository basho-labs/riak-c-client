/*********************************************************************
 *
 * RiakCTest.h: Riak Testing State
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

#include "riak.h"
#include "test_logging.h"
#include "riak_async.h"
#include "RiakCTestError.h"
#include <map>

#ifndef _RIAK_C_TEST_H
#define _RIAK_C_TEST_H

#define RIAK_TEST_NUM_THREADS   20
#define RIAK_TEST_BUCKET_PREFIX "riak_c_test_"

#define RIAK_TEST_MAX_BUCKETS   50
#define RIAK_TEST_MAX_KEYS      50

typedef std::pair<std::string, std::string> riak_test_kv;
typedef std::multimap<std::string, riak_test_kv > riak_test_db;

class RiakCTestConnection;

// Base type used to
class RiakCTestAsyncArgs {
public:
    RiakCTestConnection *conn;
};

typedef void*(*riak_test_pthread_fun)(void*);

class RiakCTest {
public:
    /**
     * @brief Hold the state which is persistent across all tests
     */
    RiakCTest();
    virtual ~RiakCTest();

    riak_config*       getConfig()       { return _cfg; }
    struct event_base* getLibeventBase() { return _base; }

    /**
     * @brief Spawn a pile of threads, fire the event loop and join on them
     * @param f Function pointer for thread to spawn
     * @param args Arguments to pass to `f`
     */
    void
    threadRunner(riak_test_pthread_fun f,
                 RiakCTestAsyncArgs*   args);

    /**
     * @brief Generate Random ASCII strings
     * @param len How long to make the string
     * @returns Random string of length `len`
     */
    static std::string
    randomString(riak_uint32_t len);

    /**
     * @brief Throw lots of rubbish into Riak
     * @param conn Riak Test Connection
     * @param db Reference to map of stored data
     */
    void
    loadDb(RiakCTestConnection* conn,
           riak_test_db*        db);

    /**
     * @brief Nuke everything in Riak with the RIAK_TEST_BUCKET_PREFIX
     * @param conn Riak Test Connection
     */
    void
    cleanupDb(RiakCTestConnection* conn);

protected:
    riak_config*       _cfg;
    test_log_data      _log_state;
    struct event_base* _base; // async-only
};

#endif // _RIAK_C_TEST_H
