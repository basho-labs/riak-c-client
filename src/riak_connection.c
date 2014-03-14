/*********************************************************************
 *
 * riak_connection.c: Management of the Riak Connection
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
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_connection.h"
#include "riak_config-internal.h"
#include "riak_connection-internal.h"
#include "riak_network.h"

riak_error
riak_connection_new(riak_config             *cfg,
                    riak_connection        **cxn_target,
                    const char              *hostname,
                    const char              *portnum,
                    riak_connection_options *opt_in) {

    riak_connection *cxn = riak_config_clean_allocate(cfg, sizeof(riak_connection));
    if (cxn == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not allocate a riak_connection");
        return ERIAK_OUT_OF_MEMORY;
    }
    *cxn_target = cxn;
    cxn->config = cfg;
    // Setup default options
    cxn->opt.resolver        = getaddrinfo;
    cxn->opt.socket_fn       = socket;
    cxn->opt.connect_fn      = connect;
    cxn->opt.close_fn        = close;
    cxn->opt.timeout.tv_sec  = 0;
    cxn->opt.timeout.tv_usec = 0;

    // Apply any user-defined overrides
    if (opt_in && opt_in->resolver) {
        cxn->opt.resolver = opt_in->resolver;
    }
    if (opt_in && opt_in->socket_fn) {
        cxn->opt.socket_fn = opt_in->socket_fn;
    }
    if (opt_in && opt_in->connect_fn) {
        cxn->opt.connect_fn = opt_in->connect_fn;
    }
    if (opt_in && opt_in->close_fn) {
        cxn->opt.close_fn = opt_in->close_fn;
    }
    if (opt_in && opt_in->timeout.tv_sec) {
        cxn->opt.timeout.tv_sec = opt_in->timeout.tv_sec;
    }
    if (opt_in && opt_in->timeout.tv_usec) {
        cxn->opt.timeout.tv_usec = opt_in->timeout.tv_usec;
    }

    riak_strlcpy(cxn->hostname, hostname, sizeof(cxn->hostname));
    riak_strlcpy(cxn->portnum, portnum, sizeof(cxn->portnum));

    riak_error err = riak_resolve_address(cfg, cxn->opt.resolver, hostname, portnum, &(cxn->addrinfo));
    if (err) {
        return ERIAK_DNS_RESOLUTION;
    }

    // TODO: Implement retry logic
    cxn->fd = riak_just_open_a_socket(cfg, cxn->addrinfo, cxn->opt.socket_fn, cxn->opt.connect_fn);
    if (cxn->fd < 0) {
        riak_log_critical_config(cfg, "%s", "Could not just open a socket");
        return ERIAK_CONNECT;
    }

    int result;
    // Optionally set the socket timeout
    if (cxn->opt.timeout.tv_sec > 0 || cxn->opt.timeout.tv_sec) {
        result = setsockopt(cxn->fd, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&cxn->opt.timeout, sizeof(struct timeval));
        if (result < 0) {
            riak_log_critical_config(cfg, "%s", "Could not set the socket send timeout");
            return ERIAK_CONNECT;
        }
        setsockopt(cxn->fd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&cxn->opt.timeout, sizeof(struct timeval));
        if (result < 0) {
            riak_log_critical_config(cfg, "%s", "Could not set the socket receive timeout");
            return ERIAK_CONNECT;
        }
    }

    // Collect initial stats
    result = gettimeofday(&cxn->open_time, &cxn->open_tz);
    if (result < 0) {
        memset(&cxn->open_time, '\0', sizeof(struct timeval));
        memset(&cxn->open_tz, '\0', sizeof(struct timezone));
    }
    return ERIAK_OK;
}

riak_socket_t
riak_connection_get_fd(riak_connection *cxn) {
    return cxn->fd;
}
riak_config*
riak_connection_get_config(riak_connection *cxn) {
    return cxn->config;
}

void riak_connection_free(riak_connection** cxn_target) {
    if (cxn_target == NULL || *cxn_target == NULL) return;
    riak_connection *cxn = *cxn_target;
    riak_config *cfg = riak_connection_get_config(cxn);

    if (cxn->fd) {
        cxn->opt.close_fn(cxn->fd);
    }
    if (cxn->addrinfo != NULL) freeaddrinfo(cxn->addrinfo);
    riak_free(cfg, cxn_target);
}

riak_error
riak_connection_pool_new(riak_config             *cfg,
                         riak_connection_pool   **pool_out,
                         riak_connection_options *opt,
                         riak_uint32_t            total_cxns,
                         riak_boolean_t           lazy_connect) {
    riak_connection_pool *pool = riak_config_clean_allocate(cfg, sizeof(riak_connection_pool));
    if (pool == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *pool_out            = NULL;
    pool->cfg            = cfg;
    pool->idx            = 0;
    pool->max_total      = total_cxns;
    pool->lazy_connect   = lazy_connect;
    pool->host_configs   = riak_array_new(cfg, RIAK_CONN_HOST_INIT_SIZE);
    if (pool->host_configs == NULL) {
        riak_free(cfg, &pool);
        return ERIAK_OUT_OF_MEMORY;
    }
    pool->available_cxns = riak_array_new(cfg, RIAK_CONN_POOL_INIT_SIZE);
    if (pool->available_cxns == NULL) {
        riak_array_free(&pool->host_configs);
        riak_free(cfg, &pool);
        return ERIAK_OUT_OF_MEMORY;
    }
    pool->in_use_cxns    = riak_array_new(cfg, RIAK_CONN_POOL_INIT_SIZE);
    if (pool->in_use_cxns == NULL) {
        riak_array_free(&pool->available_cxns);
        riak_array_free(&pool->host_configs);
        riak_free(cfg, &pool);
        return ERIAK_OUT_OF_MEMORY;
    }
    *pool_out = pool;
    return ERIAK_OK;
}

void
riak_connection_pool_free(riak_connection_pool **pool_in) {
    riak_connection_pool *pool = *pool_in;

    // Clean up any host configurations
    if (pool->host_configs) {
        riak_uint32_t num_hosts = riak_array_length(pool->host_configs);
        for(int i = 0; i < num_hosts; i++) {
            riak_connection_config *config = (riak_connection_config *)riak_array_pop(pool->host_configs);
            riak_free(pool->cfg, &config);
        }
    }

    // Close all unused connections
    if (pool->available_cxns) {
        riak_uint32_t num_avail = riak_array_length(pool->available_cxns);
        for(int i = 0; i < num_avail; i++) {
            riak_connection *cxn = (riak_connection *)riak_array_pop(pool->available_cxns);
            riak_connection_free(&cxn);
        }
    }

    // Close all in-use connections
    if (pool->in_use_cxns) {
        riak_uint32_t num_avail = riak_array_length(pool->in_use_cxns);
        for(int i = 0; i < num_avail; i++) {
            riak_connection *cxn = (riak_connection *)riak_array_pop(pool->in_use_cxns);
            riak_connection_free(&cxn);
        }
    }

    riak_free(pool->cfg, pool_in);
}
