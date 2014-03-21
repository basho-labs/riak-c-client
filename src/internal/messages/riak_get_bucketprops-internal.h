/*********************************************************************
 *
 * riak_get_bucketprops-internal.h: Riak C Client Get Bucket Properties Message
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

// Based on RpbGetBucketReq
struct _riak_get_bucketprops_request
{
    riak_binary   *bucket;
    riak_boolean_t has_type;
    riak_binary   *type;
};

// Based on RpbGetBucketResp
struct _riak_get_bucketprops_response
{
    riak_bucketprops *props;

    RpbGetBucketResp *_internal;
};

/**
 * @brief Create a request to fetch bucket properies
 * @param rop Riak Operation
 * @param bucket Name of Riak bucket
 * @param req Returned bucket properies request
 * @return Error if out of memory
 */
riak_error
riak_get_bucketprops_request_encode(riak_operation   *rop,
                                    riak_binary      *bucket_type_bin,                                    
                                    riak_binary      *bucket_bin,
                                    riak_pb_message **req);

/**
 * @brief Translate PBC get_bucketprops response into Riak structure
 * @param rop Riak Operation
 * @param pbresp PBC response message
 * @param resp Returned Riak response structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_get_bucketprops_response_decode(riak_operation                 *rop,
                                     riak_pb_message                *pbresp,
                                     riak_get_bucketprops_response **resp,
                                     riak_boolean_t                 *done);
