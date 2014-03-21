/*********************************************************************
 *
 * riak_search-internal.h: Riak C Client Legacy Search Message
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

struct _riak_search_options {
    riak_boolean_t has_rows;
    riak_uint32_t  rows;
    riak_boolean_t has_start;
    riak_uint32_t  start;
    riak_boolean_t has_sort;
    riak_binary   *sort;
    riak_boolean_t has_filter;
    riak_binary   *filter;
    riak_boolean_t has_df;
    riak_binary   *df;
    riak_boolean_t has_op;
    riak_binary   *op;
    riak_size_t    n_fl;
    riak_binary  **fl;
    riak_boolean_t has_presort;
    riak_binary   *presort;
};

struct _riak_search_doc
{
    riak_size_t   n_fields;
    riak_pair   **fields;
};

struct _riak_search_response
{
    riak_size_t      n_docs;
    riak_search_doc *docs;
    riak_boolean_t   has_max_score;
    riak_float32_t   max_score;
    riak_boolean_t   has_num_found;
    riak_uint32_t    num_found;

    RpbSearchQueryResp *_internal;
};

riak_error
riak_search_request_encode(riak_operation      *rop,
                           riak_binary         *bucket_bin,
                           riak_binary         *index,
                           riak_search_options *search_options,
                           riak_pb_message    **req);

riak_error
riak_search_response_decode(riak_operation        *rop,
                            riak_pb_message       *pbresp,
                            riak_search_response **resp,
                            riak_boolean_t        *done);

