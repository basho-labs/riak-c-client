/*********************************************************************
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

// NOTE: this is a demo program to show off basic operations
// against Riak. For a more in-depth example, see:
// https://github.com/basho/riak-c-client/blob/develop/examples/example.c
//

#include <riak.h>

void put(riak_config *cfg, riak_connection *cxn) {
    fprintf(stdout, "-------------------------------\n");
    fprintf(stdout, "Test Riak PUT\n");
    fprintf(stdout, "-------------------------------\n");

    // create a new riak_object on the heap
    riak_object *obj = riak_object_new(cfg);
    // create "binaries" on the heap to store bucket, key, and value for
    // the new riak_object
    // A binary is just a charset-agnostic byte array
    // a well behaved client should check for NULL
    // when allocating new structs
    riak_binary *bucket_bin   = riak_binary_copy_from_string(cfg, "TestBucket");
    riak_binary *key_bin      = riak_binary_copy_from_string(cfg, "TestKey");
    riak_binary *value_bin    = riak_binary_copy_from_string(cfg, "TestData");

    // a buffer to write results into
    char output[10240];
    // check this for errors after performing an operation
    riak_error err;

    // populate the riak_object with bucket, key and value
    riak_object_set_bucket(cfg, obj, bucket_bin);
    riak_object_set_key(cfg, obj, key_bin);
    riak_object_set_value(cfg, obj, value_bin);

    // allocate a struct to set PUT options, specifically to return the head
    riak_put_options *put_options = riak_put_options_new(cfg);
    riak_put_options_set_return_head(put_options, RIAK_TRUE);

    riak_put_response *put_response = NULL;
    // run the PUT and display the result (from return_head)
    err = riak_put(cxn, obj, put_options, &put_response);
    if (err == ERIAK_OK) {
        // use a riak-c-client convenience function to display the response
        // TODO: this core dumps in the SSL test
        //riak_put_response_print(put_response, output, sizeof(output));
        //printf("%s\n", output);
        printf("PUT ok\n");
    }

    // cleanup
    riak_put_response_free(cfg, &put_response);
    riak_object_free(cfg, &obj);
    riak_free(cfg, &bucket_bin);
    riak_free(cfg, &key_bin);
    riak_free(cfg, &value_bin);
    riak_put_options_free(cfg, &put_options);

    if (err) {
        fprintf(stderr, "Error executing PUT: %s\n", riak_strerror(err));
        exit(1);
    } else {
      fprintf(stdout, "Ok\n");
    }

}

void get(riak_config *cfg, riak_connection *cxn) {
    fprintf(stdout, "-------------------------------\n");
    fprintf(stdout, "Test Riak GET\n");
    fprintf(stdout, "-------------------------------\n");

    // create "binaries" on the heap to store bucket and key
    // for the object we are going to GET
    // a well behaved client should check for NULL
    // when allocating new structs
    riak_binary *bucket_bin   = riak_binary_copy_from_string(cfg, "TestBucket");
    riak_binary *key_bin      = riak_binary_copy_from_string(cfg, "TestKey");

    // a buffer to write results into
    char output[10240];
    // check this for errors after performing an operation
    riak_error err;

    // allocate a struct to set GET options, specifically
    // to set the basic_quorum & r options
    riak_get_options *get_options = riak_get_options_new(cfg);
    riak_get_options_set_basic_quorum(get_options, RIAK_TRUE);
    riak_get_options_set_r(get_options, 2);

    riak_get_response *get_response = NULL;
    err = riak_get(cxn, bucket_bin, key_bin, get_options, &get_response);
    if (err == ERIAK_OK) {
        riak_print_get_response(get_response, output, sizeof(output));
        printf("%s\n", output);
        printf("Get OK\n");
    }

    // cleanup
    riak_get_response_free(cfg, &get_response);
    riak_free(cfg, &bucket_bin);
    riak_free(cfg, &key_bin);
    riak_get_options_free(cfg, &get_options);

    if (err) {
        fprintf(stderr, "Error executing GET: %s\n", riak_strerror(err));
        exit(1);
    } else {
        fprintf(stdout, "Ok\n");
    }
}


int
main(int argc, char *argv[])
{
    // a riak_config serves as your per-thread state to interact with Riak
    riak_config *cfg;


    // use the default configuration
    riak_error err = riak_config_new_default(&cfg);
    if(err != ERIAK_OK) {
      fprintf(stderr, "Error creating client configuration\n");
      exit(1);
    }

    riak_connection *cxn = NULL;

    // Create a connection with the default address resolver
    const char* riak_host = "127.0.0.1";
    // 8087 is the default protocol buffers port if you are using
    //  Riak from a pre-built package or source build using "make rel"
    // 10017 is the default port if you have built from source
    //  using "make devrel"
    const char* riak_port = "8087";


    riak_security_credentials *creds = NULL;

    riak_security_credentials_new(cfg,
                                  &creds,
                                  "foo",
                                  "bar",
                                  "./test/ssl_tests/certs/cacert.pem");

    err = riak_secure_connection_new(cfg,
                                     &cxn,
                                     riak_host,
                                     riak_port,
                                     creds
                                     );

    printf("ERR = %d\n", err);
    if (err) {
        fprintf(stderr, "Cannot connect to Riak on %s:%s\n", riak_host, riak_port);
        exit(1);
    }

    // test a PUT
    put(cfg, cxn);

    // test a GET
    get(cfg, cxn);

    // cleanup
    riak_connection_free(&cxn);
    riak_config_free(&cfg);

    return 0;
}

