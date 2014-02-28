/*********************************************************************
 *
 * riak_config.h: Management of the Riak connection config
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

#ifndef _RIAK_CONFIG_H
#define _RIAK_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif
// per-thread
// do we need a *shared* config to complement?

typedef void *(*riak_alloc_fn)(size_t sz);
typedef void *(*riak_realloc_fn)(void *ptr, size_t size);
typedef void (*riak_free_fn)(void *ptr);
typedef void *(*riak_pb_alloc_fn)(void *allocator_data, size_t size);
typedef void (*riak_pb_free_fn)(void *allocator_data, void *pointer);

typedef struct _riak_config riak_config;

/**
 * @brief Construct a Riak Configuration
 * @param cfg Spanking new `riak_config` struct
 * @param alloc Memory allocator function (optional)
 * @param realloc Memory re-allocation function (optional)
 * @param freeme Memory releasing function (optional)
 * @param pb_alloc Memory allocator function for protocol buffers (optional)
 * @param pb_free Memory releasing function for protocol buffer (optional)
 * @return Error code
 */
riak_error
riak_config_new(riak_config     **cfg,
                riak_alloc_fn     alloc,
                riak_realloc_fn   realloc,
                riak_free_fn      freeme,
                riak_pb_alloc_fn  pb_alloc,
                riak_pb_free_fn   pb_free);
// By default use system's built-in memory management utilities (malloc/free)
#define riak_config_new_default(C) riak_config_new((C),NULL,NULL,NULL,NULL,NULL)

/**
 * @brief Add logging functions
 * @param config Riak Configuration
 * @param log_data Pointer passed to all logging functions
 * @param log_fn Main logging function
 * @param log_init Function called once at setup
 * @param log_cleanup Function called at program termination
 * @return Error code
 */
riak_error
riak_config_set_logging(riak_config        *cfg,
                        void*               log_data,
                        riak_log_fn         log_fn,
                        riak_log_init_fn    log_init,
                        riak_log_cleanup_fn log_cleanup);

/**
 * @brief Use the default allocator to claim some memory
 * @param cfg Riak Config
 * @param bytes Number of bytes to allocate
 * @returns Pointer to allocated memory (or NULL)
 */
void*
riak_config_allocate(riak_config *cfg,
                     riak_size_t  bytes);

/**
 * @brief Use the default allocator to claim some memory
 * @param cfg Riak Config
 * @param bytes Number of bytes to allocate
 * @returns Pointer to zeroed out memory (or NULL)
 */
void*
riak_config_clean_allocate(riak_config *cfg,
                           riak_size_t  bytes);

/**
 * @brief Reclaim memory used by a `riak_config`
 * @param cfg Configuration struct
 */
void
riak_config_free(riak_config **cfg);

/**
 * @brief Generic memory de-allocation function
 * @param cfg Riak Configuration
 * @param p Pointer to pointer to memory to free
 * @note p is NULLed after it is freed
 */
void
riak_free_internal(riak_config  *cfg,
                   void        **p);
#define riak_free(C,P)  riak_free_internal((C),(void**)(P))

#ifdef __cplusplus
}
#endif

#endif // _RIAK_CONFIG_H
