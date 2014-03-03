/*********************************************************************
 *
 * test.c: Riak C Common Testing Utilities
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

#include <event2/dns.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/thread.h>
#include <pthread.h>
#include "test.h"
#include "riak_async.h"
#include "../../src/adapters/riak_libevent.h"

riak_error
test_setup(riak_config **cfg) {
    return riak_config_new_default(cfg);
}

riak_error
test_connect(riak_config      *cfg,
             riak_connection **cxn) {
    char *hostname = (char*)"localhost";
    char *pb_port  = (char*)"8087";

    char *env_host = getenv(RIAK_TEST_HOST);
    char *env_port = getenv(RIAK_TEST_PB_PORT);

    if (env_host) {
        hostname = env_host;
    }
    if (env_port) {
        pb_port = env_port;
    }

    return riak_connection_new(cfg, cxn, hostname, pb_port, NULL);
}

void
test_disconnect(riak_config      *cfg,
                riak_connection **cxn) {
    riak_connection_free(cxn);
}

void
test_cleanup(riak_config **cfg) {
    riak_config_free(cfg);
}

void
riak_test_error_cb(void *resp,
                   void *ptr) {
    riak_operation *rop = (riak_operation*)ptr;
    char message[1024];
    riak_server_error *error = riak_operation_get_server_error(rop);
    if (error) {
        riak_binary_print(riak_server_error_get_errmsg(error), message, sizeof(message));
        fprintf(stderr, "ERROR: %s\n", message);
    }
    //TODO: How do we bail from this callback?
}

riak_error
test_async_connect(riak_config            *cfg,
                   test_async_connection **conn_out) {
    evthread_enable_lock_debuging();
    int result = evthread_use_pthreads();
    if (result < 0) {
        fprintf(stderr, "Could not use pthreads for libevent\n");
        return ERIAK_EVENT;
    }

    *conn_out = riak_config_clean_allocate(cfg, sizeof(test_async_connection));
    if (*conn_out == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    test_async_connection *conn = *conn_out;
    conn->cfg = cfg;
    riak_error err = test_connect(cfg, &(conn->cxn));
    if (err) {
        fprintf(stderr, "Could not connect to Riak\n");
        return err;
    }
    err = riak_operation_new(conn->cxn, &(conn->rop), NULL, NULL, NULL);
    if (err) {
        fprintf(stderr, "Could not allocate an operation\n");
        return err;
    }
    // Create a new event base for each thread
    conn->base = event_base_new();
    if (conn->base == NULL) {
        fprintf(stderr, "Could not allocate a Libevent base\n");
        return ERIAK_EVENT;
    }
    result = evthread_make_base_notifiable(conn->base);
    if (result) {
        fprintf(stderr, "Could not set libevent base to be notifiable\n");
        return ERIAK_EVENT;
    }

    err = riak_libevent_new(&(conn->rev), conn->rop, conn->base);
    if (err) {
        fprintf(stderr, "Could not allocate a new libevent\n");
        return err;
    }
    riak_operation_set_error_cb(conn->rop, riak_test_error_cb);
    riak_operation_set_cb_data(conn->rop, conn->rop);

    return err;
}

void
test_async_disconnect(test_async_connection **conn_out) {
    test_async_connection *conn = *conn_out;
    riak_operation_free(&(conn->rop));
    riak_connection_free(&(conn->cxn));
    //TODO: Fix this freeing of the event
    //riak_libevent_free(conn->cfg, &(conn->rev));
    event_base_free(conn->base);
    conn->base = NULL;
    riak_free(conn->cfg, conn_out);
}


riak_error
test_async_thread_runner(riak_config             *cfg,
                         test_async_pthread_fun   f,
                         test_async_pthread_args *args) {
    test_async_connection* conn[RIAK_TEST_NUM_THREADS];
    pthread_t tid[RIAK_TEST_NUM_THREADS];
    void *result[RIAK_TEST_NUM_THREADS];
    for(int i = 0; i < sizeof(conn)/sizeof(test_async_connection*); i++) {
        riak_error err = test_async_connect(cfg, &(conn[i]));
        if (err) {
            return err;
        }

        args->conn = conn[i];
        int status = pthread_create(&tid[i], NULL, f, args);
        if (status != 0) {
            return ERIAK_THREAD;
        }
        fprintf(stderr, "Spawned TID %llu\n", (riak_uint64_t)tid[i]);
    }

    for(int i = 0; i < sizeof(result)/sizeof(void*); i++) {
        int status = pthread_join(tid[i], &result[i]);
        if (status != 0) {
            return ERIAK_THREAD;
        }
        if (result[i] != NULL) {
            fprintf(stderr, "THREAD ERROR: %s\n", (char*)result[i]);
        }
    }
    for(int i = 0; i < sizeof(conn)/sizeof(test_async_connection*); i++) {
        test_async_disconnect(&(conn[i]));
    }
    return ERIAK_OK;
}

char*
test_random_string(riak_config  *cfg,
                   riak_uint32_t len) {
    char *result = riak_config_clean_allocate(cfg, len+1);
    if (result != NULL) {
        int i;
        for(i = 0; i< len; i++) {
            result[i] = (char)(32+(rand() % (127-32)));
        }
        result[i] = '\0';
    }
    return result;
}


riak_error
test_load_db(riak_config            *cfg,
             riak_connection        *cxn,
             test_bucket_key_value **root) {
    for (int b = 0; b < RIAK_TEST_MAX_BUCKETS; b++) {
        char *suffix = test_random_string(cfg, 20);
        if (suffix == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        char bucket[128];
        snprintf(bucket, sizeof(bucket), "%s%s", RIAK_TEST_BUCKET_PREFIX, suffix);
        for(int k = 0; k < RIAK_TEST_MAX_KEYS; k++) {
            char *key = test_random_string(cfg, 20);
            if (key == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
            char *value = test_random_string(cfg, 200);
            if (value == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
            riak_object *obj = riak_object_new(cfg);
            if (obj == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
            riak_binary *bucket_bin = riak_binary_copy_from_string(cfg, bucket);
            if (bucket_bin == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
            riak_error err = riak_object_set_bucket(cfg, obj, bucket_bin);
            if (err) {
                return err;
            }
            riak_binary *key_bin = riak_binary_copy_from_string(cfg, key);
            if (key_bin == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
            err = riak_object_set_key(cfg, obj, key_bin);
            if (err) {
                return err;
            }
            riak_binary *value_bin = riak_binary_copy_from_string(cfg, value);
            if (value_bin == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
            err = riak_object_set_value(cfg, obj, value_bin);
            if (err) {
                return err;
            }
            riak_put_response *response = NULL;
            err = test_bkv_add(cfg, root, bucket_bin, key_bin, value_bin);
            //fprintf(stderr, "%s\t%s\t%s\n", bucket, key, value);
            err = riak_put(cxn, obj, NULL, &response);
            if (err) {
                return err;
            }
            riak_put_response_free(cfg, &response);
            /*
             * These are now freed in test_bkv_free()
             *
             * riak_binary_free(cfg, &bucket_bin);
             * riak_binary_free(cfg, &key_bin);
             * riak_binary_free(cfg, &value_bin);
             */
            riak_object_free(cfg, &obj);
        }
    }
    return ERIAK_OK;
}

riak_error
test_cleanup_db(riak_connection* cxn) {
    // Bail if there is no connection
    if (cxn == NULL) return ERIAK_CONNECT;

    const int prefixlen = strlen(RIAK_TEST_BUCKET_PREFIX);
    riak_listbuckets_response *response = NULL;
    riak_listbuckets(cxn, &response);
    riak_uint32_t num_buckets = riak_listbuckets_get_n_buckets(response);
    riak_binary **buckets = riak_listbuckets_get_buckets(response);
    if (buckets == NULL) {
        return ERIAK_OUT_OF_RANGE;
    }
    riak_error err = ERIAK_OK;
    for(int b = 0; b < num_buckets; b++) {
        riak_binary* bucket = buckets[b];
        if (riak_binary_len(bucket) < prefixlen) continue;
        // Does the bucket match the prefix? If so, start nuking
        if (memcmp(riak_binary_data(bucket), RIAK_TEST_BUCKET_PREFIX, prefixlen) == 0) {
            riak_listkeys_response *response;
            err = riak_listkeys(cxn, bucket, 0, &response);
            if (err) {
                return err;
            }
            riak_uint32_t num_keys = riak_listkeys_get_n_keys(response);
            riak_binary **keys = riak_listkeys_get_keys(response);
            if (keys == NULL) {
                return ERIAK_OUT_OF_RANGE;
            }
            for(int k = 0; k < num_keys; k++) {
                riak_binary *key = keys[k];
                //fprintf(stderr, "DEL %s\t%s\n", riak_binary_data(bucket), riak_binary_data(key));
                err = riak_delete(cxn, bucket, key, NULL);
                if (err) {
                    return err;
                }
            }
        }
    }
    return ERIAK_OK;
}

int
test_async_event_loop(test_async_connection *conn) {
    fprintf(stderr, "Event Loop on TID %llu\n", (riak_uint64_t)pthread_self());
    return event_base_dispatch(conn->base);
}

riak_error
test_async_send_message(test_async_connection *conn) {
    fprintf(stderr, "Event Send on TID %llu\n", (riak_uint64_t)pthread_self());
    riak_error err = riak_libevent_send(conn->rop, conn->rev);
    if (err == ERIAK_OK) {
        int result = test_async_event_loop(conn);
        fprintf(stderr, "Event Loop on TID %llu returned %d\n", (riak_uint64_t)pthread_self(), result);
        switch(result) {
        case -1:
            err = ERIAK_EVENT;
            break;
        case 1:
            //err = ERIAK_NO_EVENT;
            break;
        default:
            break;
        }
    }
    return err;
}


