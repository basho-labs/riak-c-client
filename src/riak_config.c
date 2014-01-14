/*********************************************************************
 *
 * riak_config.c: Management of the Riak connection config
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
#include "riak_network.h"
#include "riak_config-internal.h"

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
    cfg->log_data        = NULL;
    cfg->log_fn          = NULL;
    cfg->log_init_fn     = NULL;
    cfg->log_cleanup_fn  = NULL;

    *config = cfg;
    return ERIAK_OK;
}

riak_error
riak_config_set_logging(riak_config        *cfg,
                        void*               log_data,
                        riak_log_fn         log_fn,
                        riak_log_init_fn    log_init,
                        riak_log_cleanup_fn log_cleanup) {
    if (cfg == NULL) {
        return ERIAK_UNINITIALIZED;
    }

    cfg->log_data = log_data;
    cfg->log_fn = log_fn;
    cfg->log_init_fn = log_init;
    cfg->log_cleanup_fn = log_cleanup;

    if (cfg->log_init_fn) {
        int result = (cfg->log_init_fn)(cfg->log_data);
        if (result != 0) {
            return ERIAK_LOGGING;
        }
    }
    return ERIAK_OK;
}

void*
riak_config_allocate(riak_config *cfg,
                     riak_size_t  bytes) {
    void *memory = NULL;
    if (cfg && cfg->malloc_fn) {
        memory = (cfg->malloc_fn)(bytes);
    }
    return memory;
}

void*
riak_config_clean_allocate(riak_config *cfg,
                           riak_size_t  bytes) {
    void *memory = NULL;
    if (cfg && cfg->malloc_fn) {
        memory = (cfg->malloc_fn)(bytes);
        if (memory) {
            memset(memory, '\0', bytes);
        }
    }
    return memory;
}


void
riak_config_free(riak_config **config) {
    riak_config *cfg = *config;
    riak_free_fn freer = cfg->free_fn;

    // Since we will only clean up one config, let's shut down non-threadsafe logging here, too
    if (cfg->log_cleanup_fn) {
        (cfg->log_cleanup_fn)(cfg->log_data);
    }
    (freer)(cfg);
    *config = NULL;
}
