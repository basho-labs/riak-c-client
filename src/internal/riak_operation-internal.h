/*********************************************************************
 *
 * riak_operation-internal.h: A single Riak Operation
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

#ifndef _RIAK_OPERATION_INTERNAL_H
#define _RIAK_OPERATION_INTERNAL_H

typedef riak_error (*riak_response_decoder)(struct _riak_operation  *rop,
                                            struct _riak_pb_message *pbresp,
                                            void                   **response,
                                            riak_boolean_t          *done);

// Essentially the state of the current event
struct _riak_operation {
    riak_connection         *connection;
    riak_response_decoder    decoder;
    riak_response_callback   response_cb;
    riak_response_callback   error_cb;
    void                    *cb_data;

    // Current message being decoded
    riak_uint32_t            position;
    riak_uint32_t            msglen;
    riak_uint8_t            *msgbuf;
    riak_boolean_t           msglen_complete;

    // Results of message translation
    struct _riak_pb_message *pb_request;
    struct _riak_pb_message *pb_response;

    riak_server_error       *error;

    void                    *response;

    // Cache of request data
    struct {
        riak_binary *bucket;
        riak_binary *key;
    } request;
};

/**
 * @brief Set the event's message decoding function
 * @param rop Riak Operation
 * @param decoder Function pointer to message translator
 */
void
riak_operation_set_response_decoder(riak_operation       *rop,
                                    riak_response_decoder decoder);


#endif //_RIAK_OPERATION_INTERNAL_H
