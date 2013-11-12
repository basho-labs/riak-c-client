/*********************************************************************
 *
 * riak_config.h: Management of the Riak connection config
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

#ifndef RIAK_CONTEXT_H_
#define RIAK_CONTEXT_H_

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
 * @param config Spanking new `riak_config` struct
 * @param alloc Memory allocator function (optional)
 * @param realloc Memory re-allocation function (optional)
 * @param freeme Memory releasing function (optional)
 * @param pb_alloc Memory allocator function for protocol buffers (optional)
 * @param pb_free Memory releasing function for protocol buffer (optional)
 * @return Error code
 */
riak_error
riak_config_new(riak_config    **config,
                 riak_alloc_fn     alloc,
                 riak_realloc_fn   realloc,
                 riak_free_fn      freeme,
                 riak_pb_alloc_fn  pb_alloc,
                 riak_pb_free_fn   pb_free);
// By default use system's built-in memory management utilities (malloc/free)
#define riak_config_new_default(C) riak_config_new((C),NULL,NULL,NULL,NULL,NULL)

/**
 * @brief Construct a Riak Configuration
 * @param config Existing Riak Configuration
 * @param resolver IP Address resolving function
 * @param hostname Name of Riak server
 * @param portnum Riak PBC port number
 * @return Error code
 */
riak_error
riak_config_add_connection(riak_config      *config,
                            riak_addr_resolver resolver,
                            const char        *hostname,
                            const char        *portnum);
// By default use the libevent resolver
#define riak_config_add_default_connection(C,H,P) riak_config_add_connection((C),NULL,(H),(P))

/**
 * @brief Construct a Riak Configuration
 * @param config Spanking new `riak_config` struct
 * @param logging_category logging prefix (optional)
 * @return Error code
 */
riak_error
riak_config_add_logging(riak_config *config,
                         const char   *logging_category);


/**
 * @brief Gets the underlying event base
 * @param cfg Riak Configuration
 * @return Return event base
 */
riak_connection_base*
riak_config_get_base(riak_config *cfg);

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
riak_free_internal(riak_config *cfg,
                   void        **p);
#define riak_free(C,P)  riak_free_internal((C),(void**)(P))

#endif /* RIAK_CONTEXT_H_ */
