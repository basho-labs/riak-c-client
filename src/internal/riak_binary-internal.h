/*********************************************************************
 *
 * riak_binary-internal.h: Riak C Client Binary Object Utilities
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

#include "riak_kv.pb-c.h"
#include "riak.pb-c.h"
#include "riak_search.pb-c.h"

#ifndef _RIAK_BINARY_INTERNAL_H
#define _RIAK_BINARY_INTERNAL_H

// Based off of ProtobufCBinaryData
struct _riak_binary {
    riak_size_t   len;
    riak_uint8_t *data;
};

/**
 * @brief Allocate a new riak_binary and populate from data pointer
 * @param cfg Riak Configuration
 * @param bin Existing `ProtobufCBinaryData` to be shallow copied
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_copy_from_pb(riak_config         *cfg,
                         ProtobufCBinaryData *bin);

/**
 * @brief Create a shallow copy of `riak_binary` for use in PB
 * @param to Existing PBC struct
 * @param from Existing `riak_binary`
 */
void
riak_binary_copy_to_pb(ProtobufCBinaryData *to,
                       riak_binary         *from);

#define _RIAK_DUMP_BINARY_HEX(B) { char buffer[10240]; \
    char *dest = buffer; \
    riak_int32_t len   = sizeof(buffer); \
    riak_int32_t total = 0; \
    riak_print_raw_hex_array((B)->data, (B)->len, &dest, &len, &total); \
    printf("%s\n", buffer); }

#endif // _RIAK_BINARY_H
