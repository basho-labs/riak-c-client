/*********************************************************************
 *
 * riak.h: Riak C Client Types
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

#ifndef RIAK_H
#define RIAK_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <event2/dns.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event.h>
#include "riak_types.h"
#include "riak_error.h"
#include "riak_context.h"
#include "riak_binary.h"
#include "riak_event.h"
#include "riak_object.h"
#include "riak_bucket_props.h"
#include "riak_messages.h"
#include "riak_log.h"

//
// Basic Synchronous Operations
//

/**
 * @brief Send a Ping Request
 * @param ctx Riak Context
 * @return ERIAK_OK on Pong response
 */
riak_error
riak_ping(riak_context *ctx);

/**
 * @brief Send a Server Info Request
 * @param ctx Riak Context
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_serverinfo(riak_context              *ctx,
                riak_serverinfo_response **response);

/**
 * @brief Synchronous Fetch request
 * @param ctx Riak Context
 * @param bucket Name of Riak bucket
 * @param key Name of Riak key
 * @param opts Fetch options
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_get(riak_context              *ctx,
         riak_binary               *bucket,
         riak_binary               *key,
         riak_get_options          *opts,
         riak_get_response        **response);

/**
 * @brief Synchronous Store request
 * @param ctx Riak Context
 * @param obj Object to be stored in Riak
 * @param opts Store options
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_put(riak_context       *ctx,
         riak_object        *obj,
         riak_put_options   *opts,
         riak_put_response **response);

/**
 * @brief Synchronous Delete request
 * @param ctx Riak Context
 * @param bucket Name of Riak bucket
 * @param key Name of Riak key
 * @param opts Delete options
 * @returns Error code
 */
riak_error
riak_delete(riak_context        *ctx,
            riak_binary         *bucket,
            riak_binary         *key,
            riak_delete_options *opts);

/**
 * @brief List all of the buckets on a server
 * @param ctx Riak Context
 * @param response Returned collection of bucket names
 * @return ERIAK_OK on Pong response
 */
riak_error
riak_listbuckets(riak_context               *ctx,
                 riak_listbuckets_response **repsonse);

/**
 * @brief List all of the keys in a bucket
 * @param ctx Riak Context
 * @param bucket Name of bucket
 * @param timeout How long to wait for a response
 * @param response Returned collection of key names
 * @return Error code
 */
riak_error
riak_listkeys(riak_context            *ctx,
              riak_binary             *bucket,
              riak_uint32_t            timeout,
              riak_listkeys_response **repsonse);

/**
 * @brief Synchronous setting of client ID request
 * @param ctx Riak Context
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_get_clientid(riak_context                *ctx,
                  riak_get_clientid_response **response);

/**
 * @brief Synchronous fetching of client ID request
 * @param ctx Riak Context
 * @param clientid Name of client id for current connection
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_set_clientid(riak_context                *ctx,
                  riak_binary                 *clientid,
                  riak_set_clientid_response **response);

/**
 * @brief List the properties associated with a bucket
 * @param ctx Riak Context
 * @param bucket Name of bucket
 * @param response Returned bucket properties
 * @return Error code
 */
riak_error
riak_get_bucketprops(riak_context                   *ctx,
                     riak_binary                    *bucket,
                     riak_get_bucketprops_response **repsonse);

/**
 * @brief Reset the properties associated with a bucket
 * @param ctx Riak Context
 * @param bucket Name of bucket
 * @param response Returned bucket properties
 * @return Error code
 */
riak_error
riak_reset_bucketprops(riak_context                     *ctx,
                       riak_binary                      *bucket,
                       riak_reset_bucketprops_response **response);

/**
 * @brief Update the properties associated with a bucket
 * @param ctx Riak Context
 * @param bucket Name of bucket
 * @param props Properties to set
 * @param response Returned bucket properties
 * @return Error code
 */
riak_error
riak_set_bucketprops(riak_context                   *ctx,
                     riak_binary                    *bucket,
                     riak_bucket_props              *props,
                     riak_set_bucketprops_response **response);

void riak_bucket_set_props(riak_context*);

void riak_bucket_get_props(riak_context*);

void riak_query_2i(riak_context*);

void riak_map_reduce(riak_context*);

#endif
