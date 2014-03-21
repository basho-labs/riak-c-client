/*********************************************************************
 *
 * riak_2i-internal.h: Riak C Client Index Message
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


//    riak_binary bucket;
//    riak_binary index;
// Based on RpbIndexReq
struct _riak_2i_options
{
    RpbIndexReq__IndexQueryType qtype;
    riak_boolean_t has_key;
    riak_binary   *key;
    riak_boolean_t has_range_min;
    riak_binary   *range_min;
    riak_boolean_t has_range_max;
    riak_binary   *range_max;
    riak_boolean_t has_return_terms;
    riak_boolean_t return_terms;
    riak_boolean_t has_stream;
    riak_boolean_t stream;
    riak_boolean_t has_max_results;
    riak_uint32_t  max_results;
    riak_boolean_t has_continuation;
    riak_binary   *continuation;
    riak_boolean_t has_timeout;
    riak_uint32_t  timeout;
    riak_boolean_t has_type;
    riak_binary   *type;
    riak_boolean_t has_term_regex;
    riak_binary   *term_regex;
    riak_boolean_t has_pagination_sort;
    riak_boolean_t pagination_sort;
};

// Based on RpbIndexResp
struct _riak_2i_response
{
    riak_uint32_t  n_keys;
    riak_binary  **keys;
    riak_uint32_t  n_results;
    riak_pair    **results;
    riak_boolean_t has_continuation;
    riak_binary   *continuation;
    riak_boolean_t has_done;
    riak_boolean_t done;

    riak_uint32_t  _n_responses;
    RpbIndexResp **_internal;
};

riak_error
riak_2i_request_encode(riak_operation      *rop,
                       riak_binary         *bucket_type,
                       riak_binary         *bucket,
                       riak_binary         *index,
                       riak_2i_options *index_options,
                       riak_pb_message    **req);

riak_error
riak_2i_response_decode(riak_operation       *rop,
                        riak_pb_message       *pbresp,
                        riak_2i_response **resp,
                        riak_boolean_t        *done);

