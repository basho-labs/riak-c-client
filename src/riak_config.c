/*********************************************************************
 *
 * riak_config.c: Management of the Riak connection config
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
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_network.h"
#include "riak_config-internal.h"
#include <zlog.h>

extern ProtobufCAllocator protobuf_c_default_allocator;

riak_error
riak_config_new(riak_config      **config,
                 riak_alloc_fn     alloc,
                 riak_realloc_fn   realloc,
                 riak_free_fn      freeme,
                 riak_pb_alloc_fn  pb_alloc,
                 riak_pb_free_fn   pb_free) {
    *config                   = NULL;
    riak_alloc_fn   alloc_fn   = malloc;
    riak_realloc_fn realloc_fn = realloc;
    riak_free_fn    free_fn    = free;


    if (alloc != NULL) {
        alloc_fn = alloc;
    }
    if (realloc != NULL) {
        realloc_fn = realloc;
    }
    if (freeme != NULL) {
        free_fn = freeme;
    }
    riak_config* cfg = (riak_config*)(alloc_fn)(sizeof(riak_config));
    if (cfg == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset((void*)cfg, '\0', sizeof(riak_config));
    cfg->malloc_fn    = alloc_fn;
    cfg->realloc_fn   = realloc_fn;
    cfg->free_fn      = free_fn;
    cfg->pb_allocator = NULL;
    if (pb_alloc != NULL && pb_free != NULL) {
        cfg->pb_allocator = &protobuf_c_default_allocator;
        cfg->pb_allocator->alloc = pb_alloc;
        cfg->pb_allocator->tmp_alloc = pb_alloc;
        cfg->pb_allocator->free = pb_free;
    }

    cfg->base = event_base_new();
    if (cfg->base == NULL) {
        riak_config_free(&cfg);
        return ERIAK_EVENT;
    }

    *config = cfg;
    return ERIAK_OK;
}

riak_error
riak_config_add_connection(riak_config        *cfg,
                            riak_addr_resolver resolver,
                            const char        *hostname,
                            const char        *portnum) {
    if (cfg == NULL) {
        return ERIAK_UNINITIALIZED;
    }
    if (resolver == NULL) {
        resolver = evutil_getaddrinfo;
    }

    riak_strlcpy(cfg->hostname, hostname, sizeof(cfg->hostname));
    riak_strlcpy(cfg->portnum, portnum, sizeof(cfg->hostname));

    riak_error err = riak_resolve_address(cfg, resolver, hostname, portnum, &(cfg->addrinfo));
    if (err) {
        return ERIAK_DNS_RESOLUTION;
    }
    return ERIAK_OK;
}

riak_error
riak_config_add_logging(riak_config *cfg,
                         const char *logging_category) {
    if (cfg == NULL) {
        return ERIAK_UNINITIALIZED;
    }
    if (logging_category == NULL) {
        riak_strlcpy(cfg->logging_category, RIAK_LOGGING_DEFAULT_CATEGORY, sizeof(cfg->logging_category));
    } else {
        riak_strlcpy(cfg->logging_category, logging_category, sizeof(cfg->logging_category));
    }
    // Since we will likely only have one config, set up non-thread-safe logging here
    // TODO: Configurable zlog.conf file
    int result = zlog_init("zlog.conf");
    if (result != 0) {
        fprintf(stderr, "Could not initialize logging\n");
        return ERIAK_LOGGING;
    }
    return ERIAK_OK;
}


riak_connection_base*
riak_config_get_base(riak_config *cfg) {
    return cfg->base;
}

void
riak_config_free(riak_config **config) {
    riak_config *cfg = *config;
    riak_free_fn freer = cfg->free_fn;
    if (cfg->base != NULL) event_base_free(cfg->base);
    if (cfg->addrinfo != NULL) evutil_freeaddrinfo(cfg->addrinfo);
    (freer)(cfg);
    *config = NULL;

    // Since we will only clean up one config, let's shut down non-threadsafe logging here, too
    zlog_fini();
}
