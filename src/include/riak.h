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
#include "riak_log_config.h"
#include "riak_error.h"
#include "riak_config.h"
#include "riak_binary.h"
#include "riak_connection.h"
#include "riak_object.h"
#include "riak_bucket_props.h"
#include "riak_messages.h"
#include "riak_options.h"
#include "riak_log.h"

//
// Basic Synchronous Operations
//

/**
 * @brief Send a Ping Request
 * @param cfg Riak Configuration
 * @return ERIAK_OK on Pong response
 */
riak_error
riak_ping(riak_config *cfg);

/**
 * @brief Send a Server Info Request
 * @param cfg Riak Configuration
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_serverinfo(riak_config              *cfg,
                riak_serverinfo_response **response);

/**
 * @brief Synchronous Fetch request
 * @param cfg Riak Configuration
 * @param bucket Name of Riak bucket
 * @param key Name of Riak key
 * @param opts Fetch options
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_get(riak_config              *cfg,
         riak_binary               *bucket,
         riak_binary               *key,
         riak_get_options          *opts,
         riak_get_response        **response);

/**
 * @brief Synchronous Store request
 * @param cfg Riak Configuration
 * @param obj Object to be stored in Riak
 * @param opts Store options
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_put(riak_config       *cfg,
         riak_object        *obj,
         riak_put_options   *opts,
         riak_put_response **response);

/**
 * @brief Synchronous Delete request
 * @param cfg Riak Configuration
 * @param bucket Name of Riak bucket
 * @param key Name of Riak key
 * @param opts Delete options
 * @returns Error code
 */
riak_error
riak_delete(riak_config        *cfg,
            riak_binary         *bucket,
            riak_binary         *key,
            riak_delete_options *opts);

/**
 * @brief List all of the buckets on a server
 * @param cfg Riak Configuration
 * @param response Returned collection of bucket names
 * @return ERIAK_OK on Pong response
 */
riak_error
riak_listbuckets(riak_config               *cfg,
                 riak_listbuckets_response **repsonse);

/**
 * @brief List all of the keys in a bucket
 * @param cfg Riak Configuration
 * @param bucket Name of bucket
 * @param timeout How long to wait for a response
 * @param response Returned collection of key names
 * @return Error code
 */
riak_error
riak_listkeys(riak_config            *cfg,
              riak_binary             *bucket,
              riak_uint32_t            timeout,
              riak_listkeys_response **repsonse);

/**
 * @brief Synchronous setting of client ID request
 * @param cfg Riak Configuration
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_get_clientid(riak_config                *cfg,
                  riak_get_clientid_response **response);

/**
 * @brief Synchronous fetching of client ID request
 * @param cfg Riak Configuration
 * @param clientid Name of client id for current connection
 * @param response Returned Fetched data
 * @returns Error code
 */
riak_error
riak_set_clientid(riak_config                *cfg,
                  riak_binary                 *clientid,
                  riak_set_clientid_response **response);

/**
 * @brief List the properties associated with a bucket
 * @param cfg Riak Configuration
 * @param bucket Name of bucket
 * @param response Returned bucket properties
 * @return Error code
 */
riak_error
riak_get_bucketprops(riak_config                   *cfg,
                     riak_binary                    *bucket,
                     riak_get_bucketprops_response **repsonse);

/**
 * @brief Reset the properties associated with a bucket
 * @param cfg Riak Configuration
 * @param bucket Name of bucket
 * @param response Returned bucket properties
 * @return Error code
 */
riak_error
riak_reset_bucketprops(riak_config                     *cfg,
                       riak_binary                      *bucket,
                       riak_reset_bucketprops_response **response);

/**
 * @brief Update the properties associated with a bucket
 * @param cfg Riak Configuration
 * @param bucket Name of bucket
 * @param props Properties to set
 * @param response Returned bucket properties
 * @return Error code
 */
riak_error
riak_set_bucketprops(riak_config                   *cfg,
                     riak_binary                    *bucket,
                     riak_bucket_props              *props,
                     riak_set_bucketprops_response **response);

void riak_bucket_set_props(riak_config*);

void riak_bucket_get_props(riak_config*);

void riak_query_2i(riak_config*);

void riak_map_reduce(riak_config*);

//
// A S Y N C H R O N O U S
//

/**
 * @brief Create a Riak Connection for async use
 * @param cfg Riak Configuration
 * @param cxn Riak Connection (out)
 * @returns Error code
 */
riak_error
riak_async_create_event(riak_config *cfg,
                        riak_connection  **cxn);

riak_error
riak_async_register_ping(riak_connection            *cxn,
                         riak_response_callback cb);

riak_error
riak_async_register_serverinfo(riak_connection            *cxn,
                               riak_response_callback cb);

riak_error
riak_async_register_get(riak_connection            *cxn,
                        riak_binary           *bucket,
                        riak_binary           *key,
                        riak_get_options      *get_options,
                        riak_response_callback cb);

riak_error
riak_async_register_put(riak_connection            *cxn,
                        riak_object           *riak_obj,
                        riak_put_options      *options,
                        riak_response_callback cb);
riak_error
riak_async_register_delete(riak_connection            *cxn,
                           riak_binary           *bucket,
                           riak_binary           *key,
                           riak_delete_options   *options,
                           riak_response_callback cb);

riak_error
riak_async_register_listbuckets(riak_connection            *cxn,
                                riak_response_callback cb);

riak_error
riak_async_register_listkeys(riak_connection            *cxn,
                             riak_binary           *bucket,
                             riak_uint32_t          timeout,
                             riak_response_callback cb );

riak_error
riak_async_register_get_clientid(riak_connection            *cxn,
                                 riak_response_callback cb);

riak_error
riak_async_register_set_clientid(riak_connection            *cxn,
                                 riak_binary           *clientid,
                                 riak_response_callback cb);

riak_error
riak_async_register_get_bucketprops(riak_connection            *cxn,
                                    riak_binary           *bucket,
                                    riak_response_callback cb);
riak_error
riak_async_register_reset_bucketprops(riak_connection            *cxn,
                                      riak_binary           *bucket,
                                      riak_response_callback cb);
riak_error
riak_async_register_set_bucketprops(riak_connection            *cxn,
                                    riak_binary           *bucket,
                                    riak_bucket_props     *props,
                                    riak_response_callback cb);

riak_error
riak_async_send_msg(riak_connection *cxn);

#endif
