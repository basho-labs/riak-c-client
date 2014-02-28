/*********************************************************************
 *
 * riak_counter-internal.h: Riak C Client 1.4 Counters
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

struct _riak_counter_get_response {
    RpbCounterGetResp **_internal; // Array for many responses
};


struct _riak_counter_update_response {
    RpbCounterUpdateResp **_internal; // Array for many responses
};

riak_error
riak_counter_update_request_encode(riak_operation   *rop,
                                   riak_binary  *bucket,
                                   riak_uint32_t timeout,
                                   riak_pb_message **req);

riak_error
riak_counter_update_response_decode(riak_operation                *rop,
                                    riak_pb_message               *pbresp,
                                    riak_counter_update_response  **resp,
                                    riak_boolean_t                *done);

riak_error
riak_counter_get_request_encode(riak_operation   *rop,
                                riak_binary  *bucket,
                                riak_uint32_t timeout,
                                riak_pb_message **req);

riak_error
riak_counter_get_response_decode(riak_operation                *rop,
                                 riak_pb_message               *pbresp,
                                 riak_counter_get_response     **resp,
                                 riak_boolean_t                *done);

