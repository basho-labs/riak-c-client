/*********************************************************************
 *
 * riak_listkeys-internal.h: Riak C Client List Key Message
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

// Based on RpbListKeysResp
struct _riak_listkeys_response {
    riak_uint32_t     n_keys;
    riak_binary     **keys; // Array of pointers to allow growth
    riak_boolean_t    done;

    riak_uint32_t     n_responses;
    RpbListKeysResp **_internal; // Array for many responses
};

/**
 * @brief Create a request to find all keys in a bucket
 * @param rop Riak Operation
 * @param bucket Name of Riak bucket
 * @param timeout How long to wait for a response
 * @param req Returned listbuckets request
 * @return Error if out of memory
 */
riak_error
riak_listkeys_request_encode(riak_operation   *rop,
                                 riak_binary  *bucket,
                                 riak_uint32_t timeout,
                                 riak_pb_message **req);

/**
 * @brief Translate PBC listkeys response into Riak structure
 * @param rop Riak Operation
 * @param pbresp PBC response message
 * @param resp Returned Riak response structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_listkeys_response_decode(riak_operation          *rop,
                              riak_pb_message         *pbresp,
                              riak_listkeys_response **resp,
                              riak_boolean_t          *done);

