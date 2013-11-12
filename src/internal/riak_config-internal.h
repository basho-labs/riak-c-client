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

#ifndef RIAK_CONTEXT_INTERNAL_H_
#define RIAK_CONTEXT_INTERNAL_H_

// TODO: one day make this configurable?
// FYI, zlog does not support dots or hyphens in the category
#define RIAK_LOGGING_DEFAULT_CATEGORY   "riakc"
#define RIAK_LOGGING_MAX_LEN            256
#define RIAK_HOST_MAX_LEN               256

struct _riak_config {
    riak_alloc_fn       malloc_fn;
    riak_realloc_fn     realloc_fn;
    riak_free_fn        free_fn;
    ProtobufCAllocator *pb_allocator;
    char                logging_category[RIAK_LOGGING_MAX_LEN];
    riak_connection_base    *base; // Is this the right location? One per thread, so probably
    char                hostname[RIAK_HOST_MAX_LEN];
    char                portnum[RIAK_HOST_MAX_LEN]; // Keep as a string for debugging
    riak_addrinfo      *addrinfo;
};

#endif /* RIAK_CONTEXT_INTERNAL_H_ */
