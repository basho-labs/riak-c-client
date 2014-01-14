/*********************************************************************
 *
 * riak_mapreduce-internal.h: Riak C Client Map Reduce Message
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


// Based on _RpbMapRedResp
struct _riak_mapreduce_message
{
    riak_boolean_t  has_phase;
    riak_uint32_t   phase;
    riak_boolean_t  has_response;
    riak_binary    *response;
    riak_boolean_t  has_done;
    riak_boolean_t  done;
};

struct _riak_mapreduce_response
{
    // Arrays for many responses
    riak_mapreduce_message **msg;
    riak_uint32_t                     n_responses;
    RpbMapRedResp**                   _internal;
};

riak_error
riak_mapreduce_request_encode(riak_operation   *rop,
                              riak_binary      *content_type,
                              riak_binary      *map_request,
                              riak_pb_message **req);

riak_error
riak_mapreduce_response_decode(riak_operation           *rop,
                               riak_pb_message          *pbresp,
                               riak_mapreduce_response **resp,
                               riak_boolean_t           *done);

