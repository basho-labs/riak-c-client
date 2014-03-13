/*********************************************************************
 *
 * example.c: Rxample Riak C Client Synchronous Application
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

    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));

    int it;

    // create some sample binary values to use
    riak_binary *bucket_bin   = riak_binary_copy_from_string(cfg, args.bucket); // Not copied
    riak_binary *bucket_type_bin = NULL;
    if(args.has_bucket_type) {
        bucket_type_bin = riak_binary_copy_from_string(cfg, args.bucket_type);
    }
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
    riak_2index_options *index_options;
    riak_bucketprops    *props;
    riak_delete_options *delete_options;
    riak_get_options    *get_options;
    riak_object         *obj;
    riak_put_options    *put_options;
    riak_search_options *search_options;

    // Every possible message response type
    riak_2index_response            *index_response = NULL;
    riak_get_bucketprops_response   *props_response = NULL;
    riak_get_clientid_response      *getcli_response = NULL;
    riak_get_response               *get_response = NULL;
    riak_listbuckets_response       *bucket_response = NULL;
    riak_listkeys_response          *key_response = NULL;
    riak_mapreduce_response         *mapred_response = NULL;
    riak_put_response               *put_response = NULL;
    riak_reset_bucketprops_response *reset_response = NULL;
    riak_search_response            *search_response = NULL;
    riak_serverinfo_response        *serverinfo_response = NULL;
    riak_set_clientid_response      *setcli_response = NULL;

    // iterate through argv
    for(it = 0; it < args.iterate; it++) {
        riak_connection  *cxn   = NULL;
        // Create a connection with the default address resolver
        err = riak_connection_new(cfg, &cxn, args.host, args.portnum, NULL);
        if (err) {
            exit(1);
        }
        // if the application was configured using  -D_RIAK_DEBUG,
        // it will trace debug level messages
        riak_log_debug(cxn, "Loop %d", it);

        // handle possible operations from the command line
        switch (operation) {
        case RIAK_COMMAND_PING:
            err = riak_ping(cxn);
            if (err) {
                fprintf(stderr, "PANG [%s]\n", riak_strerror(err));
                exit(1);
            }
            printf("PONG\n");
            break;
        case RIAK_COMMAND_GETSERVERINFO:
            err = riak_serverinfo(cxn, &serverinfo_response);
            riak_serverinfo_response_print(&print_state, serverinfo_response);
            printf("%s\n", output);
            riak_serverinfo_response_free(cfg, &serverinfo_response);
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
            err = riak_get(cxn, bucket_bin, bucket_type_bin, key_bin, get_options, &get_response);
            if (err == ERIAK_OK) {
                riak_get_response_print(&print_state, get_response);
                printf("%s\n", output);
            }
            riak_get_response_free(cfg, &get_response);
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

            err = riak_put(cxn, obj, put_options, &put_response);
            if (err == ERIAK_OK) {
                riak_put_response_print(&print_state, put_response);
                printf("%s\n", output);
            }
            riak_put_response_free(cfg, &put_response);
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
            err = riak_delete(cxn, bucket_bin, bucket_type_bin, key_bin, delete_options);
            riak_delete_options_free(cfg, &delete_options);
            if (err) {
                fprintf(stderr, "Delete Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_LISTBUCKETS:
            err = riak_listbuckets(cxn,
                                   bucket_type_bin,
                                   args.timeout * 1000,
                &bucket_response);
            if (err == ERIAK_OK) {
                riak_listbuckets_response_print(&print_state, bucket_response);
                printf("%s\n", output);
            }
            riak_listbuckets_response_free(cfg, &bucket_response);
            if (err) {
                fprintf(stderr, "List Buckets Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_LISTKEYS:
            err = riak_listkeys(cxn,
                                bucket_bin,
                                bucket_type_bin,
                                args.timeout * 1000,
                                &key_response);
            if (err == ERIAK_OK) {
                riak_listkeys_response_print(&print_state, key_response);
                printf("%s\n", output);
            }
            riak_listkeys_response_free(cfg, &key_response);
            if (err) {
                fprintf(stderr, "List Keys Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETCLIENTID:
            err = riak_get_clientid(cxn, &getcli_response);
            if (err == ERIAK_OK) {
                riak_get_clientid_response_print(&print_state, getcli_response);
                printf("%s\n", output);
            }
            riak_get_clientid_response_free(cfg, &getcli_response);
            if (err) {
                fprintf(stderr, "Get ClientID Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_SETCLIENTID:
            err = riak_set_clientid(cxn, value_bin, &setcli_response);
            riak_set_clientid_response_free(cfg, &setcli_response);
            if (err) {
                fprintf(stderr, "Set ClientID Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_GETBUCKET:
            err = riak_get_bucketprops(cxn, bucket_bin, &props_response);
            if (err == ERIAK_OK) {
                riak_get_bucketprops_response_print(&print_state, props_response);
                printf("%s\n", output);
            }
            riak_get_bucketprops_response_free(cfg, &props_response);
            if (err) {
                fprintf(stderr, "Get Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_RESETBUCKET:
            err = riak_reset_bucketprops(cxn, bucket_bin, &reset_response);
            if (err == ERIAK_OK) {
                riak_reset_bucketprops_response_free(cfg, &reset_response);
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
            riak_set_bucketprops_response *bucket_response = NULL;
             err = riak_set_bucketprops(cxn, bucket_bin, props, &bucket_response);
             if (err == ERIAK_OK) {
                 riak_set_bucketprops_response_free(cfg, &bucket_response);
             }
            if (err) {
                fprintf(stderr, "Set Bucket Properties Problems [%s]\n", riak_strerror(err));
                exit(1);
            }
            break;
        case RIAK_COMMAND_MAPRED:

             err = riak_mapreduce(cxn, content_type, value_bin, &mapred_response);
             if (err == ERIAK_OK) {
                 riak_mapreduce_response_print(&print_state, mapred_response);
                 printf("%s\n", output);

                 riak_mapreduce_response_free(cfg, &mapred_response);
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
            err = riak_2index(cxn, bucket_bin, index_bin, index_options, &index_response);
            if (err == ERIAK_OK) {
                riak_2index_response_print(&print_state, index_response);
                printf("%s\n", output);
            }
            riak_2index_response_free(cfg, &index_response);
            riak_2index_options_free(cfg, &index_options);
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
            err = riak_search(cxn, bucket_bin, value_bin, search_options, &search_response);
            if (err == ERIAK_OK) {
                riak_search_response_print(&print_state, search_response);
                printf("%s\n", output);
            }
            riak_search_response_free(cfg, &search_response);
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
    }

    // cleanup
    riak_binary_free(cfg, &bucket_bin);
    riak_binary_free(cfg, &bucket_type_bin);
    riak_binary_free(cfg, &key_bin);
    riak_binary_free(cfg, &value_bin);
    riak_binary_free(cfg, &index_bin);
    riak_binary_free(cfg, &content_type);
    riak_config_free(&cfg);

    return 0;
}
