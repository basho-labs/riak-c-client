/*********************************************************************
 *
 * riak_connection.h: Management of the Riak Connection
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

#ifndef _RIAK_CONNECTION_H
#define _RIAK_CONNECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#define RIAK_CONNECTION_RETRIES     5

typedef struct _riak_connection riak_connection;
typedef struct _riak_connection_pool riak_connection_pool;

// Optional networking overrides per connection
typedef struct _riak_connection_options {
    riak_addr_resolver resolver;     // IP Address resolving function (NULL for default)
    riak_socket_fn     socket_fn;    // Socket creating function (NULL for default)
    riak_connect_fn    connect_fn;   // Socket connecting function (NULL for default)
    riak_close_fn      close_fn;     // Socket closing function (NULL for default)
    struct timeval     timeout;      // >0 is timeout in seconds & microseconds
} riak_connection_options;

/**
 * @brief Construct a Riak event
 * @param cfg Riak config for memory allocation
 * @param cxn Riak Connection (out)
 * @param hostname Name of Riak server
 * @param portnum Riak PBC port number
 * @param opt Riak Connection Options (NULL will apply all defaults)
 * @returns Error code
 */
riak_error
riak_connection_new(riak_config             *cfg,
                    riak_connection        **cxn,
                    const char              *hostname,
                    const char              *portnum,
                    riak_connection_options *opt);
/**
 * @brief Cleanup memory used by a Riak Connection
 * @param re Riak Connection
 */
void
riak_connection_free(riak_connection** re);

/**
 * @brief Access the underlying File Descriptor
 * @param cxn Riak Connection
 * @returns Socket File Descriptor
 */
riak_socket_t
riak_connection_get_fd(riak_connection *cxn);

/**
 * @brief Access the `riak_config` inside the `riak_connection`
 * @param cxn Riak Connection
 * @returns Riak Configuration
 */
riak_config*
riak_connection_get_config(riak_connection *cxn);

//
//
// C O N N E C T I O N   P O O L S
//
//

/**
 * @brief Create a new connection pool
 * @param cfg Riak Configuration
 * @param pool Riak Connection Pool (out)
 * @param opt Optional Riak Connection options, applied to all new connections
 * @param total_cxn Maximum allowed number of connections (0 for "unlimited")
 * @param lazy_connect If true, open connections on demand
 * @returns ERIAK_OUT_OF_MEMORY, ERIAK_DNS_RESOLUTION, ERIAK_CONNECT, ...
 */
riak_error
riak_connection_pool_new(riak_config             *cfg,
                         riak_connection_pool   **pool,
                         riak_connection_options *opt,
                         riak_uint32_t            total_cxns,
                         riak_boolean_t           lazy_connect);
#define riak_connection_pool_new_default(C,P) (riak_connection_pool_new((C),(P),0,RIAK_TRUE))

/**
 * @brief Free up the memory associated with a connection pool
 * NOTE: This closes all available connections
 * @param pool Riak Connection Pool
 * @returns Error Code
 */
riak_error
riak_connection_pool_free(riak_connection_pool **pool);

/**
 * @brief Register a node for creating connections
 * @param pool Riak Connection Pool
 * @param hostname Name of Riak server
 * @param portnum Riak PBC port number
 * @param max_cxn Maximum allowed number of connections (0 for "unlimited")
 * @returns Error Code
 */
riak_error
riak_connection_pool_add_node(riak_connection_pool *pool,
                              const char           *hostname,
                              const char           *portnum,
                              riak_uint32_t         max_cxns);

/**
 * @brief Remove a node from the list of connections
 * @param pool Riak Connection Pool
 * @param hostname Name of Riak server
 * @param portnum Riak PBC port number
 * @returns Error Code
 */
riak_error
riak_connection_pool_remove_node(riak_connection_pool *pool,
                                 const char           *hostname,
                                 const char           *portnum);

/**
 * @brief Get a connection from the pool to use
 * @param pool Riak Connection Pool
 * @returns Error Code
 */
riak_error
riak_connection_pool_get(riak_connection_pool *pool,
                         riak_connection     **cxn);

/**
 * @brief Return a connection back to the pool
 * @param pool Riak Connection Pool
 * @returns Error Code
 */
riak_error
riak_connection_pool_return(riak_connection_pool *pool,
                            riak_connection      *cxn);

#ifdef __cplusplus
}
#endif

#endif // _RIAK_CONNECTION_H
