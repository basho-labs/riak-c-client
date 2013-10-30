/*********************************************************************
 *
 * riak_command.h: Riak C Parse command-line (and usage)
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

#ifndef _RIAK_COMMAND_H
#define _RIAK_COMMAND_H

typedef enum {
    RIAK_COMMAND_PING = 0,
    RIAK_COMMAND_GETCLIENTID,
    RIAK_COMMAND_SETCLIENTID,
    RIAK_COMMAND_GETSERVERINFO,
    RIAK_COMMAND_GET,
    RIAK_COMMAND_PUT,
    RIAK_COMMAND_DEL,
    RIAK_COMMAND_LISTBUCKETS,
    RIAK_COMMAND_LISTKEYS,
    RIAK_COMMAND_GETBUCKET,
    RIAK_COMMAND_SETBUCKET,
    RIAK_COMMAND_MAPRED,
    RIAK_COMMAND_INDEX,
    RIAK_COMMAND_SEARCHQUERY,
    RIAK_COMMAND_RESETBUCKET,
    RIAK_COMMAND_GETBUCKETTYPE,
    RIAK_COMMAND_SETBUCKETTYPE,
    RIAK_COMMAND_RESETBUCKETTYPE,
    RIAK_COMMAND_CSBUCKET,
    RIAK_COMMAND_COUNTERUPDATE,
    RIAK_COMMAND_COUNTERGET,
    RIAK_COMMAND_YOKOZUNAINDEXGET,
    RIAK_COMMAND_YOKOZUNAINDEXPUT,
    RIAK_COMMAND_YOKOZUNAINDEXDELETE,
    RIAK_COMMAND_YOKOZUNASCHEMAGET,
    RIAK_COMMAND_YOKOZUNASCHEMAPUT,
    RIAK_COMMAND_DTFETCH,
    RIAK_COMMAND_DTUPDATE,
    RIAK_COMMAND_AUTH,
} riak_command_msg;

typedef struct _riak_command riak_command;

typedef struct {
    int            operation;
    riak_boolean_t async;
    riak_int32_t   iterate;
    riak_int32_t   port;
    riak_int32_t   timeout;
    riak_boolean_t has_bucket;
    riak_boolean_t has_key;
    riak_boolean_t has_value;
    char bucket[1024];
    char host[256];
    char portnum[6];
    char key[1024];
    char value[1024];
} riak_args;

void
usage(FILE       *fp,
      const char *progname);

/**
 * @brief Parse the command-line arguments
 * @param argc Number of arguments
 * @param argv Value of arguments
 * @param args Return value of parsed arguments
 * @return -1 on failure; > 0 on success
 */
int
riak_parse_args(int           argc,
                char         *argv[],
                riak_args    *args);

#endif

