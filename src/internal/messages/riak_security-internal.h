/*********************************************************************
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

struct _riak_auth_response {
    riak_boolean_t success;
};



// TODO: fix naming of riak_encode/riak_decode
riak_error
riak_auth_request_encode(riak_operation   *rop,
                         riak_binary       *user,
                         riak_binary       *password,
                         riak_pb_message  **req);

riak_error
riak_auth_response_decode(riak_operation      *rop,
                          riak_pb_message     *pbresp,
                          riak_auth_response **resp,
                          riak_boolean_t      *done);

