/*********************************************************************
 *
 * riak_types.h: Riak C Client Types
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

#ifndef _RIAK_TYPES_H
#define _RIAK_TYPES_H

// protobuf-c maps boolean to an int
typedef int                 riak_boolean_t;
typedef uint8_t             riak_uint8_t;
typedef int8_t              riak_int8_t;
typedef uint16_t            riak_uint16_t;
typedef int16_t             riak_int16_t;
typedef uint32_t            riak_uint32_t;
typedef int32_t             riak_int32_t;
typedef uint64_t            riak_uint64_t;
typedef int64_t             riak_int64_t;
typedef size_t              riak_size_t;

// network types
typedef struct addrinfo     riak_addrinfo;
typedef int                 riak_socket_t;
typedef int (*riak_addr_resolver)(const char *nodename,
                                  const char *servname,
                                  const riak_addrinfo *hints_in,
                                  riak_addrinfo **res);

#define RIAK_FALSE          0
#define RIAK_TRUE           1

#endif // _RIAK_TYPES_H
