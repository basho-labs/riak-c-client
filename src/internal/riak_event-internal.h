/*********************************************************************
 *
 * riak_event-internal.h: Management of the Riak event
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

#ifndef RIAK_EVENT_INTERNAL_H_
#define RIAK_EVENT_INTERNAL_H_

typedef riak_error (*riak_response_decoder)(struct _riak_event      *rev,
                                            struct _riak_pb_message *pbresp,
                                            void                   **response,
                                            riak_boolean_t          *done);

// Essentially the state of the current event
struct _riak_event {
    riak_context            *context;
    riak_event_base         *base;
    riak_bufferevent        *bevent;
    riak_response_decoder    decoder;
    riak_response_callback   response_cb;
    riak_response_callback   error_cb;
    void                    *cb_data;
    riak_socket_t            fd;

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
};

/**
 * @brief Called by libevent when event posts
 * @param bev Riak Bufferevent (libevent)
 * @param ptr User-supplied pointer (Riak Event)
 */
void
riak_event_callback(riak_bufferevent *bev,
                    short             events,
                    void             *ptr);

/**
 * @brief Called by libevent when event is ready for writing
 * @param bev Riak Bufferevent (libevent)
 * @param ptr User-supplied pointer (Riak Event)
 */
void
riak_write_callback(riak_bufferevent *bev,
                    void             *ptr);

/**
 * @brief Called by libevent on a read event
 * @param bev Riak Bufferevent (libevent)
 * @param ptr User-supplied pointer (Riak Event)
 */
void
riak_read_result_callback(riak_bufferevent *bev,
                          void             *ptr);

/**
 * @brief Set the event's message decoding function
 * @param rev Riak Event
 * @param decoder Function pointer to message translator
 */
void
riak_event_set_response_decoder(riak_event           *rev,
                                riak_response_decoder decoder);


#endif
