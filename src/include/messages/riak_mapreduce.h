/*********************************************************************
 *
 * riak_mapreduce.h: Riak C Client Map Reduce Message
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

#ifndef _RIAK_MAPREDUCE_H
#define _RIAK_MAPREDUCE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _riak_mapreduce_response riak_mapreduce_response;
typedef struct _riak_mapreduce_message riak_mapreduce_message;
typedef void (*riak_mapreduce_response_callback)(riak_mapreduce_response *response, void *ptr);

/**
 * @brief Free Map/Reduce response
 * @param cfg Riak Configuration
 * @param resp Map/Reduce response
 */
void
riak_mapreduce_response_free(riak_config              *cfg,
                             riak_mapreduce_response **resp);

/**
 * @brief Print out a Map/Reduce Response
 * @param state Riak Print State
 * @param response the Map/Reduce response to print
 * @returns Number of bytes written
 */
riak_int32_t
riak_mapreduce_response_print(riak_print_state        *state,
                              riak_mapreduce_response *response);

/**
 * @brief Determine the number of messages returned in the response
 * @prarm response The Map/Reduce response
 * @returns Number of messages included in the response
 */
riak_int32_t
riak_mapreduce_get_n_messages(riak_mapreduce_response *response);

/**
 * @brief Access the array of received map/reduce messages
 * @param response Riak Map/Reduce response
 * @returns Array of pointers to map/reduce messages
 */
riak_mapreduce_message**
riak_mapreduce_get_messages(riak_mapreduce_response *response);

/**
 * @brief Determine if a map/reduce message has a populated `phase` field
 * @param msg Riak Map/Reduce message
 * @returns True if a map/reduce message has a populated `phase` field
 */
riak_boolean_t
riak_mapreduce_message_get_has_phase(riak_mapreduce_message *msg);

/**
 * @brief Access the phase of a map/reduce message
 * @param response Riak Map/Reduce response
 * @returns The phase of a map/reduce message
 */
riak_uint32_t
riak_mapreduce_message_get_phase(riak_mapreduce_message *msg);

/**
 * @brief Determine if a map/reduce message has a populated body
 * @param msg Riak Map/Reduce message
 * @returns True if a map/reduce message has a populated body
 */
riak_boolean_t
riak_mapreduce_message_get_has_response(riak_mapreduce_message *msg);

/**
 * @brief Access the body of a map/reduce message
 * @param response Riak Map/Reduce response
 * @returns The body of a map/reduce message
 */
riak_binary*
riak_mapreduce_message_get_response(riak_mapreduce_message *msg);

/**
 * @brief Determine if a map/reduce message has a populated `done` field
 * @param msg Riak Map/Reduce message
 * @returns True if a map/reduce message has a populated `done` field
 */
riak_boolean_t
riak_mapreduce_message_get_has_done(riak_mapreduce_message *msg);

/**
 * @brief Determine if a map/reduce message is the last in a sequence
 * @param msg Riak Map/Reduce message
 * @returns True if a map/reduce message is the last in a sequence
 */
riak_boolean_t
riak_mapreduce_message_get_done(riak_mapreduce_message *msg);

#ifdef __cplusplus
}
#endif

#endif // _RIAK_MAPREDUCE_H
