/*********************************************************************
 *
 * riak_set_bucketprops-internal.h: Riak C Client Set Bucket Properties Message
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

// Based on RpbResetBucketReq
struct _riak_reset_bucketprops_request
{
    riak_binary   *bucket;
    riak_boolean_t has_type;
    riak_binary   *type;
};

// Placeholder
struct _riak_reset_bucketprops_response
{
// Empty
};

/**
 * @brief Create a request to reset bucket properties
 * @param rop Riak Operation
 * @param bucket Name of Riak bucket
 * @param req Returned bucket properties request
 * @return Error if out of memory
 */
riak_error
riak_reset_bucketprops_request_encode(riak_operation   *rop,
                                      riak_binary      *bucket,
                                      riak_pb_message **req);

/**
 * @brief Translate PBC reset_bucketprops response into Riak structure
 * @param rop Riak Operation
 * @param pbresp PBC response message
 * @param resp Returned Riak response structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_reset_bucketprops_response_decode(riak_operation                   *rop,
                                       riak_pb_message                  *pbresp,
                                       riak_reset_bucketprops_response **resp,
                                       riak_boolean_t                   *done);
