/*********************************************************************
 *
 * test_mapreduce.c:  Riak C Unit testing for Map/Reduce Message
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

/**
 * @brief Test the Map/Reduce message decoder with a fixed message response
 */
void
test_mapreduce_response_decode() {
    riak_config     *cfg;
    riak_operation  *rop = NULL;
    riak_connection *cxn = NULL;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    err = riak_connection_new(cfg, &cxn, "localhost", "1", NULL);
    CU_ASSERT_FATAL(err == ERIAK_CONNECT)
    err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    CU_ASSERT_FATAL(err == ERIAK_OK)

    // Static response from Riak server
    riak_uint8_t bytes0[] = { 0x18,0x08,0x00,0x12,0x0b,0x5b,0x5b,0x22,0x66,0x6f,0x6f,0x22,0x2c,0x31,0x5d,0x5d };
    riak_uint8_t bytes1[] = { 0x18,0x08,0x00,0x12,0x0b,0x5b,0x5b,0x22,0x62,0x61,0x6d,0x22,0x2c,0x33,0x5d,0x5d };
    riak_uint8_t bytes2[] = { 0x18,0x08,0x00,0x12,0x0b,0x5b,0x5b,0x22,0x62,0x61,0x7a,0x22,0x2c,0x30,0x5d,0x5d };
    riak_uint8_t bytes3[] = { 0x18,0x08,0x00,0x12,0x0b,0x5b,0x5b,0x22,0x62,0x61,0x72,0x22,0x2c,0x34,0x5d,0x5d };
    riak_uint8_t bytes4[] = { 0x18,0x18,0x01 };
    riak_uint8_t *bytes[] = { bytes0, bytes1, bytes2, bytes3, bytes4 };
    riak_int32_t len[]    = { sizeof(bytes0), sizeof(bytes1), sizeof(bytes2), sizeof(bytes3), sizeof(bytes4) };

    riak_pb_message          pb_response;
    riak_mapreduce_response *response = NULL;
    riak_boolean_t           done = RIAK_FALSE;
    for(int i = 0; i < 5; i++) {
        pb_response.data = bytes[i];
        pb_response.len  = len[i];
        err = riak_mapreduce_response_decode(rop, &pb_response, &response, &done);
        CU_ASSERT_FATAL(err == ERIAK_OK)
    }
    CU_ASSERT_EQUAL_FATAL(response->n_responses, 5)
    riak_mapreduce_message** msgs = riak_mapreduce_get_messages(response);
    riak_binary *msg = riak_mapreduce_message_get_response(msgs[0]);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(msg), "[[\"foo\",1]]", riak_binary_len(msg)),0)
    msg = riak_mapreduce_message_get_response(msgs[1]);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(msg), "[[\"bam\",3]]", riak_binary_len(msg)),0)
    msg = riak_mapreduce_message_get_response(msgs[2]);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(msg), "[[\"baz\",0]]", riak_binary_len(msg)),0)
    msg = riak_mapreduce_message_get_response(msgs[3]);
    CU_ASSERT_EQUAL_FATAL(memcmp(riak_binary_data(msg), "[[\"bar\",4]]", riak_binary_len(msg)),0)
    CU_ASSERT_EQUAL_FATAL(riak_mapreduce_message_get_has_done(msgs[4]), RIAK_TRUE)
    CU_ASSERT_EQUAL_FATAL(riak_mapreduce_message_get_done(msgs[4]), RIAK_TRUE)
}

typedef struct _test_load_mr_data {
    const char *key;
    const char *value;
} test_load_mr_data;

/**
 * @brief Load a pile of data for an M/R operation
 * @param cxn Riak Connection
 * @param query_string Returned M/R query Javascript function
 */
riak_error
test_setup_mr_test(riak_connection *cxn,
                   riak_binary    **query) {
    riak_config *cfg = riak_connection_get_config(cxn);
    riak_binary *bucket = test_random_binary_with_prefix(cfg, RIAK_TEST_BUCKET_PREFIX, RIAK_TEST_BUCKET_KEY_LEN);
    riak_binary *content = riak_binary_copy_from_string(cfg, "text/plain");
    if (bucket == NULL || content == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    test_load_mr_data data[] = {
            { "foo", "pizza data goes here" },
            { "bar", "pizza pizza pizza pizza" },
            { "baz", "nothing to see here" },
            { "bam", "pizza pizza pizza" }
    };
    for(int i = 0; i < sizeof(data)/sizeof(test_load_mr_data); i++) {
        riak_object *obj = riak_object_new(cfg);
        if (obj == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        riak_error err = riak_object_set_bucket(cfg, obj, bucket);
        if (err) {
            return err;
        }
        riak_binary *key = riak_binary_copy_from_string(cfg, data[i].key);
        if (key == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        riak_binary *value = riak_binary_copy_from_string(cfg, data[i].value);
        if (value == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        err = riak_object_set_key(cfg, obj, key);
        if (err) {
            return err;
        }
        err = riak_object_set_value(cfg, obj, value);
        if (err) {
            return err;
        }
        err = riak_object_set_content_encoding(cfg, obj, content);
        if (err) {
            return err;
        }
        riak_put_response *response;
        err = riak_put(cxn, obj, NULL, &response);
        if (err) {
            return err;
        }
        riak_binary_free(cfg, &key);
        riak_binary_free(cfg, &value);
        riak_object_free(cfg, &obj);
        riak_put_response_free(cfg, &response);
    }

    riak_binary_free(cfg, &content);

    char query_buffer[1024];
    riak_print_state query_state;
    riak_print_init(&query_state, query_buffer, sizeof(query_buffer));
    riak_print(&query_state, "%s", "{\"inputs\":\"");
    riak_print_binary(&query_state, bucket);
    riak_print(&query_state, "%s", "\","
        "\"query\":[{\"map\":{\"language\":\"javascript\","
        "\"source\":\"function(riakObject) {"
        "    var val = riakObject.values[0].data.match(/pizza/g);"
        "    return [[riakObject.key, (val ? val.length : 0 )]];"
        "}\"}}]}");
    *query = riak_binary_copy_from_string(cfg, query_buffer);
    riak_binary_free(cfg, &bucket);

    return ERIAK_OK;
}

riak_boolean_t
test_mapreduce_compare(riak_mapreduce_response *response) {
    riak_uint32_t num = riak_mapreduce_get_n_messages(response);
    CU_ASSERT_EQUAL_FATAL(num,5)
    riak_mapreduce_message **messages = riak_mapreduce_get_messages(response);
    CU_ASSERT_NOT_EQUAL_FATAL(messages,NULL)

    const char* results[] = {
        "[[\"bam\",3]]",
        "[[\"baz\",0]]",
        "[[\"bar\",4]]",
        "[[\"foo\",1]]"
    };
    // Go through all results and make sure they come back (in any order)
    for(int i = 0; i < sizeof(results)/sizeof(char*); i++) {
        int j = 0;
        for(j = 0; j < num; j++) {
            riak_binary *result = riak_mapreduce_message_get_response(messages[i]);
            if (result == NULL) {
                return RIAK_FALSE;
            }
            if (riak_binary_compare_string(result, results[j]) == 0 &&
                riak_mapreduce_message_get_phase(messages[i]) == 0) {
                break;
            }
        }
        if (j >= num) {
            return RIAK_FALSE;
        }
    }

    return RIAK_TRUE;
}

void
test_integration_mapreduce() {
    riak_config     *cfg;
    riak_connection *cxn = NULL;

    riak_error err = test_setup(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    err = test_connect(cfg, &cxn);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_binary *query;
    err = test_setup_mr_test(cxn, &query);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    CU_ASSERT_NOT_EQUAL_FATAL(query,NULL)
    riak_binary *content = riak_binary_copy_from_string(cfg, "application/json");
    CU_ASSERT_NOT_EQUAL_FATAL(content,NULL)

    riak_mapreduce_response *response = NULL;
    err = riak_mapreduce(cxn, content, query, &response);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    char output[10240];
    riak_print_state state;
    riak_print_init(&state, output, sizeof(output));
    riak_mapreduce_response_print(&state, response);
    fprintf(stderr, "%s", output);
    CU_ASSERT_EQUAL_FATAL(test_mapreduce_compare(response),RIAK_TRUE)
    riak_mapreduce_response_free(cfg, &response);

    riak_binary_free(cfg, &content);
    riak_binary_free(cfg, &query);
    test_cleanup_db(cxn);
    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_mapreduce_value passed")
}

void
test_mapreduce_async_cb(riak_mapreduce_response *response,
                  void              *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn = (test_async_connection*)state->conn;

    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));

    //riak_object_print(&state, expected);
    riak_mapreduce_response_print(&print_state, response);
    fprintf(stderr, "%s", output);
    riak_boolean_t match = test_mapreduce_compare(response);
    riak_mapreduce_response_free(conn->cfg, &response);
    state->err = (match) ? ERIAK_OK : ERIAK_INVALID;
}

typedef struct _test_async_pthread_mapreduce_args {
    riak_binary   *content;
    riak_binary   *query;
    riak_boolean_t streaming;
} test_async_pthread_mapreduce_args;

/**
 * @brief Encode and Send a Get request
 * @param args Parameters required to create Get request
 */
void*
test_mapreduce_async_thread(void *ptr) {
    test_async_pthread    *state = (test_async_pthread*)ptr;
    test_async_connection *conn  = state->conn;
    test_async_pthread_mapreduce_args *args = (test_async_pthread_mapreduce_args*)(state->args);
    riak_error err = riak_async_register_mapreduce(conn->rop, args->content, args->query, args->streaming, (riak_response_callback)test_mapreduce_async_cb);
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
test_integration_async_mapreduce() {
    riak_config     *cfg;
    riak_connection *cxn = NULL;

    riak_error err = test_setup(&cfg);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    err = test_connect(cfg, &cxn);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_binary *query;
    err = test_setup_mr_test(cxn, &query);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)
    CU_ASSERT_NOT_EQUAL_FATAL(query,NULL)
    riak_binary *content = riak_binary_copy_from_string(cfg, "application/json");
    CU_ASSERT_NOT_EQUAL_FATAL(content,NULL)

    test_async_pthread_mapreduce_args args;
    args.content   = content;
    args.query     = query;
    args.streaming = RIAK_FALSE;
    err = test_async_thread_runner(cfg, test_mapreduce_async_thread, (void*)&args, (void*)NULL);
    CU_ASSERT_EQUAL_FATAL(err,ERIAK_OK)

    riak_binary_free(cfg, &content);
    riak_binary_free(cfg, &query);
    test_cleanup_db(cxn);
    test_disconnect(cfg, &cxn);
    test_cleanup(&cfg);
    CU_PASS("test_integration_async_mapreduce passed")
}

