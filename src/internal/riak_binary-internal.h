/*********************************************************************
 *
 * riak_binary-internal.h: Riak C Client Binary Object Utilities
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

#include "riak_kv.pb-c.h"
#include "riak.pb-c.h"

#ifndef RIAK_BINARY_INTERNAL_H_
#define RIAK_BINARY_INTERNAL_H_

// Based off of ProtobufCBinaryData
struct _riak_binary {
    riak_size_t   len;
    riak_uint8_t *data;
};

/**
 * @brief Allocate a new riak_binary and populate from data pointer
 * @param bin Existing `ProtobufCBinaryData` to be shallow copied
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_populate_from_pb(riak_config *cfg,
                             ProtobufCBinaryData  *bin);

void
riak_binary_to_pb_copy(ProtobufCBinaryData *to,
                       riak_binary         *from);
riak_error
riak_binary_to_pb_deep_copy(riak_config        *cfg,
                            ProtobufCBinaryData *to,
                            riak_binary         *from);
void
riak_binary_from_pb_copy(riak_binary        *to,
                         ProtobufCBinaryData *from);
riak_error
riak_binary_from_pb_deep_copy(riak_config        *cfg,
                              riak_binary         *to,
                              ProtobufCBinaryData *from);

/**
 * @brief Clean up deeply allocated PB binary
 * @param cfg Riak Configuration
 * @param b Protocol Buffer
 */
void
riak_binary_deep_free_pb(riak_config        *cfg,
                         ProtobufCBinaryData *b);

#endif /* RIAK_BINARY_H_ */
