/*********************************************************************
 *
 * riak_connection-internal.h: Management of the Riak Connection
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

#ifndef _RIAK_CONNECTION_INTERNAL_H
#define _RIAK_CONNECTION_INTERNAL_H

#include <sys/time.h>

#define RIAK_HOST_MAX_LEN           256
#define RIAK_CONN_HOST_INIT_SIZE    10
#define RIAK_CONN_POOL_INIT_SIZE    10

struct _riak_connection {
    riak_config            *config;
    char                    hostname[RIAK_HOST_MAX_LEN];
    char                    portnum[RIAK_HOST_MAX_LEN]; // Keep as a string for getaddrinfo
    riak_connection_options opt;
    riak_addrinfo          *addrinfo;
    riak_socket_t           fd;
    struct timeval          open_time;
    struct timezone         open_tz;
    struct timeval          last_activity_time;
    struct timezone         last_activity_tz;

    riak_connection_pool    *pool;    // With which pool is this connection associated (NULL for none)
    riak_boolean_t           active;  // Is connection currently being used?
};

typedef struct _riak_connection_config {
    char          hostname[RIAK_HOST_MAX_LEN];
    char          portnum[RIAK_HOST_MAX_LEN]; // Keep as a string for getaddrinfo
    riak_uint32_t max_cxns;
    riak_array   *cxns;
} riak_connection_config;

struct _riak_connection_pool {
    riak_config   *cfg;             // Riak Configuration
    riak_uint32_t  max_total;       // Total number of connections
    riak_boolean_t lazy_connect;    // True if connections are created on-demand
    riak_array    *available_cxns;  // Queue of available connections
    riak_array    *in_use_cxns;     // Queue of currently in-use connections
    riak_array    *host_configs;    // Queue of host configurations
    riak_int64_t   idx;             // Which config is next in a round robin?
};

/**
 * @brief Add a new connection to the pool
 * @param pool Riak Connection Pool
 * @returns ERIAK_OUT_OF_MEMORY, ERIAK_DNS_RESOLUTION, ERIAK_UNINITIALIZED, ERIAK_CONNECT, ERIAK_MAX_CONNECTIONS
 */
riak_error
riak_connection_pool_add_connection(riak_connection_pool *pool);

/**
 * @brief Remove an existing connection from the pool
 * @param cxn Riak Connection
 * @returns ERIAK_NOT_FOUND
 */
riak_error
riak_connection_pool_remove_connection(riak_connection *cxn);


#endif // _RIAK_CONNECTION_INTERNAL_H
