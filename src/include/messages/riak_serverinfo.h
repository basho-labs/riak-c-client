/*********************************************************************
 *
 * riak_serverinfo.h: Riak C Client Server Information Message
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

#ifndef _RIAK_SERVERINFO_MESSAGE_H
#define _RIAK_SERVERINFO_MESSAGE_H

typedef struct _riak_serverinfo_response riak_serverinfo_response;
typedef void (*riak_serverinfo_response_callback)(riak_serverinfo_response *response, void *ptr);

/**
 * @brief Print a summary of a `riak_serverinfo_response`
 * @param response Result from a Server Info request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_serverinfo_response_print(riak_serverinfo_response *response,
                               char                     *target,
                               riak_size_t               len);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp Server Info PBC Response
 */
void
riak_serverinfo_response_free(riak_config               *cfg,
                              riak_serverinfo_response **resp);

/**
 * @brief Determine if a node is present in the response
 * @param resp Riak Server Info Response
 * @returns True if node is returned
 */
riak_boolean_t
riak_serverinfo_get_has_node(riak_serverinfo_response *resp);

/**
 * @brief Return the server node name
 * @param resp Riak Server Info Response
 * @returns Node name sting (e.g. dev1@127.0.0.1)
 */
riak_binary*
riak_serverinfo_get_node(riak_serverinfo_response *resp);

/**
 * @brief Determine if a server version is present in the response
 * @param resp Riak Server Info Response
 * @returns True if server version is returned
 */
riak_boolean_t
riak_serverinfo_get_has_server_version(riak_serverinfo_response *resp);

/**
 * @brief Get the
 * @param resp Riak Server Info Response
 * @returns Server version string (e.g. 2.0.0pre4)
 */
riak_binary*
riak_serverinfo_get_server_version(riak_serverinfo_response *resp);

#endif
