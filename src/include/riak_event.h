/*********************************************************************
 *
 * riak_event.h: Management of the Riak event
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

#ifndef RIAK_EVENT_H_
#define RIAK_EVENT_H_

typedef struct _riak_event riak_event;

// Generic placeholder for message-specific callbacks
typedef void (*riak_response_callback)(void *response, void *ptr);

/**
 * @brief Construct a Riak event
 * @param ctx Riak context for memory allocation
 * @param rev Riak Event
 * @param response_cb Reaponse callback function (user-supplied)
 * @param cb_data Pointer passed to `response_cb` when it is called
 * @returns Error code
 */
riak_error
riak_event_new(riak_context          *ctx,
               riak_event             **rev,
               riak_response_callback response_cb,
               riak_response_callback error_cb,
               void                  *cb_data);

/**
 * @brief Set the event's callback data
 * @param rev Riak Event
 * @param cb_data Pointer to data used in user's callback
 */
void
riak_event_set_cb_data(riak_event         *rev,
                       void               *cb_data);

/**
 * @brief Set the event's response callback
 * @param rev Riak Event
 * @param cb Function pointer to response callback
 */
void
riak_event_set_response_cb(riak_event             *rev,
                           riak_response_callback  cb);

/**
 * @brief Set the event's error callback
 * @param rev Riak Event
 * @param cb Function pointer to error callback
 */
void
riak_event_set_error_cb(riak_event             *rev,
                        riak_response_callback  cb);

/**
 * @brief Cleanup memory used by a Riak Event
 * @param re Riak Event
 */
void
riak_event_free(riak_event** re);

/**
 * @brief Fire up an event loop
 * @param ctx Riak Context
 */
void
riak_event_loop(riak_context *ctx);

/**
 * @brief Return the underlying socket file descriptor
 * @param rev Riak Event
 * @returns File descriptor
 */
riak_socket_t
riak_event_get_fd(riak_event *rev);

/**
 * @brief Return the Riak Context
 * @param rev Riak Event
 * @returns The corresponding Riak Context
 */
riak_context*
riak_event_get_context(riak_event *rev);

#endif
