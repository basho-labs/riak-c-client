/*********************************************************************
 *
 * riak_delete-internal.h: Riak C Client Delete Message
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

// Based on RpbDelReq
struct _riak_delete_options
{
    riak_boolean_t has_vclock;
    riak_binary   *vclock;
    riak_boolean_t has_w;
    riak_uint32_t  w;
    riak_boolean_t has_dw;
    riak_uint32_t  dw;
    riak_boolean_t has_pw;
    riak_uint32_t  pw;
    riak_boolean_t has_timeout;
    riak_uint32_t  timeout;
    riak_boolean_t has_sloppy_quorum;
    riak_boolean_t sloppy_quorum;
    riak_boolean_t has_n_val;
    riak_uint32_t  n_val;
};

struct _riak_delete_response {
// Nothing to see here
};

/**
 * @brief Create a deletion request
 * @param rop Riak Operation
 * @param bucket Name of Riak bucket
 * @param key Name of Riak key
 * @param options Delete request parameters
 * @param req Returned PBC request
 * @return Error if out of memory
 */
riak_error
riak_delete_request_encode(riak_operation      *rop,
                           riak_binary         *bucket_type,
                           riak_binary         *bucket,
                           riak_binary         *key,
                           riak_delete_options *options,
                           riak_pb_message    **req);

/**
 * @brief Translate PBC delete message to a Riak response
 * @param rop Riak Operation
 * @param pbresp Protocol Buffer message
 * @param resp Returned Delete message
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_delete_response_decode(riak_operation        *rop,
                            riak_pb_message       *pbresp,
                            riak_delete_response **resp,
                            riak_boolean_t        *done);

