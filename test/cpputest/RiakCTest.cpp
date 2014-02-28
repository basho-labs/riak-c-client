/*********************************************************************
 *
 * RiakCTest.cpp: Riak Testing State
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
#include "RiakCTest.h"
#include "RiakCTestConnection.h"
#include "riak_async.h"
#include "../../src/adapters/riak_libevent.h"

RiakCTest::RiakCTest() {
    // use the default configuration
    riak_error err = riak_config_new_default(&_cfg);
    if (err) {
        throw new RiakCTestError("Could not set up default configuration");
    }
    err = riak_config_set_logging(_cfg,
                                  &_log_state,
                                  test_log,
                                  test_log_init,
                                  test_log_cleanup);
    if (err) {
        throw new RiakCTestError("Could not set up logging");
    }
}

RiakCTest::~RiakCTest() {
    riak_config_free(&_cfg);
}

void
RiakCTest::threadRunner(riak_test_pthread_fun f,
                        RiakCTestAsyncArgs*   args) {
    RiakCTestConnection* conn[RIAK_TEST_NUM_THREADS];
    pthread_t tid[RIAK_TEST_NUM_THREADS];
    void *result[RIAK_TEST_NUM_THREADS];
    for(int i = 0; i < sizeof(conn)/sizeof(RiakCTestConnection*); i++) {
        try {
            conn[i] = new RiakCTestConnection(*this);
            if (conn[i] == NULL) {
                FAIL("Could not create a RiakTestConnection");
            }
        } catch(RiakCTestError* err) {
            FAIL(err->getMessage());
        }
        args->conn = conn[i];
        int status = pthread_create(&tid[i], NULL, f, args);
        if (status != 0) {
            FAIL("Could not spawn ping receive thread");
        }
        fprintf(stderr, "Spawned TID %llu\n", (riak_uint64_t)tid[i]);
    }

    for(int i = 0; i < sizeof(result)/sizeof(void*); i++) {
        int status = pthread_join(tid[i], &result[i]);
        if (status != 0) {
            FAIL("Could not join ping receive thread");
        }
    }
    for(int i = 0; i < sizeof(conn)/sizeof(RiakCTestConnection*); i++) {
        delete conn[i];
    }
}

std::string
RiakCTest::randomString(riak_uint32_t len) {
    std::string result = "";
    for(int i = 0; i< len; i++) {
        result += (char)(32+(rand() % (127-32)));
    }
    return result;
}

void
RiakCTest::loadDb(RiakCTestConnection* conn,
                  riak_test_db*        db) {
    for(int b = 0; b < RIAK_TEST_MAX_BUCKETS; b++) {
        std::string bucket = std::string(RIAK_TEST_BUCKET_PREFIX) + RiakCTest::randomString(20);
        for(int k = 0; k < RIAK_TEST_MAX_KEYS; k++) {
            std::string key = RiakCTest::randomString(20);
            std::string value = RiakCTest::randomString(200);
            riak_object *obj = riak_object_new(_cfg);
            if (obj == NULL) {
                FAIL("Could not construct a riak_object");
            }
            riak_binary *bucket_bin = riak_binary_copy_from_string(_cfg, bucket.c_str());
            if (bucket_bin == NULL) {
                FAIL("Could not allocate the bucket");
            }
            riak_error err = riak_object_set_bucket(_cfg, obj, bucket_bin);
            if (err) {
                FAIL("Could not set the bucket");
            }
            riak_binary *key_bin = riak_binary_copy_from_string(_cfg, key.c_str());
            if (key_bin == NULL) {
                FAIL("Could not allocate the key");
            }
            err = riak_object_set_key(_cfg, obj, key_bin);
            if (err) {
                FAIL("Could not set the key");
            }
            riak_binary *value_bin = riak_binary_copy_from_string(_cfg, value.c_str());
            if (value_bin == NULL) {
                FAIL("Could not allocate the value");
            }
            err = riak_object_set_value(_cfg, obj, value_bin);
            if (err) {
                FAIL("Could not set the bucket");
            }
            riak_put_response *response = NULL;
            riak_test_kv key_value(key,value);
            db->insert(std::make_pair<std::string, riak_test_kv>(bucket, key_value));
            //fprintf(stderr, "%s\t%s\t%s\n", bucket.c_str(), key.c_str(), value.c_str());
            err = riak_put(conn->getConnection(), obj, NULL, &response);
            if (err) {
                FAIL(riak_strerror(err));
            }
            riak_put_response_free(_cfg, &response);
            riak_binary_free(_cfg, &bucket_bin);
            riak_binary_free(_cfg, &key_bin);
            riak_binary_free(_cfg, &value_bin);
            riak_object_free(_cfg, &obj);
        }
    }
}

/**
 * @brief Destroy all buckets starting with RIAK_TEST_BUCKET_PREFIX
 * @param conn Test connection to the DB
 */
void
RiakCTest::cleanupDb(RiakCTestConnection* conn) {
    // Bail if there is no connection
    if (conn == NULL) return;

    const int prefixlen = strlen(RIAK_TEST_BUCKET_PREFIX);
    riak_listbuckets_response *response = NULL;
    riak_listbuckets(conn->getConnection(), &response);
    riak_uint32_t num_buckets = riak_listbuckets_get_n_buckets(response);
    for(int b = 0; b < num_buckets; b++) {
        riak_binary* bucket;
        riak_error err = riak_listbuckets_get_bucket(response, &bucket, b);
        if (err) {
            FAIL("Could not fetch a bucket");
        }
        if (riak_binary_len(bucket) < prefixlen) continue;
        // Does the bucket match the prefix? If so, start nuking
        if (memcmp(riak_binary_data(bucket), RIAK_TEST_BUCKET_PREFIX, prefixlen) == 0) {
            riak_listkeys_response *response;
            err = riak_listkeys(conn->getConnection(), bucket, 0, &response);
            if (err) {
                FAIL("Could not list keys");
            }
            riak_uint32_t num_keys = riak_listkeys_get_n_keys(response);
            for(int k = 0; k < num_keys; k++) {
                riak_binary *key;
                err = riak_listkeys_get_key(response, &key, k);
                if (err) {
                    FAIL("Could not fetch a key");
                }
                //fprintf(stderr, "DEL %s\t%s\n", riak_binary_data(bucket), riak_binary_data(key));
                err = riak_delete(conn->getConnection(), bucket, key, NULL);
                if (err) {
                    FAIL("Could not delete a key");
                }
            }
        }
    }
}
