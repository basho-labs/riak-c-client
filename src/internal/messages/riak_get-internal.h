/*********************************************************************
 *
 * riak_get-internal.h: Riak C Client Get Message
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

// Based on RpbGetResp
struct _riak_get_response {
    riak_boolean_t has_vclock;
    riak_binary   *vclock;
    riak_boolean_t has_unmodified;
    riak_boolean_t unmodified;
    riak_boolean_t deleted;
    riak_int32_t   n_content;
    riak_object  **content; // Array of pointers to allow expansion

    RpbGetResp    *_internal;
};

// Based on RpbGetReq
struct _riak_get_options {
    riak_boolean_t has_r;
    riak_uint32_t  r;
    riak_boolean_t has_pr;
    riak_uint32_t  pr;
    riak_boolean_t has_basic_quorum;
    riak_boolean_t basic_quorum;
    riak_boolean_t has_notfound_ok;
    riak_boolean_t notfound_ok;
    riak_boolean_t has_if_modified;
    riak_binary   *if_modified;
    riak_boolean_t has_head;
    riak_boolean_t head;
    riak_boolean_t has_deletedvclock;
    riak_boolean_t deletedvclock;
    riak_boolean_t has_timeout;
    riak_uint32_t  timeout;
    riak_boolean_t has_sloppy_quorum;
    riak_boolean_t sloppy_quorum;
    riak_boolean_t has_n_val;
    riak_uint32_t  n_val;
};

/**
 * @brief Create a get/fetch Request
 * @param rop Riak Operation
 * @param bucket Name of Riak bucket
 * @param key Name of Riak key
 * @param options Get request parameters
 * @param req Returned PBC request
 * @return Error if out of memory
 */
riak_error
riak_get_request_encode(riak_operation   *rop,
                        riak_binary      *bucket_type,
                        riak_binary      *bucket,
                        riak_binary      *key,
                        riak_get_options *options,
                        riak_pb_message **req);

/**
 * @brief Translate PBC message to Riak message
 * @param rop Riak Operation
 * @param pbresp Protocol Buffer message
 * @param done Returned flag set to true if finished streaming
 * @param resp Returned Get message
 * @return Error if out of memory
 */
riak_error
riak_get_response_decode(riak_operation     *rop,
                         riak_pb_message    *pbresp,
                         riak_get_response **resp,
                         riak_boolean_t     *done);
