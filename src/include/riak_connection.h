/*********************************************************************
 *
 * riak_connection.h: Management of the Riak event
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

typedef struct _riak_connection riak_connection;

// Generic placeholder for message-specific callbacks
typedef void (*riak_response_callback)(void *response, void *ptr);

/**
 * @brief Construct a Riak event
 * @param cfg Riak config for memory allocation
 * @param cxn Riak Connection
 * @param response_cb Reaponse callback function (user-supplied)
 * @param cb_data Pointer passed to `response_cb` when it is called
 * @returns Error code
 */
riak_error
riak_connection_new(riak_config          *cfg,
               riak_connection             **cxn,
               riak_response_callback response_cb,
               riak_response_callback error_cb,
               void                  *cb_data);

/**
 * @brief Set the event's callback data
 * @param cxn Riak Connection
 * @param cb_data Pointer to data used in user's callback
 */
void
riak_connection_set_cb_data(riak_connection         *cxn,
                       void               *cb_data);

/**
 * @brief Set the event's response callback
 * @param cxn Riak Connection
 * @param cb Function pointer to response callback
 */
void
riak_connection_set_response_cb(riak_connection             *cxn,
                           riak_response_callback  cb);

/**
 * @brief Set the event's error callback
 * @param cxn Riak Connection
 * @param cb Function pointer to error callback
 */
void
riak_connection_set_error_cb(riak_connection             *cxn,
                        riak_response_callback  cb);

/**
 * @brief Cleanup memory used by a Riak Connection
 * @param re Riak Connection
 */
void
riak_connection_free(riak_connection** re);

/**
 * @brief Fire up an event loop
 * @param cfg Riak Configuration
 */
void
riak_connection_loop(riak_config *cfg);

/**
 * @brief Return the underlying socket file descriptor
 * @param cxn Riak Connection
 * @returns File descriptor
 */
riak_socket_t
riak_connection_get_fd(riak_connection *cxn);

/**
 * @brief Return the Riak Configuration
 * @param cxn Riak Connection
 * @returns The corresponding Riak Configuration
 */
riak_config*
riak_connection_get_config(riak_connection *cxn);

/**
 * @brief Get the optional server error
 * @param cxn Riak Connection
 * @returns Error message sent from the server
 */
riak_server_error*
riak_connection_get_server_error(riak_connection *cxn);

#endif
