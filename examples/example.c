/*********************************************************************
 *
 * example.h: Riak C Main
 *
 * Copyright (c) 2007-2013 Basho Technologies, Inc.  All Rights Reserved.
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
#include "riak_command.h"
#include "riak_event.h"
 // TODO: Move sample callbacks into examples?
#include "riak_call_backs.h"

int
main(int   argc,
     char *argv[])
{
    riak_args args;
    int operation = riak_parse_args(argc, argv, &args);

#ifdef _RIAK_DEBUG
    event_enable_debug_mode();
#endif

    // if you see an error such as "Could not initialize logging"
    // make sure you have zlog.conf in the same directory as the
    // binary (ie ./examples/build/zlog.conf)

    // a riak_context serves as your per-thread state to interact with Riak
    riak_context *ctx;
    riak_error err = riak_context_new_default(&ctx);
    if (err) {
        exit(1);
    }
    err = riak_context_add_default_connection(ctx, args.host, args.portnum);
    if (err) {
        exit(1);
    }
    err = riak_context_add_logging(ctx, NULL);
    if (err) {
        exit(1);
    }
    riak_event  *rev = NULL;
    riak_object *obj;
    riak_bucket_props *props;
    char output[10240];
    int it;

    riak_binary *bucket_bin = riak_binary_new_from_string(ctx, args.bucket); // Not copied
    riak_binary *key_bin    = riak_binary_new_from_string(ctx, args.key); // Not copied
    riak_binary *value_bin  = riak_binary_new_from_string(ctx, args.value); // Not copied
    if (bucket_bin == NULL ||
        key_bin    == NULL ||
        value_bin  == NULL) {
        fprintf(stderr, "Could not allocate bucket/key/value\n");
        exit(1);
    }

    for(it = 0; it < args.iterate; it++) {
        riak_log_debug_context(ctx, "Loop %d", it);

        if (args.async) {
            err = riak_async_create_event(ctx, &rev);
            if (err) {
                return err;
            }
        }
        switch (operation) {
        case RIAK_COMMAND_PING:
            if (args.async) {
                err = riak_async_register_ping(rev, (riak_response_callback)ping_cb);
            } else {
                err = riak_ping(ctx);
            }
            if (err) {
                fprintf(stderr, "No Ping [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETSERVERINFO:
            if (args.async) {
                err = riak_async_register_serverinfo(rev, (riak_response_callback)serverinfo_cb);
            } else {
                riak_serverinfo_response *serverinfo_response;
                err = riak_serverinfo(ctx, &serverinfo_response);
                riak_print_serverinfo_response(serverinfo_response, output, sizeof(output));
                printf("%s\n", output);
                riak_free_serverinfo_response(ctx, &serverinfo_response);
            }
            if (err) {
                fprintf(stderr, "Server Info Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GET:
            if (args.async) {
                err = riak_async_register_get(rev, bucket_bin, key_bin, NULL, (riak_response_callback)get_cb);
            } else {
                riak_get_response *get_response;
                err = riak_get(ctx, bucket_bin, key_bin, NULL, &get_response);
                if (err == ERIAK_OK) {
                    riak_print_get_response(get_response, output, sizeof(output));
                    printf("%s\n", output);
                }
                riak_free_get_response(ctx, &get_response);
            }
            if (err) {
                fprintf(stderr, "Get Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_PUT:
            obj = riak_object_new(ctx);
            if (obj == NULL) {
                riak_log_fatal(rev, "%s","Could not allocate a Riak Object");
                return 1;
            }
            riak_object_set_bucket(obj, riak_binary_new_from_string(ctx, args.bucket)); // Not copied
            riak_object_set_value(obj, riak_binary_new_from_string(ctx, args.value)); // Not copied
            if (riak_object_get_bucket(obj) == NULL ||
                riak_object_get_value(obj) == NULL) {
                fprintf(stderr, "Could not allocate bucket/value\n");
                riak_free(ctx, &obj);
                exit(1);
            }
            riak_put_options *put_options = riak_put_options_new(ctx);
            if (put_options == NULL) {
                riak_log_fatal(rev, "%s","Could not allocate a Riak Put Options");
                return 1;
            }
            riak_put_options_set_return_head(put_options, RIAK_FALSE);
            riak_put_options_set_return_body(put_options, RIAK_FALSE);

            if (args.async) {
                err = riak_async_register_put(rev, obj, put_options, (riak_response_callback)put_cb);
            } else {
                riak_put_response *put_response;
                err = riak_put(ctx, obj, put_options, &put_response);
                if (err != ERIAK_OK) {
                    riak_print_put_response(put_response, output, sizeof(output));
                    printf("%s\n", output);
                }
                riak_free_put_response(ctx, &put_response);
            }
            riak_object_free(ctx, &obj);
            if (err) {
                fprintf(stderr, "Put Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_DEL:
            if (args.async) {
                err = riak_async_register_delete(rev, bucket_bin, key_bin, NULL, (riak_response_callback)delete_cb);
            } else {
                err = riak_delete(ctx, bucket_bin, key_bin, NULL);
            }
            if (err) {
                fprintf(stderr, "Delete Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_LISTBUCKETS:
            if (args.async) {
                err = riak_async_register_listbuckets(rev, (riak_response_callback)listbucket_cb);
            } else {
                riak_listbuckets_response *bucket_response;
                err = riak_listbuckets(ctx, &bucket_response);
                if (err == ERIAK_OK) {
                    riak_print_listbuckets_response(bucket_response, output, sizeof(output));
                    riak_log_debug_context(ctx, "%s", output);
                }
                riak_free_listbuckets_response(ctx, &bucket_response);
            }
            if (err) {
                fprintf(stderr, "List Buckets Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_LISTKEYS:
            if (args.async) {
                err = riak_async_register_listkeys(rev, bucket_bin, args.timeout * 1000, (riak_response_callback)listkey_cb);
            } else {
                riak_listkeys_response *key_response;
                err = riak_listkeys(ctx, bucket_bin, args.timeout * 1000, &key_response);
                if (err == ERIAK_OK) {
                    riak_print_listkeys_response(key_response, output, sizeof(output));
                    riak_log_debug_context(ctx, "%s", output);
                }
                riak_free_listkeys_response(ctx, &key_response);
            }
            if (err) {
                fprintf(stderr, "List Keys Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETCLIENTID:
            if (args.async) {
                err = riak_async_register_get_clientid(rev, (riak_response_callback)getclientid_cb);
            } else {
                riak_get_clientid_response *getcli_response;
                err = riak_get_clientid(ctx, &getcli_response);
                if (err == ERIAK_OK) {
                    riak_print_get_clientid_response(getcli_response, output, sizeof(output));
                    printf("%s\n", output);
                }
                riak_free_get_clientid_response(ctx, &getcli_response);
            }
            if (err) {
                fprintf(stderr, "Get ClientID Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_SETCLIENTID:
            if (args.async) {
                err = riak_async_register_set_clientid(rev, value_bin, (riak_response_callback)setclientid_cb);
            } else {
                riak_set_clientid_response *setcli_response;
                err = riak_set_clientid(ctx, value_bin, &setcli_response);
                riak_free_set_clientid_response(ctx, &setcli_response);
            }
            if (err) {
                fprintf(stderr, "Set ClientID Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETBUCKET:
            if (args.async) {
                err = riak_async_register_get_bucketprops(rev, bucket_bin, (riak_response_callback)getbucketprops_cb);
            } else {
                riak_get_bucketprops_response *bucket_response;
                err = riak_get_bucketprops(ctx, bucket_bin, &bucket_response);
                if (err == ERIAK_OK) {
                    riak_print_get_bucketprops_response(bucket_response, output, sizeof(output));
                    riak_log_debug_context(ctx, "%s", output);
                }
                riak_free_get_bucketprops_response(ctx, &bucket_response);
            }
            if (err) {
                fprintf(stderr, "Get Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
            case RIAK_COMMAND_RESETBUCKET:
            if (args.async) {
                err = riak_async_register_reset_bucketprops(rev, bucket_bin, (riak_response_callback)resetbucketprops_cb);
            } else {
                riak_reset_bucketprops_response *bucket_response;
                err = riak_reset_bucketprops(ctx, bucket_bin, &bucket_response);
            }
            if (err) {
                fprintf(stderr, "Reset Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_SETBUCKET:
            props = riak_bucket_props_new(ctx);
            if (obj == NULL) {
                riak_log_fatal(rev, "%s", "Could not allocate a Riak Bucket Properties");
                return 1;
            }
            riak_bucket_props_set_last_write_wins(props, RIAK_FALSE);
            if (args.async) {
                err = riak_async_register_set_bucketprops(rev, bucket_bin, props, (riak_response_callback)setbucketprops_cb);
            } else {
                riak_set_bucketprops_response *bucket_response;
                 err = riak_set_bucketprops(ctx, bucket_bin, props, &bucket_response);
            }
            if (err) {
                fprintf(stderr, "Reset Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        default:
            usage(stderr, argv[0]);
        }

        if (args.async) {
            err = riak_async_send_msg(rev);
            if (err) {
                riak_log_fatal(rev, "%s", "Could not send request");
                exit(1);
            }
        }
    }
    // What has been queued up
    fflush(stdout);

    if (args.async) {
        // Terminates only on error or timeout
        riak_event_loop(ctx);
    }

    riak_free(ctx, &bucket_bin);
    riak_free(ctx, &key_bin);
    riak_free(ctx, &value_bin);
    riak_context_free(&ctx);

    return 0;
}
