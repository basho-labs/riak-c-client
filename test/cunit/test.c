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
    riak_error err = riak_config_new_default(cfg);
    if (err == ERIAK_OK) {
        err = riak_config_set_logging(*cfg, NULL, test_log, NULL, NULL);
    }
    return err;
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
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn  = state->conn;
    char message[1024];
    riak_server_error *error = riak_operation_get_server_error(conn->rop);
    if (error) {
        riak_binary_print(riak_server_error_get_errmsg(error), message, sizeof(message));
        fprintf(stderr, "SERVER ERROR: %s\n", message);
        state->err      = ERIAK_SERVER_ERROR;
        strncpy(state->err_msg, message, sizeof(state->err_msg));
    }
}

riak_error
test_async_connect(riak_config            *cfg,
                   test_async_connection **conn_out) {
    *conn_out = riak_config_clean_allocate(cfg, sizeof(test_async_connection));
    if (*conn_out == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    test_async_connection *conn = *conn_out;
    conn->cfg        = cfg;
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
    int result = evthread_make_base_notifiable(conn->base);
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

    return err;
}

void
test_async_disconnect(test_async_connection **conn_out) {
    test_async_connection *conn = *conn_out;
    riak_operation_free(&(conn->rop));
    riak_connection_free(&(conn->cxn));
    riak_libevent_free(conn->cfg, &(conn->rev));
    event_base_free(conn->base);
    conn->base = NULL;
    riak_free(conn->cfg, conn_out);
}


riak_error
test_async_thread_runner(riak_config             *cfg,
                         test_async_pthread_fun   f,
                         void                    *args,
                         void                    *expected) {
    test_async_pthread state[RIAK_TEST_NUM_THREADS];
    riak_error err = ERIAK_OK;
    for(int i = 0; i < RIAK_TEST_NUM_THREADS; i++) {
        err = test_async_connect(cfg, &(state[i].conn));
        if (err) {
            return err;
        }
        state[i].args       = args;
        state[i].expected   = expected;
        state[i].err        = ERIAK_TEST_FAILURE; // Returned error flag
        state[i].err_msg[0] = '\0';

        // Pass the whole state to the callback
        riak_operation_set_cb_data(state[i].conn->rop, &(state[i]));

        int status = pthread_create(&(state[i].tid), NULL, f, &(state[i]));
        if (status != 0) {
            return ERIAK_THREAD;
        }
#ifdef _RIAK_DEBUG
        fprintf(stderr, "Spawned TID %llu\n", (riak_int64_t)(state[i].tid));
#endif
    }

    for(int i = 0; i < RIAK_TEST_NUM_THREADS; i++) {
        int status = pthread_join(state[i].tid, &(state[i].pthread_result));
        if (status != 0) {
            err = ERIAK_THREAD;
        }
        if (state[i].pthread_result != NULL) {
            riak_log_critical(state[i].conn->cxn, "THREAD ERROR: %s\n", (char*)(state[i].pthread_result));
            err = ERIAK_THREAD;
        }
        if (state[i].err) {
            riak_log_critical(state[i].conn->cxn, "CALLBACK ERROR: %s\n", state[i].err_msg);
            err = state[i].err;
        }
    }
    for(int i = 0; i < RIAK_TEST_NUM_THREADS; i++) {
        test_async_disconnect(&(state[i].conn));
    }
    return err;
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

riak_binary*
test_random_binary(riak_config  *cfg,
                   riak_uint32_t len) {
    char *result = test_random_string(cfg, len);
    if (result == NULL) {
        return NULL;
    }
    riak_binary *bin = riak_binary_new(cfg, len, (riak_uint8_t*)result);
    riak_free(cfg, &result);
    return bin;
}

int
test_random_int() {
    return rand();
}

riak_boolean_t
test_random_bool() {
    return (rand() % 2) ? RIAK_TRUE : RIAK_FALSE;
}

typedef riak_error (*test_object_field)(riak_config*,riak_object*,riak_binary*);

riak_error
test_load_dummy_object_field(riak_config      *cfg,
                             riak_object      *obj,
                             riak_uint32_t     len,
                             test_object_field fn) {
    if (len == 0) len = RIAK_TEST_BUCKET_KEY_LEN;
    riak_binary *value = test_random_binary(cfg, len);
    if (value == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_error err = fn(cfg, obj, value);
    riak_binary_free(cfg, &value);
    return err;
}

/**
 * @brief Create a simple dummy object to test reading of data
 * @param cfg Riak Config
 * @param bucket Riak bucket name
 * @param key Riak key name
 * @param num_extras Number of indexes, links and metadata to generate
 * @param obj_out Returned populated object
 * @param root Test Bucket/Key/Value to store the generated object
 * @returns Error Code
 */
riak_error
test_load_dummy_object(riak_config            *cfg,
                       riak_binary            *bucket,
                       riak_binary            *key,
                       riak_uint32_t           num_extras,
                       riak_object           **obj_out,
                       test_bucket_key_value **root) {
    riak_object *obj = riak_object_new(cfg);
    if (obj == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_error err = riak_object_set_bucket(cfg, obj, bucket);
    if (err) {
        fprintf(stderr, "Could not add bucket to dummy object\n");
        return err;
    }
    err = riak_object_set_key(cfg, obj, key);
    if (err) {
        fprintf(stderr, "Could not add key to dummy object\n");
        return err;
    }
    err = test_load_dummy_object_field(cfg, obj, RIAK_TEST_VALUE_LEN, riak_object_set_value);
    if (err) {
        fprintf(stderr, "Could not add value to dummy object\n");
        return err;
    }
    err = test_load_dummy_object_field(cfg, obj, 0, riak_object_set_charset);
    if (err) {
        fprintf(stderr, "Could not add charset to dummy object\n");
        return err;
    }
    err = test_load_dummy_object_field(cfg, obj, 0, riak_object_set_content_type);
    if (err) {
        fprintf(stderr, "Could not add content-type to dummy object\n");
        return err;
    }
    err = test_load_dummy_object_field(cfg, obj, 0, riak_object_set_content_encoding);
    if (err) {
        fprintf(stderr, "Could not add encoding to dummy object\n");
        return err;
    }
    // Since the VTag and last modified stamps are set by Riak, don't put them in a dummy object
    // err = test_load_dummy_object_field(cfg, obj, 0, riak_object_set_vtag);
    // if (err) {
    //     fprintf(stderr, "Could not add vtag to dummy object\n");
    //     return err;
    // }
    // riak_object_set_last_mod(obj, test_random_int());
    // riak_object_set_last_mod_usecs(obj, test_random_int());

    // Add some Links
    for(int i = 0; i < num_extras; i++) {
        riak_link *link = riak_object_new_link(cfg, obj);
        if (link == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        err = riak_link_set_bucket(cfg, link, bucket);
        if (err) {
            fprintf(stderr, "Could not add bucket to dummy link\n");
            return err;
        }
        err = riak_link_set_key(cfg, link, key);
        riak_binary *tag = test_random_binary(cfg, RIAK_TEST_BUCKET_KEY_LEN);
        if (err) {
            fprintf(stderr, "Could not add key to dummy link\n");
            return err;
        }
        err = riak_link_set_tag(cfg, link, tag);
        if (err) {
            fprintf(stderr, "Could not add tag to dummy link\n");
            return err;
        }
        riak_binary_free(cfg, &tag);
    }

    for(int i = 0; i < num_extras; i++) {
        riak_pair *usermeta = riak_object_new_usermeta(cfg, obj);
        if (usermeta == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        err = riak_pair_set_key(cfg, usermeta, key);
        if (err) {
            fprintf(stderr, "Could not add key to dummy metadata\n");
            return err;
        }
        err = riak_pair_set_value(cfg, usermeta, bucket);
        if (err) {
            fprintf(stderr, "Could not add value to dummy metadata\n");
            return err;
        }
    }

#if NEED_TO_SET_UP_2I_INDEXES_FIRST
    for(int i = 0; i < num_extras; i++) {
        riak_pair *index = riak_object_new_index(cfg, obj);
        if (index == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        err = riak_pair_set_key(cfg, index, bucket);
        if (err) {
            fprintf(stderr, "Could not add key to dummy index\n");
            return err;
        }
        err = riak_pair_set_value(cfg, index, bucket);
        if (err) {
            fprintf(stderr, "Could not add value to dummy index\n");
            return err;
        }
    }
#endif

    err = test_bkv_add(cfg, root, bucket, key, obj);
    if (err) {
        fprintf(stderr, "Could not add object to internal cache\n");
        return err;
    }
    *obj_out = obj;
    return ERIAK_OK;
}

riak_error
test_load_db(riak_config            *cfg,
             riak_connection        *cxn,
             test_bucket_key_value **root,
             riak_uint32_t           n_buckets,
             riak_uint32_t           n_keys) {
    for (int b = 0; b < n_buckets; b++) {
        char *suffix = test_random_string(cfg, RIAK_TEST_BUCKET_KEY_LEN);
        if (suffix == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        char bucket_str[128];
        snprintf(bucket_str, sizeof(bucket_str), "%s%s", RIAK_TEST_BUCKET_PREFIX, suffix);
        riak_binary *bucket = riak_binary_copy_from_string(cfg, bucket_str);
        if (bucket == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        for(int k = 0; k < n_keys; k++) {
            riak_binary *key = test_random_binary(cfg, RIAK_TEST_BUCKET_KEY_LEN);
            if (key == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
            riak_put_response *response = NULL;
            riak_object *obj;
            riak_error err = test_load_dummy_object(cfg, bucket, key, RIAK_TEST_NUM_OBJ_EXTRAS, &obj, root);
            if (err) {
                return err;
            }
            err = riak_put(cxn, obj, NULL, &response);
            if (err) {
                return err;
            }
            riak_binary_free(cfg, &key);
            riak_put_response_free(cfg, &response);
            /*
             * These are now freed in test_bkv_free()
             *
             * riak_object_free(cfg, &obj);
             */
        }
        riak_binary_free(cfg, &bucket);
        riak_free(cfg, &suffix);
    }
    return ERIAK_OK;
}

riak_error
test_cleanup_db(riak_connection* cxn) {
    // Bail if there is no connection
    if (cxn == NULL) return ERIAK_CONNECT;
    riak_config *cfg = riak_connection_get_config(cxn);

    const int prefixlen = strlen(RIAK_TEST_BUCKET_PREFIX);

    riak_listbuckets_response *response = NULL;
    riak_error err = riak_listbuckets(cxn, NULL, DEFAULT_TIMEOUT, &response);
    if (err) {
        return err;
    }
    riak_uint32_t num_buckets = riak_listbuckets_get_n_buckets(bucket_response);
    riak_binary **buckets = riak_listbuckets_get_buckets(bucket_response);
    if (buckets == NULL) {
        return ERIAK_OUT_OF_RANGE;
    }
    for(int b = 0; b < num_buckets; b++) {
        riak_binary* bucket = buckets[b];
        if (riak_binary_len(bucket) < prefixlen) continue;
        // Does the bucket match the prefix? If so, start nuking
        if (memcmp(riak_binary_data(bucket), RIAK_TEST_BUCKET_PREFIX, prefixlen) == 0) {
            riak_listkeys_response *response;
            err = riak_listkeys(cxn, bucket, NULL, 0, &response);
            if (err) {
                return err;
            }
            riak_uint32_t num_keys = riak_listkeys_get_n_keys(key_response);
            riak_binary **keys = riak_listkeys_get_keys(key_response);
            if (keys == NULL) {
                return ERIAK_OUT_OF_RANGE;
            }
            for(int k = 0; k < num_keys; k++) {
                riak_binary *key = keys[k];
                err = riak_delete(cxn, bucket, NULL, key, NULL);
                if (err) {
                    fprintf(stderr, "DELETE Failed and leftover data may remain\n");
                    return err;
                }
            }
            riak_listkeys_response_free(cfg, &key_response);
        }
    }
    riak_listbuckets_response_free(cfg, &bucket_response);
    return ERIAK_OK;
}

int
test_async_event_loop(test_async_connection *conn) {
#ifdef _RIAK_DEBUG
    fprintf(stderr, "Event Loop on TID %llu with base %llx and connection %llx\n", (riak_int64_t)pthread_self(), (riak_int64_t)conn->base, (riak_int64_t)conn);
#endif
    return event_base_dispatch(conn->base);
}

riak_error
test_async_send_message(test_async_connection *conn) {
#ifdef _RIAK_DEBUG
    fprintf(stderr, "Event Send on TID %llu\n", (riak_int64_t)pthread_self());
#endif
    riak_error err = riak_libevent_send(conn->rop, conn->rev);
    if (err == ERIAK_OK) {
        int result = test_async_event_loop(conn);
#ifdef _RIAK_DEBUG
        fprintf(stderr, "Event Loop on TID %llu returned %d\n", (riak_int64_t)pthread_self(), result);
#endif
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


