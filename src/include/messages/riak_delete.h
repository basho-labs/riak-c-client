/*********************************************************************
 *
 * riak_messages.h: Riak C Client Delete Message
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

#ifndef _RIAK_DELETE_MESSAGE_H
#define _RIAK_DELETE_MESSAGE_H

typedef struct _riak_delete_options riak_delete_options;
typedef struct _riak_delete_response riak_delete_response;
typedef void (*riak_delete_response_callback)(riak_delete_response *response, void *ptr);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp Delete PBC Response
 */
void
riak_delete_response_free(riak_config           *cfg,
                          riak_delete_response **resp);

/**
 * @brief Construct a new Riak Delete Options object
 * @param cfg Riak Configuration
 * @return Riak Object
 */
riak_delete_options*
riak_delete_options_new(riak_config *cfg);

/**
 * @brief Release claimed memory used by Riak Delete Options
 * @param cfg Riak Configuration
 * @param obj Riak Object to be freed
 */
void
riak_delete_options_free(riak_config *cfg,
                         riak_delete_options **opt);

/**
 * @brief Print contents of a Riak Delete Options to a string
 * @param obj Object to print
 * @param target Location to write formatted string
 * @param len Number of bytes to write
 * @return Number of bytes written
 */
int
riak_delete_options_print(riak_delete_options *opt,
                          char                *target,
                          riak_int32_t         len);

/**
 * @brief Set the Vector Clock on a delete operation
 * @param cfg Riak Configuration
 * @param opt Delete options
 * @param value Value of Vector Clock
 */
void
riak_delete_options_set_vclock(riak_config         *cfg,
                               riak_delete_options *opt,
                               riak_binary         *value);
/**
 * @brief Set the Write Quorum on a delete operation
 * @param cfg Riak Configuration
 * @param opt Delete options
 * @param value Value of Write Quorum
 */
void
riak_delete_options_set_w(riak_delete_options *opt,
                          riak_uint32_t        value);
/**
 * @brief Set the Durable Write Quorum on a delete operation
 * @param cfg Riak Configuration
 * @param opt Delete options
 * @param value Value of Durable Write Quorum
 */
void
riak_delete_options_set_dw(riak_delete_options *opt,
                           riak_uint32_t        value);
/**
 * @brief Set the Primary Write Quorum on a delete operation
 * @param cfg Riak Configuration
 * @param opt Delete options
 * @param value Value of Primary Write Quorum
 */
void
riak_delete_options_set_pw(riak_delete_options *opt,
                           riak_uint32_t        value);
/**
 * @brief Set the Time out on a delete operation
 * @param cfg Riak Configuration
 * @param opt Delete options
 * @param value Value of Time Out (in ms)
 */
void
riak_delete_options_set_timeout(riak_delete_options *opt,
                                riak_uint32_t        value);
/**
 * @brief Allow a Sloppy Quorum on a delete operation
 * @param cfg Riak Configuration
 * @param opt Delete options
 * @param value True or False
 */
void
riak_delete_options_set_sloppy_quorum(riak_delete_options *opt,
                                      riak_boolean_t       value);
/**
 * @brief Set the N Value on a delete operation
 * @param cfg Riak Configuration
 * @param opt Delete options
 * @param value N Value
 */
void
riak_delete_options_set_n_val(riak_delete_options *opt,
                              riak_uint32_t        value);
#endif
