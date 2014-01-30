/*********************************************************************
 *
 * example.c: example Riak C Client application
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
#include "riak_command.h"
#include "example_call_backs.h"
#include "../adapters/riak_libevent.h"
#include <time.h>


//********************************************************************/
// Logging struct(s)/functions
//********************************************************************/
typedef struct {
    FILE *fp;
} example_log_data;

// log initialization callback, open files here etc.
riak_int32_t
example_log_init(void *ptr) {
    example_log_data* datum = (example_log_data*)ptr;
    datum->fp = fopen("riak.log", "w+");
    if (datum->fp == NULL) {
        return 1;
    }
    return 0;
}

// log cleanup callback, close files here etc.
void
example_log_cleanup(void *ptr) {
    example_log_data* datum = (example_log_data*)ptr;
    fclose(datum->fp);
}

// handle an incoming log message
void
example_log(void            *ptr,
            riak_log_level_t level,
            const char      *file,
            riak_size_t      filelen,
            const char      *func,
            riak_size_t      funclen,
            riak_uint32_t    line,
            const char      *format,
            va_list          args) {
    example_log_data* datum = (example_log_data*)ptr;
    time_t ltime;
    struct tm result;
    char stime[32];

    if (datum->fp == NULL) return;

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    strftime(stime, sizeof(stime), "%F %X %Z", &result);
    fprintf(datum->fp, "%s %s ", stime, riak_log_level_description(level));
    vfprintf(datum->fp, format, args);
    fprintf(datum->fp, "\n");
    fflush(datum->fp);
}



//********************************************************************/
// main application
//********************************************************************/

int
main(int   argc,
     char *argv[])
{
    riak_args args;
    int operation = riak_parse_args(argc, argv, &args);

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


    riak_object *obj;
    riak_bucketprops *props;
    riak_get_options *get_options;
    riak_delete_options *delete_options;
    riak_2index_options *index_options;
    char output[10240];
    struct event_base *base = event_base_new();
    if (base == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not create libevent base");
        exit(1);
    }
    int it;

    // create some sample binary values to use
    riak_binary *bucket_bin   = riak_binary_new_from_string(cfg, args.bucket); // Not copied
    riak_binary *key_bin      = riak_binary_new_from_string(cfg, args.key);   // Not copied
    riak_binary *value_bin    = riak_binary_new_from_string(cfg, args.value); // Not copied
    riak_binary *index_bin    = riak_binary_new_from_string(cfg, args.index); // Not copied
    riak_binary *content_type = riak_binary_new_from_string(cfg, "application/json");

    // check for memory allocation problems
    if (bucket_bin == NULL ||
        key_bin    == NULL ||
        value_bin  == NULL) {
        fprintf(stderr, "Could not allocate bucket/key/value/index\n");
        exit(1);
    }

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
        // TODO: Operation callbacks?  Needed at construction time?

        if (args.async) {
            err = riak_libevent_new(&event, rop, base);
            if (err) {
                return err;
            }
            riak_operation_set_error_cb(rop, example_error_cb);
            riak_operation_set_cb_data(rop, rop);
        }

        // handle possible operations from the command line
        switch (operation) {
        case RIAK_COMMAND_PING:
            if (args.async) {
                err = riak_async_register_ping(rop, (riak_response_callback)example_ping_cb);
            } else {
                err = riak_ping(cxn);
                printf("PONG\n");
            }
            if (err) {
                fprintf(stderr, "No Ping [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETSERVERINFO:
            if (args.async) {
                err = riak_async_register_serverinfo(rop, (riak_response_callback)example_serverinfo_cb);
            } else {
                riak_serverinfo_response *serverinfo_response;
                err = riak_serverinfo(cxn, &serverinfo_response);
                riak_serverinfo_response_print(serverinfo_response, output, sizeof(output));
                printf("%s\n", output);
                riak_serverinfo_response_free(cfg, &serverinfo_response);
            }
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
            if (args.async) {
                err = riak_async_register_get(rop, bucket_bin, key_bin, get_options, (riak_response_callback)example_get_cb);
            } else {
                riak_get_response *get_response;
                err = riak_get(cxn, bucket_bin, key_bin, get_options, &get_response);
                if (err == ERIAK_OK) {
                    riak_print_get_response(get_response, output, sizeof(output));
                    printf("%s\n", output);
                }
                riak_get_response_free(cfg, &get_response);
            }
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
            riak_object_set_bucket(obj, riak_binary_new_from_string(cfg, args.bucket)); // Not copied
            if (args.has_key) {
                riak_object_set_key(obj, riak_binary_new_from_string(cfg, args.key)); // Not copied
            }
            riak_object_set_value(obj, riak_binary_new_from_string(cfg, args.value)); // Not copied
            if (riak_object_get_bucket(obj) == NULL ||
                riak_object_get_value(obj) == NULL) {
                fprintf(stderr, "Could not allocate bucket/value\n");
                riak_free(cfg, &obj);
                exit(1);
            }
            riak_put_options *put_options = riak_put_options_new(cfg);
            if (put_options == NULL) {
                riak_log_critical(cxn, "%s","Could not allocate a Riak Put Options");
                return 1;
            }
            riak_put_options_set_return_head(put_options, RIAK_TRUE);
            riak_put_options_set_return_body(put_options, RIAK_TRUE);

            if (args.async) {
                err = riak_async_register_put(rop, obj, put_options, (riak_response_callback)example_put_cb);
            } else {
                riak_put_response *put_response;
                err = riak_put(cxn, obj, put_options, &put_response);
                if (err == ERIAK_OK) {
                    riak_put_response_print(put_response, output, sizeof(output));
                    printf("%s\n", output);
                }
                riak_put_response_free(cfg, &put_response);
            }
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
           if (args.async) {
                err = riak_async_register_delete(rop, bucket_bin, key_bin, delete_options, (riak_response_callback)example_delete_cb);
            } else {
                err = riak_delete(cxn, bucket_bin, key_bin, delete_options);
            }
            riak_delete_options_free(cfg, &delete_options);
            if (err) {
                fprintf(stderr, "Delete Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_LISTBUCKETS:
            if (args.async) {
                err = riak_async_register_listbuckets(rop, (riak_response_callback)example_listbucket_cb);
            } else {
                riak_listbuckets_response *bucket_response;
                err = riak_listbuckets(cxn, &bucket_response);
                if (err == ERIAK_OK) {
                    riak_listbuckets_response_print(bucket_response, output, sizeof(output));
                    printf("%s\n", output);
                }
                riak_listbuckets_response_free(cfg, &bucket_response);
            }
            if (err) {
                fprintf(stderr, "List Buckets Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_LISTKEYS:
            if (args.async) {
                err = riak_async_register_listkeys(rop, bucket_bin, args.timeout * 1000, (riak_response_callback)example_listkey_cb);
            } else {
                riak_listkeys_response *key_response;
                err = riak_listkeys(cxn, bucket_bin, args.timeout * 1000, &key_response);
                if (err == ERIAK_OK) {
                    riak_listkeys_response_print(key_response, output, sizeof(output));
                    printf("%s\n", output);
                }
                riak_listkeys_response_free(cfg, &key_response);
            }
            if (err) {
                fprintf(stderr, "List Keys Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETCLIENTID:
            if (args.async) {
                err = riak_async_register_get_clientid(rop, (riak_response_callback)example_getclientid_cb);
            } else {
                riak_get_clientid_response *getcli_response;
                err = riak_get_clientid(cxn, &getcli_response);
                if (err == ERIAK_OK) {
                    riak_get_clientid_response_print(getcli_response, output, sizeof(output));
                    printf("%s\n", output);
                }
                riak_get_clientid_response_free(cfg, &getcli_response);
            }
            if (err) {
                fprintf(stderr, "Get ClientID Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_SETCLIENTID:
            if (args.async) {
                err = riak_async_register_set_clientid(rop, value_bin, (riak_response_callback)example_setclientid_cb);
            } else {
                riak_set_clientid_response *setcli_response;
                err = riak_set_clientid(cxn, value_bin, &setcli_response);
                riak_set_clientid_response_free(cfg, &setcli_response);
            }
            if (err) {
                fprintf(stderr, "Set ClientID Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETBUCKET:
            if (args.async) {
                err = riak_async_register_get_bucketprops(rop, bucket_bin, (riak_response_callback)example_getbucketprops_cb);
            } else {
                riak_get_bucketprops_response *bucket_response;
                err = riak_get_bucketprops(cxn, bucket_bin, &bucket_response);
                if (err == ERIAK_OK) {
                    riak_get_bucketprops_response_print(bucket_response, output, sizeof(output));
                    printf("%s\n", output);
                }
                riak_get_bucketprops_response_free(cfg, &bucket_response);
            }
            if (err) {
                fprintf(stderr, "Get Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
            case RIAK_COMMAND_RESETBUCKET:
            if (args.async) {
                err = riak_async_register_reset_bucketprops(rop, bucket_bin, (riak_response_callback)example_resetbucketprops_cb);
            } else {
                riak_reset_bucketprops_response *bucket_response;
                err = riak_reset_bucketprops(cxn, bucket_bin, &bucket_response);
                if (err == ERIAK_OK) {
                    riak_reset_bucketprops_response_free(cfg, &bucket_response);
                }
            }
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
            if (args.async) {
                err = riak_async_register_set_bucketprops(rop, bucket_bin, props, (riak_response_callback)example_setbucketprops_cb);
            } else {
                riak_set_bucketprops_response *bucket_response;
                 err = riak_set_bucketprops(cxn, bucket_bin, props, &bucket_response);
                 if (err == ERIAK_OK) {
                     riak_set_bucketprops_response_free(cfg, &bucket_response);
                 }
            }
            if (err) {
                fprintf(stderr, "Set Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_MAPRED:
            if (args.async) {
                err = riak_async_register_mapreduce(rop, content_type, value_bin, RIAK_FALSE, (riak_response_callback)example_mapreduce_cb);
            } else {
                riak_mapreduce_response *mapred_response = NULL;
                 err = riak_mapreduce(cxn, content_type, value_bin, &mapred_response);
                 if (err == ERIAK_OK) {
                     riak_int32_t len   = sizeof(output);
                     riak_int32_t total = 0;
                     char *target       = output;
                     riak_mapreduce_response_print(mapred_response, &target, &len, &total);
                     printf("%s\n", output);
                 
                     riak_mapreduce_response_free(cfg, &mapred_response);
                 }
            }
            if (err) {
                fprintf(stderr, "Map/Reduce Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_INDEX:
            index_options = riak_2index_options_new(cfg);
            if (index_options == NULL) {
                riak_log_critical(cxn, "%s","Could not allocate Riak Secondary Index Options");
                return 1;
            }
            riak_2index_options_set_stream(index_options, RIAK_TRUE);
            riak_2index_options_set_timeout(index_options, 10000);
            riak_2index_options_set_key(cfg, index_options, value_bin);
            if (args.async) {
                err = riak_async_register_2index(rop, bucket_bin, index_bin, index_options, (riak_response_callback)example_2index_cb);
            } else {
                riak_2index_response *index_response = NULL;
                err = riak_2index(cxn, bucket_bin, index_bin, index_options, &index_response);
                if (err == ERIAK_OK) {
                    riak_int32_t len   = sizeof(output);
                    riak_int32_t total = 0;
                    char *target       = output;
                    riak_2index_response_print(index_response, &target, &len, &total);
                    printf("%s\n", output);
                }
                riak_2index_response_free(cfg, &index_response);
            }
            riak_2index_options_free(cfg, &index_options);
            if (err) {
                fprintf(stderr, "Secondary Index Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_SEARCHQUERY:
        default:
            usage(stderr, argv[0]);
        }

        if (args.async) {
            err = riak_libevent_send(rop, event);
            if (err) {
                riak_log_critical(cxn, "%s", "Could not send request");
                exit(1);
            }
        }
    }

    if (args.async) {
        // Terminates only on error or timeout
        event_base_dispatch(base);
    }

    // cleanup
    event_base_free(base);
    riak_free(cfg, &bucket_bin);
    riak_free(cfg, &key_bin);
    riak_free(cfg, &value_bin);
    riak_config_free(&cfg);

    return 0;
}
