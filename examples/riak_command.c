/*********************************************************************
 *
 * riak_command.h: Riak C Command Parser
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

#include <getopt.h>
#include "riak.h"
#include "riak_command.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"

struct _riak_command {
    // name of long option
    const char    *name;
    // Text description
    const char   *description;
    // Name of optional argument
    const char   *argument;
    // Optionals return value
    int           val;
    // True if --bucket is required
    riak_boolean_t needs_bucket;
    // True if --key is required
    riak_boolean_t needs_key;
    // True if --value is required
    riak_boolean_t needs_value;
};

static riak_command s_commands[] = {
    // These options set a flag.
    {"delete",       "Delete a key",                 NULL, RIAK_COMMAND_DEL,           RIAK_TRUE,  RIAK_TRUE,  RIAK_FALSE},
    {"get-bucket",   "Fetch bucket properties",      NULL, RIAK_COMMAND_GETBUCKET,     RIAK_TRUE,  RIAK_FALSE, RIAK_FALSE},
    {"get-clientid", "Fetch client identifier",      NULL, RIAK_COMMAND_GETCLIENTID,   RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"get",          "Fetch a key value",            NULL, RIAK_COMMAND_GET,           RIAK_TRUE,  RIAK_TRUE,  RIAK_FALSE},
    {"index",        "Index a bucket",               NULL, RIAK_COMMAND_INDEX,         RIAK_TRUE,  RIAK_FALSE, RIAK_FALSE},
    {"list-buckets", "List all buckets on a server", NULL, RIAK_COMMAND_LISTBUCKETS,   RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"list-keys",    "List all keys in a bucket",    NULL, RIAK_COMMAND_LISTKEYS,      RIAK_TRUE,  RIAK_FALSE, RIAK_FALSE},
    {"map-reduce",   "Execute map/reduce",           NULL, RIAK_COMMAND_MAPRED,        RIAK_TRUE,  RIAK_FALSE, RIAK_FALSE},
    {"ping",         "Look for signs of life",       NULL, RIAK_COMMAND_PING,          RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"put",          "Store a value in a key",       NULL, RIAK_COMMAND_PUT,           RIAK_TRUE,  RIAK_FALSE, RIAK_TRUE},
    {"reset-bucket", "Reset bucket properties",      NULL, RIAK_COMMAND_RESETBUCKET,   RIAK_TRUE,  RIAK_FALSE, RIAK_FALSE},
    {"search",       "Use 2i Search",                NULL, RIAK_COMMAND_SEARCHQUERY,   RIAK_TRUE,  RIAK_FALSE, RIAK_TRUE},
    {"server-info",  "Return server settings",       NULL, RIAK_COMMAND_GETSERVERINFO, RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"set-bucket",   "Store bucket properties",      NULL, RIAK_COMMAND_SETBUCKET,     RIAK_TRUE,  RIAK_FALSE, RIAK_FALSE},
    {"set-clientid", "Store client identifier",      NULL, RIAK_COMMAND_SETCLIENTID,   RIAK_FALSE, RIAK_FALSE, RIAK_TRUE},

    // These options don't set a flag.
    // We distinguish them by their indices.
    {"async",        "Asynchronous messaging", "",       'a', RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"bucket",       "",                       "name",   'b', RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"host",         "",                       "name",   'h', RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"iterate",      "",                       "times",  'i', RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"key",          "",                       "name",   'k', RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"port",         "",                       "number", 'p', RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"timeout",      "",                       "secs",   't', RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"type",         "",                       "type",   'T', RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {"value",        "",                       "val",    'v', RIAK_FALSE, RIAK_FALSE, RIAK_FALSE},
    {NULL, NULL, NULL, 0, RIAK_FALSE, RIAK_FALSE, RIAK_FALSE}
};

void
usage(FILE       *fp,
      const char *progname) {
    int i;
    fprintf(fp, "%s Usage:\n", progname);
    for(i = 0; s_commands[i].name != NULL; i++) {
        if (s_commands[i].argument == NULL) {
            fprintf(fp, "  --%-20s%s\n",
                    s_commands[i].name,
                    s_commands[i].description);
        } else {
            if (s_commands[i].argument[0] == '\0') {
                fprintf(fp, "  --%-20s%s\n",
                        s_commands[i].name,
                        s_commands[i].description);
            } else {
                fprintf(fp, "  --%s <%s>%s\n",
                        s_commands[i].name,
                        s_commands[i].argument,
                        s_commands[i].description);
            }
        }
    }
}

void
build_options(riak_command  *commands,
              struct option **long_options,
              int            *operation,
              char           *optstring) {
    riak_int32_t num_commands = 0;
    // Count the number of commands
    for(; commands[num_commands].name != NULL; num_commands++);
    struct option *opts = (struct option*)calloc(sizeof(struct option), num_commands+1);
    if (opts == NULL) {
        fprintf(stderr, "Could not allocate options array!\n");
        exit(1);
    }

    int i;
    for(i = 0; i < num_commands; i++) {
        // Does this relate to an operation?
        if (commands[i].argument == NULL) {
            opts[i].name = commands[i].name;
            opts[i].has_arg = no_argument;
            opts[i].flag = operation;
            opts[i].val = commands[i].val;
        } else {
            opts[i].name = commands[i].name;
            if (commands[i].argument[0] != '\0') {
                opts[i].has_arg = required_argument;
                *optstring++ = commands[i].val;
                *optstring++ = ':';
            } else {
                // Flag has no argument, but operation either
                opts[i].has_arg = no_argument;
                *optstring++ = commands[i].val;
            }
            opts[i].flag = NULL;
            opts[i].val = commands[i].val;
        }
    }
    *long_options = opts;
}

int
argument_check(FILE         *fp,
               const char   *progname,
               riak_command *commands,
               riak_args    *args) {
    int i;
    riak_boolean_t ok = RIAK_TRUE;
    for(i = 0; commands[i].name != NULL; i++) {
        if (args->operation == commands[i].val) {
            if (commands[i].needs_bucket && args->has_bucket != RIAK_TRUE) {
                fprintf(fp, "--bucket parameter required\n");
                ok = RIAK_FALSE;
            }
            if (commands[i].needs_key && args->has_key != RIAK_TRUE) {
                fprintf(fp, "--key parameter required\n");
                ok = RIAK_FALSE;
            }
            if (commands[i].needs_value && args->has_value != RIAK_TRUE) {
                fprintf(fp, "--value parameter required\n");
                ok = RIAK_FALSE;
            }
            break;
        }
    }

    // Case of not found
    if (commands[i].name == NULL) ok = RIAK_FALSE;

    if (!ok) {
        return -1;
    }
    return 0;
}


int
riak_parse_args(int           argc,
                char         *argv[],
                riak_args    *args) {

    static int operation;  // static required for getopt
    int  c;

    memset((void*)args, '\0', sizeof(riak_args));
    args->operation  = MSG_RPBERRORRESP;
    args->async      = RIAK_FALSE;
    args->iterate    = 1;
    args->port       = 10017;
    args->timeout    = 10;
    args->has_btype  = RIAK_FALSE;
    args->has_bucket = RIAK_FALSE;
    args->has_key    = RIAK_FALSE;
    args->has_value  = RIAK_FALSE;

    strcpy(args->host, "localhost");
    strcpy(args->portnum, "10017");

    char optstring[128];
    struct option *long_options = NULL;
    build_options(s_commands, &long_options, &operation, optstring);

    // Keep parsing args as long as we can
    while (1) {
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, optstring,
                        long_options, &option_index);

         // Detect the end of the options.
        if (c == -1)
            break;

        switch (c) {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0) {
                    break;
                }
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'a':
                args->async = RIAK_TRUE;
                break;

            case 'b':
                printf ("option -b with value `%s'\n", optarg);
                riak_strlcpy(args->bucket, optarg, sizeof(args->bucket));
                args->has_bucket = RIAK_TRUE;
                break;

            case 'h':
                printf ("option -h with value `%s'\n", optarg);
                riak_strlcpy(args->host, optarg, sizeof(args->host));
                break;

            case 'i':
                printf ("option -i with value `%s'\n", optarg);
                args->iterate = atol(optarg);
                break;

            case 'k':
                printf ("option -k with value `%s'\n", optarg);
                riak_strlcpy(args->key, optarg, sizeof(args->key));
                args->has_key = RIAK_TRUE;
                break;

            case 'p':
                printf ("option -p with value `%s'\n", optarg);
                riak_strlcpy(args->portnum, optarg, sizeof(args->portnum));
                args->port = atol(optarg);
                break;

            case 't':
                printf ("option -t with value `%s'\n", optarg);
                args->timeout = atol(optarg);
                break;

            case 'T':
                printf ("option -T with value `%s'\n", optarg);
                riak_strlcpy(args->btype, optarg, sizeof(args->btype));
                args->has_btype = RIAK_TRUE;
                break;
            case 'v':
                printf ("option -v with value `%s'\n", optarg);
                riak_strlcpy(args->value, optarg, sizeof(args->value));
                args->has_value = RIAK_TRUE;
                break;

            case '?':
                /* getopt_long already printed an error message. */
                operation = -1;
                break;

            default:
                return -1;
        }
    }
    args->operation = operation;
    if (argument_check(stderr, argv[0], s_commands, args) < 0) {
        operation = -1;
    }
    free(long_options);
    return operation;
}
