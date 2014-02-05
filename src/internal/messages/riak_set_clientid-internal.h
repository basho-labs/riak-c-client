/*********************************************************************
 *
 * riak_set_clientid-internal.h: Riak C Client Set Client Identifier Message
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

// Based on RpbSetClientIdReq
struct _riak_set_clientid_request
{
    riak_binary *client_id;
};

// Placeholder
struct _riak_set_clientid_response
{
// Empty
};

/**
 * @brief Build a client id request
 * @param rop Riak Operation
 * @param clientid Client id for current operation
 * @param req Created PB message
 * @return Error if out of memory
 */
riak_error
riak_set_clientid_request_encode(riak_operation   *rop,
                                 riak_binary      *clientid,
                                 riak_pb_message **req);

/**
 * @brief Translate PBC message to Riak message
 * @param rop Riak Operation
 * @param pbresp Protocol Buffer message
 * @param done Returned flag set to true if finished streaming
 * @param resp Returned set message
 * @return Error if out of memory
 */
riak_error
riak_set_clientid_response_decode(riak_operation              *rop,
                                  riak_pb_message             *pbresp,
                                  riak_set_clientid_response **resp,
                                  riak_boolean_t              *done);
