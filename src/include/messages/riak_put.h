/*********************************************************************
 *
 * riak_put.h: Riak C Client Put Message
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

#ifndef _RIAK_PUT_MESSAGE_H
#define _RIAK_PUT_MESSAGE_H

typedef struct _riak_put_response riak_put_response;
typedef struct _riak_put_options riak_put_options;
typedef void (*riak_put_response_callback)(riak_put_response *response, void *ptr);

/**
 * @brief Print a summary of a `riak_put_response`
 * @param response Result from a Put request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 */
void
riak_put_response_print(riak_put_response *response,
                        char              *target,
                        riak_size_t        len);

/**
 * @brief Free put response
 * @param cfg Riak Configuration
 * @param resp Put message to be cleaned up
 */
void
riak_put_response_free(riak_config        *cfg,
                       riak_put_response **resp);

/**
 * @brief Construct a new Riak Put Options object
 * @param cfg Riak Configuration
 * @return Riak Object
 */
riak_put_options*
riak_put_options_new(riak_config *cfg);

/**
 * @brief Release claimed memory used by Riak Put Options
 * @param cfg Riak Configuration
 * @param obj Riak Object to be freed
 */
void
riak_put_options_free(riak_config       *cfg,
                      riak_put_options **opt);

/**
 * @brief Print contents of a Riak Put Options to a string
 * @param obj Object to print
 * @param target Location to write formatted string
 * @param len Number of bytes to write
 * @return Number of bytes written
 */
int
riak_put_options_print(riak_put_options *opt,
                       char             *target,
                       riak_int32_t      len);

/**
 * @brief Determine if Vector Clock is in a Put response
 * @param response Riak Put Response
 * @returns True if Vector Clock is returned
 */
riak_boolean_t
riak_put_get_has_vclock(riak_put_response *response);

/**
 * @brief Access the Vector Clock in a Put response
 * @param response Riak Put Response
 * @returns Vector Clock value
 */
riak_binary*
riak_put_get_vclock(riak_put_response *response);

/**
 * @brief Determine if Key is in a Put response
 * @param response Riak Put Response
 * @returns True if Key is returned
 */
riak_boolean_t
riak_put_get_has_key(riak_put_response *response);

/**
 * @brief Access the Key in a Put response
 * @param response Riak Put Response
 * @returns Key value
 */
riak_binary*
riak_put_get_key(riak_put_response *response);

/**
 * @brief Access the Number of Riak Objects in a Put response
 * @param response Riak Put Response
 * @returns Number of Riak Objects
 */
riak_int32_t
riak_put_get_n_content(riak_put_response *response);

/**
 * @brief Access an array of Riak Objects in a Put response
 * @param response Riak Put Response
 * @returns Array of Riak Objects (siblings)
 */
riak_object**
riak_put_get_content(riak_put_response *response);

/**
 * @brief Determine if Vector Clock has been set on Put request
 * @param opt Riak Put Option
 * @returns True if Vector Clock has been set
 */
riak_boolean_t
riak_put_options_get_has_vclock(riak_put_options *opt);

/**
 * @brief Access the Vector Clock on Put request
 * @param opt Riak Put Option
 * @returns Read Vector Clock value
 */
riak_binary*
riak_put_options_get_vclock(riak_put_options *opt);

/**
 * @brief Determine if Write Quorum has been set on Put request
 * @param opt Riak Put Option
 * @returns True if Write Quorum has been set
 */
riak_boolean_t
riak_put_options_get_has_w(riak_put_options *opt);

/**
 * @brief Access the Write Quorum on Put request
 * @param opt Riak Put Option
 * @returns Write Quorum value
 */
riak_uint32_t
riak_put_options_get_w(riak_put_options *opt);

/**
 * @brief Determine if Durable Write Quroum has been set on Put request
 * @param opt Riak Put Option
 * @returns True if Durable Write Quorum has been set
 */
riak_boolean_t
riak_put_options_get_has_dw(riak_put_options *opt);

/**
 * @brief Access the Durable Write Quroum on Put request
 * @param opt Riak Put Option
 * @returns Durable Write Quroum value
 */
riak_uint32_t
riak_put_options_get_dw(riak_put_options *opt);

/**
 * @brief Determine if Return Body flag has been set on Put request
 * @param opt Riak Put Option
 * @returns True if Return Body flag has been set
 */
riak_boolean_t
riak_put_options_get_has_return_body(riak_put_options *opt);

/**
 * @brief Access the Return Body flag on Put request
 * @param opt Riak Put Option
 * @returns Return Body flag value
 */
riak_boolean_t
riak_put_options_get_return_body(riak_put_options *opt);

/**
 * @brief Determine if Primary Write Quroum has been set on Put request
 * @param opt Riak Put Option
 * @returns True if Primary Write Quroum has been set
 */
riak_boolean_t
riak_put_options_get_has_pw(riak_put_options *opt);

/**
 * @brief Access the Primary Write Quroum on Put request
 * @param opt Riak Put Option
 * @returns Primary Write Quroum value
 */
riak_uint32_t
riak_put_options_get_pw(riak_put_options *opt);

/**
 * @brief Determine if the If Not Modified flag has been set on Put request
 * @param opt Riak Put Option
 * @returns True if the If Not Modified flag has been set
 */
riak_boolean_t
riak_put_options_get_has_if_not_modified(riak_put_options *opt);

/**
 * @brief Access the If Not Modified flag on Put request
 * @param opt Riak Put Option
 * @returns If Not Modified flag value
 */
riak_boolean_t
riak_put_options_get_if_not_modified(riak_put_options *opt);

/**
 * @brief Determine if the If None Match flag has been set on Put request
 * @param opt Riak Put Option
 * @returns True if the If None Match flag has been set
 */
riak_boolean_t
riak_put_options_get_has_if_none_match(riak_put_options *opt);

/**
 * @brief Access the If None Match flag on Put request
 * @param opt Riak Put Option
 * @returns If None Match flag value
 */
riak_boolean_t
riak_put_options_get_if_none_match(riak_put_options *opt);

/**
 * @brief Determine if Return Head flag has been set on Put request
 * @param opt Riak Put Option
 * @returns True if Return Head flag has been set
 */
riak_boolean_t
riak_put_options_get_has_return_head(riak_put_options *opt);

/**
 * @brief Access the Return Head flag on Put request
 * @param opt Riak Put Option
 * @returns Return Head flag value
 */
riak_boolean_t
riak_put_options_get_return_head(riak_put_options *opt);

/**
 * @brief Determine if Time Out has been set on Put request
 * @param opt Riak Put Option
 * @returns True if Time Out has been set
 */
riak_boolean_t
riak_put_options_get_has_timeout(riak_put_options *opt);

/**
 * @brief Access the Time Out on Put request
 * @param opt Riak Put Option
 * @returns Time Out value
 */
riak_uint32_t
riak_put_options_get_timeout(riak_put_options *opt);

/**
 * @brief Determine if As Is flag has been set on Put request
 * @param opt Riak Put Option
 * @returns True if As Is Flag has been set
 */
riak_boolean_t
riak_put_options_get_has_asis(riak_put_options *opt);

/**
 * @brief Access the As Is flag on Put request
 * @param opt Riak Put Option
 * @returns As Is flag value
 */
riak_boolean_t
riak_put_options_get_asis(riak_put_options *opt);

/**
 * @brief Determine if Sloppy Quorum flag has been set on Put request
 * @param opt Riak Put Option
 * @returns True if Sloppy Quorum flag has been set
 */
riak_boolean_t
riak_put_options_get_has_sloppy_quorum(riak_put_options *opt);

/**
 * @brief Access the Sloppy Quorum flag on Put request
 * @param opt Riak Put Option
 * @returns Sloppy Quorum flag value
 */
riak_boolean_t
riak_put_options_get_sloppy_quorum(riak_put_options *opt);

/**
 * @brief Determine if N Val has been set on Put request
 * @param opt Riak Put Option
 * @returns True if N Val has been set
 */
riak_boolean_t
riak_put_options_get_has_n_val(riak_put_options *opt);

/**
 * @brief Access the N Val on Put request
 * @param opt Riak Put Option
 * @returns N Val value
 */
riak_uint32_t
riak_put_options_get_n_val(riak_put_options *opt);

/**
 * @brief Set the Vector Clock
 * @param cfg Riak Configuration
 * @param opt Riak Put Option
 * @param value Vector Clock
 */
void
riak_put_options_set_vclock(riak_config      *cfg,
                            riak_put_options *opt,
                            riak_binary      *value);
/**
 * @brief Set the Write Quorum
 * @param opt Riak Put Option
 * @param value Write Quorum
 */
void
riak_put_options_set_w(riak_put_options *opt,
                       riak_uint32_t     value);
/**
 * @brief Set the Durable Write Quroum
 * @param opt Riak Put Option
 * @param value Durable Write Quorum
 */
void
riak_put_options_set_dw(riak_put_options *opt,
                        riak_uint32_t     value);
/**
 * @brief Set the Return Body flag
 * @param opt Riak Put Option
 * @param value Return Body flag
 */
void
riak_put_options_set_return_body(riak_put_options *opt,
                                 riak_boolean_t    value);
/**
 * @brief Set the Primary Write Quorum
 * @param opt Riak Put Option
 * @param value Primary Write Quorum
 */
void
riak_put_options_set_pw(riak_put_options *opt,
                        riak_uint32_t     value);
/**
 * @brief Set the If Not Modified flag
 * @param opt Riak Put Option
 * @param value If Not Modified flag
 */
void
riak_put_options_set_if_not_modified(riak_put_options *opt,
                                     riak_boolean_t    value);
/**
 * @brief Set the If None Match flag
 * @param opt Riak Put Option
 * @param value If None Match flag
 */
void
riak_put_options_set_if_none_match(riak_put_options *opt,
                                   riak_boolean_t    value);
/**
 * @brief Set the Return Head flag
 * @param opt Riak Put Option
 * @param value Return Head flag
 */
void
riak_put_options_set_return_head(riak_put_options *opt,
                                 riak_boolean_t    value);
/**
 * @brief Set the Time Out
 * @param opt Riak Put Option
 * @param value Time Out (in ms)
 */
void
riak_put_options_set_timeout(riak_put_options *opt,
                             riak_uint32_t     value);
/**
 * @brief Set the As Is flag
 * @param opt Riak Put Option
 * @param value As Is flag
 */
void
riak_put_options_set_asis(riak_put_options *opt,
                          riak_boolean_t    value);
/**
 * @brief Set the Sloppy Quorum flag
 * @param opt Riak Put Option
 * @param value Sloppy Quorum flag
 */
void
riak_put_options_set_sloppy_quorum(riak_put_options *opt,
                                   riak_boolean_t    value);
/**
 * @brief Set the N Val
 * @param opt Riak Put Option
 * @param value N Val
 */
void
riak_put_options_set_n_val(riak_put_options *opt,
                           riak_uint32_t     value);
#endif
