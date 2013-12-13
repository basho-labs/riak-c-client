/*********************************************************************
 *
 * riak_listbuckets-internal.h: Riak C Client List Buckets Message
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

// Based on RpbListBucketsResp
struct _riak_listbuckets_response {
    riak_uint32_t        n_buckets;
    riak_binary        **buckets; // Array of pointers to allow growth
    riak_boolean_t       done;

    riak_uint32_t        n_responses;
    RpbListBucketsResp **_internal; // Array for many responses
};

/**
 * @brief Create a request to find all buckets
 * @param rop Riak Operation
 * @param req Returned listbuckets request
 * @return Error if out of memory
 */
riak_error
riak_listbuckets_request_encode(riak_operation   *rop,
                                riak_pb_message **req);

/**
 * @brief Translate PBC listbuckets response into Riak strucuture
 * @param rop Riak Operation
 * @param pbresp PBC response message
 * @param resp Returned Riak response structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_listbuckets_response_decode(riak_operation             *rop,
                                 riak_pb_message            *pbresp,
                                 riak_listbuckets_response **resp,
                                 riak_boolean_t             *done);

