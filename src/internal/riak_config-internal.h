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

#ifndef _RIAK_CONFIG_INTERNAL_H
#define _RIAK_CONFIG_INTERNAL_H

struct _riak_config {
    riak_alloc_fn       malloc_fn;
    riak_realloc_fn     realloc_fn;
    riak_free_fn        free_fn;
    ProtobufCAllocator *pb_allocator;

    // LOGGING
    void*               log_data;
    riak_log_fn         log_fn;
    riak_log_init_fn    log_init_fn;
    riak_log_cleanup_fn log_cleanup_fn;
};

#endif // _RIAK_CONFIG_INTERNAL_H
