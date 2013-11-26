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

#ifndef _RIAK_CONNECTION_H
#define _RIAK_CONNECTION_H

typedef struct _riak_connection riak_connection;

/**
 * @brief Construct a Riak event
 * @param cfg Riak config for memory allocation
 * @param cxn Riak Connection (out)
 * @param hostname Name of Riak server
 * @param portnum Riak PBC port number
 * @param resolver IP Address resolving function (NULL for default)
 * @returns Error code
 */
riak_error
riak_connection_new(riak_config       *cfg,
                    riak_connection  **cxn,
                    const char        *hostname,
                    const char        *portnum,
                    riak_addr_resolver resolver);
/**
 * @brief Cleanup memory used by a Riak Connection
 * @param re Riak Connection
 */
void
riak_connection_free(riak_connection** re);

riak_socket_t
riak_connection_get_fd(riak_connection *cxn);

riak_config*
riak_connection_get_config(riak_connection *cxn);

#endif // _RIAK_CONNECTION_H
