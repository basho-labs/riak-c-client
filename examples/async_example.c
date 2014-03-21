/*********************************************************************
 *
 * async_example.c: Example Riak C Client Asynchronous Application
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
#include "riak_async.h"
#include "riak_command.h"
#include "example_call_backs.h"
#include "../adapters/riak_libevent.h"
#include "example_log.h"

int
main(int   argc,
     char *argv[])
{
    riak_args args;
    int operation = riak_parse_args(argc, argv, &args);
    if (operation < 0) {
        usage(stderr, argv[0]);
    }

#ifdef _RIAK_DEBUG
    event_enable_debug_mode();
#endif

    // a riak_config serves as your per-thread state to interact with Riak
    riak_config *cfg;

    // use the default configuration
    riak_error err = riak_config_new_default(&cfg);
    if (err) {
        exit(1);
    }
    example_log_data datum;
    // configurate logging using the callbacks defined above
    err = riak_config_set_logging(cfg,
                                  (void*)&datum,
                                  example_log,
                                  example_log_init,
                                  example_log_cleanup);
    if (err) {
        exit(1);
    }

    int result = evthread_use_pthreads();
    if (result < 0) {
        riak_log_critical_config(cfg, "%s", "Could not use pthreads with libevent");
        exit(1);
    }
    struct event_base *base = event_base_new();
    if (base == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not create libevent base");
        exit(1);
    }
    result = evthread_make_base_notifiable(base);
    if (result) {
        riak_log_critical_config(cfg, "%s", "Could not initialize libevent base");
        exit(1);
    }
    int it;

    // create some sample binary values to use
    riak_binary *bucket_bin   = riak_binary_copy_from_string(cfg, args.bucket); // Not copied
    riak_binary *bucket_type_bin = NULL;
    bucket_type_bin = riak_binary_copy_from_string(cfg, args.bucket_type);
    riak_binary *key_bin      = riak_binary_copy_from_string(cfg, args.key);   // Not copied
    riak_binary *value_bin    = riak_binary_copy_from_string(cfg, args.value); // Not copied
    riak_binary *index_bin    = riak_binary_copy_from_string(cfg, args.index); // Not copied
    riak_binary *content_type = riak_binary_copy_from_string(cfg, "application/json");

    // check for memory allocation problems
    if (bucket_bin == NULL ||
        key_bin    == NULL ||
        value_bin  == NULL) {
        fprintf(stderr, "Could not allocate bucket/key/value/index\n");
        exit(1);
    }

    // Supporting Options and outputs
    riak_2i_options *index_options;
    riak_bucketprops    *props;
    riak_delete_options *delete_options;
    riak_get_options    *get_options;
    riak_object         *obj;
    riak_put_options    *put_options;
    riak_search_options *search_options;

    // iterate through argv
    for(it = 0; it < args.iterate; it++) {
        riak_connection  *cxn   = NULL;
        riak_libevent    *event = NULL;
        riak_operation   *rop   = NULL;
        // Create a connection with the default address resolver
        err = riak_connection_new(cfg, &cxn, args.host, args.portnum, NULL);
        if (err) {
            exit(1);
        }
        // if the application was configured using  -D_RIAK_DEBUG,
        // it will trace debug level messages
        riak_log_debug(cxn, "Loop %d", it);
        err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
        if (err) {
            fprintf(stderr, "Could not allocate operation\n");
            exit(1);
        }

        err = riak_libevent_new(&event, rop, base);
        if (err) {
            return err;
        }
        riak_operation_set_error_cb(rop, example_error_cb);
        riak_operation_set_cb_data(rop, rop);

        // handle possible operations from the command line
        switch (operation) {
        case RIAK_COMMAND_PING:
            err = riak_async_register_ping(rop, (riak_response_callback)example_ping_cb);
            if (err) {
                fprintf(stderr, "No Ping [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETSERVERINFO:
            err = riak_async_register_serverinfo(rop, (riak_response_callback)example_serverinfo_cb);
            if (err) {
                fprintf(stderr, "Server Info Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GET:
            get_options = riak_get_options_new(cfg);
            if (get_options == NULL) {
                riak_log_critical(cxn, "%s","Could not allocate a Riak Get Options");
                return 1;
            }
            riak_get_options_set_basic_quorum(get_options, RIAK_TRUE);
            riak_get_options_set_r(get_options, 2);
            err = riak_async_register_get(rop, 
                                          bucket_type_bin,
                                          bucket_bin,
                                          key_bin, get_options,
                                          (riak_response_callback)example_get_cb);
            riak_get_options_free(cfg, &get_options);
            if (err) {
                fprintf(stderr, "Get Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_PUT:
            obj = riak_object_new(cfg);
            if (obj == NULL) {
                riak_log_critical(cxn, "%s","Could not allocate a Riak Object");
                return 1;
            }
            riak_object_set_bucket(cfg, obj, riak_binary_copy_from_string(cfg, args.bucket));
            if (args.has_bucket_type) {
                riak_object_set_bucket_type(cfg, obj, riak_binary_copy_from_string(cfg, args.bucket_type));
            }
            if (args.has_key) {
                riak_object_set_key(cfg, obj, riak_binary_copy_from_string(cfg, args.key));
            }
            riak_object_set_value(cfg, obj, riak_binary_copy_from_string(cfg, args.value));
            if (riak_object_get_bucket(obj) == NULL ||
                riak_object_get_value(obj) == NULL) {
                fprintf(stderr, "Could not allocate bucket/value\n");
                riak_free(cfg, &obj);
                exit(1);
            }
            put_options = riak_put_options_new(cfg);
            if (put_options == NULL) {
                riak_log_critical(cxn, "%s","Could not allocate a Riak Put Options");
                return 1;
            }
            riak_put_options_set_return_head(put_options, RIAK_TRUE);
            riak_put_options_set_return_body(put_options, RIAK_TRUE);
            err = riak_async_register_put(rop, obj, put_options, (riak_response_callback)example_put_cb);
            riak_object_free(cfg, &obj);
            riak_put_options_free(cfg, &put_options);
            if (err) {
                fprintf(stderr, "Put Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_DEL:
            delete_options = riak_delete_options_new(cfg);
            if (delete_options == NULL) {
                riak_log_critical(cxn, "%s","Could not allocate a Riak Delete Options");
                return 1;
            }
            riak_delete_options_set_w(delete_options, 1);
            riak_delete_options_set_dw(delete_options, 1);
            err = riak_async_register_delete(rop,
                                             bucket_type_bin,
                                             bucket_bin,
                                             key_bin,
                                             delete_options,
                                             (riak_response_callback)example_delete_cb);
            riak_delete_options_free(cfg, &delete_options);
            if (err) {
                fprintf(stderr, "Delete Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_LISTBUCKETS:
            err = riak_async_register_listbuckets(rop,
                                                  bucket_type_bin,
                                                  args.timeout * 1000,
                                                  (riak_response_callback)example_listbucket_cb);
            if (err) {
                fprintf(stderr, "List Buckets Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_LISTKEYS:
            err = riak_async_register_listkeys(rop,
                                               bucket_type_bin,
                                               bucket_bin,
                                               args.timeout * 1000,
                                               (riak_response_callback)example_listkey_cb);
            if (err) {
                fprintf(stderr, "List Keys Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETCLIENTID:
            err = riak_async_register_get_clientid(rop, (riak_response_callback)example_getclientid_cb);
            if (err) {
                fprintf(stderr, "Get ClientID Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_SETCLIENTID:
            err = riak_async_register_set_clientid(rop, value_bin, (riak_response_callback)example_setclientid_cb);
            if (err) {
                fprintf(stderr, "Set ClientID Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETBUCKET:
            err = riak_async_register_get_bucketprops(rop, bucket_type_bin, bucket_bin, (riak_response_callback)example_getbucketprops_cb);
            if (err) {
                fprintf(stderr, "Get Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_RESETBUCKET:
            err = riak_async_register_reset_bucketprops(rop, bucket_type_bin, bucket_bin, (riak_response_callback)example_resetbucketprops_cb);
            if (err) {
                fprintf(stderr, "Reset Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_SETBUCKET:
            props = riak_bucketprops_new(cfg);
            if (obj == NULL) {
                riak_log_critical(cxn, "%s", "Could not allocate a Riak Bucket Properties");
                return 1;
            }
            riak_bucketprops_set_last_write_wins(props, RIAK_FALSE);
            err = riak_async_register_set_bucketprops(rop, bucket_type_bin, bucket_bin, props, (riak_response_callback)example_setbucketprops_cb);
            if (err) {
                fprintf(stderr, "Set Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_MAPRED:
            err = riak_async_register_mapreduce(rop, content_type, value_bin, RIAK_FALSE, (riak_response_callback)example_mapreduce_cb);
            if (err) {
                fprintf(stderr, "Map/Reduce Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_INDEX:
            index_options = riak_2i_options_new(cfg);
            if (index_options == NULL) {
                riak_log_critical(cxn, "%s","Could not allocate Riak Secondary Index Options");
                return 1;
            }
            riak_2i_options_set_stream(index_options, RIAK_TRUE);
            riak_2i_options_set_timeout(index_options, 10000);
            riak_2i_options_set_key(cfg, index_options, value_bin);
            err = riak_async_register_2i(rop,
                                         bucket_type_bin,
                                         bucket_bin,
                                         index_bin,
                                         index_options,
                                         (riak_response_callback)example_2i_cb);
            riak_2i_options_free(cfg, &index_options);
            if (err) {
                fprintf(stderr, "Secondary Index Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_SEARCHQUERY:
            search_options = riak_search_options_new(cfg);
            if (search_options == NULL) {
                riak_log_critical(cxn, "%s","Could not allocate a Riak Search Options");
                return 1;
            }
            riak_search_options_set_start(search_options, 0);
            riak_search_options_set_rows(search_options, 50);
            err = riak_async_register_search(rop, bucket_bin, value_bin, search_options, (riak_response_callback)example_search_cb);
            riak_search_options_free(cfg, &search_options);
            if (err) {
                fprintf(stderr, "Search Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        default:
            usage(stderr, argv[0]);
            exit(1);
        }

        err = riak_libevent_send(rop, event);
        if (err) {
            riak_log_critical(cxn, "%s", "Could not send request");
            exit(1);
        }
    }

    // Terminates only on error or timeout
    event_base_dispatch(base);

    // cleanup
    event_base_free(base);
    riak_binary_free(cfg, &bucket_type_bin);
    riak_binary_free(cfg, &bucket_bin);
    riak_binary_free(cfg, &key_bin);
    riak_binary_free(cfg, &value_bin);
    riak_binary_free(cfg, &index_bin);
    riak_binary_free(cfg, &content_type);
    riak_config_free(&cfg);

    return 0;
}
