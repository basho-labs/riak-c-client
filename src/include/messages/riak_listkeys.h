/*********************************************************************
 *
 * riak_listkeys.h: Riak C Client List Keys Message
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

#ifndef _RIAK_LISTKEYS_MESSAGE_H
#define _RIAK_LISTKEYS_MESSAGE_H

typedef struct _riak_listkeys_response riak_listkeys_response;
typedef void (*riak_listkeys_response_callback)(riak_listkeys_response *response, void *ptr);

/**
 * @brief Print a summary of a `riak_listkeys_response`
 * @param response Result from a Listkeys request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_listkeys_response_print(riak_listkeys_response *response,
                             char                   *target,
                             riak_size_t             len);

/**
 * @brief Free listkeys response
 * @param cfg Riak Configuration
 * @param resp List keys message to be cleaned up
 */
void
riak_listkeys_response_free(riak_config             *cfg,
                            riak_listkeys_response **resp);

/**
 * @brief Access the number of keys in a bucket
 * @param response List Keys response message
 * @returns The number of keys in a bucket
 */
riak_uint32_t
riak_listkeys_get_n_keys(riak_listkeys_response *response);

/**
 * @brief Access the list of keys in a bucket
 * @param response List Keys response message
 * @returns An array of keys in a bucket
 */
riak_binary**
riak_listkeys_get_keys(riak_listkeys_response *response);

#endif
