/*********************************************************************
 *
 * riak_get.h: Riak C Client Get Message
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

#ifndef _RIAK_GET_MESSAGE_H
#define _RIAK_GET_MESSAGE_H

typedef struct _riak_get_response riak_get_response;
typedef struct _riak_get_options riak_get_options;
typedef void (*riak_get_response_callback)(riak_get_response *response, void *ptr);

/**
 * @brief Print a summary of a `riak_get_response`
 * @param response Result from a Get request
 * @param target Location of string to be formatted
 * @param len Number of free bytes
 * @returns Number of bytes written
 */
int
riak_print_get_response(riak_get_response *response,
                        char              *target,
                        riak_size_t        len);

/**
 * @brief Free get response
 * @param cfg Riak Configuration
 * @param resp Get response
 */
void
riak_get_response_free(riak_config        *cfg,
                       riak_get_response **resp);

/**
 * @brief Determine if Vector Clock is in a Get response
 * @param response Riak Get Response
 * @returns True if Vector Clock is returned
 */
riak_boolean_t
riak_get_get_has_vclock(riak_get_response *response);

/**
 * @brief Access the Vector Clock in a Get response
 * @param response Riak Get Response
 * @returns Vector Clock value
 */
riak_binary*
riak_get_get_vclock(riak_get_response *response);

/**
 * @brief Determine if Unmodified flag is in a Get response
 * @param response Riak Get Response
 * @returns True if Unmodified flag is returned
 */
riak_boolean_t
riak_get_get_has_unmodified(riak_get_response *response);

/**
 * @brief Access the Unmodified flag in a Get response
 * @param response Riak Get Response
 * @returns Unmodified flag
 */
riak_boolean_t
riak_get_get_unmodified(riak_get_response *response);

/**
 * @brief Access the Deleted flag in a Get response
 * @param response Riak Get Response
 * @returns Deleted flag
 */
riak_boolean_t
riak_get_get_deleted(riak_get_response *response);

/**
 * @brief Access the Number of Riak Objects in a Get response
 * @param response Riak Get Response
 * @returns Number of Riak Objects
 */
riak_int32_t
riak_get_get_n_content(riak_get_response *response);

/**
 * @brief Access an array of Riak Objects in a Get response
 * @param response Riak Get Response
 * @returns Array of Riak Objects (siblings)
 */
riak_object**
riak_get_get_content(riak_get_response *response);

/**
 * @brief Construct a new Riak Get Options object
 * @param cfg Riak Configuration
 * @return Riak Object
 */
riak_get_options*
riak_get_options_new(riak_config *cfg);

/**
 * @brief Release claimed memory used by Riak Get Options
 * @param cfg Riak Configuration
 * @param obj Riak Object to be freed
 */
void
riak_get_options_free(riak_config       *cfg,
                      riak_get_options **opt);

/**
 * @brief Print contents of a Riak Get Options to a string
 * @param obj Object to print
 * @param target Location to write formatted string
 * @param len Number of bytes to write
 * @return Number of bytes written
 */
int
riak_get_options_print(riak_get_options *opt,
                       char             *target,
                       riak_int32_t      len);

/**
 * @brief Determine if Read Quorum has been set on Get request
 * @param opt Riak Get Option
 * @returns True if Read Quorum has been set
 */
riak_boolean_t
riak_get_options_get_has_r(riak_get_options *opt);

/**
 * @brief Access the Read Quorum on Get request
 * @param opt Riak Get Option
 * @returns Read Quorum value
 */
riak_uint32_t
riak_get_options_get_r(riak_get_options *opt);

/**
 * @brief Determine if Primary Read Quorum has been set on Get request
 * @param opt Riak Get Option
 * @returns True if Primary Read Quorum has been set
 */
riak_boolean_t
riak_get_options_get_has_pr(riak_get_options *opt);

/**
 * @brief Access the Primary Read Quorum on Get request
 * @param opt Riak Get Option
 * @returns Primary Read Quorum value
 */
riak_uint32_t
riak_get_options_get_pr(riak_get_options *opt);
/**
 * @brief Determine if Basic Quorum flag has been set on Get request
 * @param opt Riak Get Option
 * @returns True if has been set
 */
riak_boolean_t
riak_get_options_get_has_basic_quorum(riak_get_options *opt);

/**
 * @brief Access the Basic Quorum flag on Get request
 * @param opt Riak Get Option
 * @returns Basic Quorum value
 */
riak_boolean_t
riak_get_options_get_basic_quorum(riak_get_options *opt);

/**
 * @brief Determine if Not Found OK flag has been set on Get request
 * @param opt Riak Get Option
 * @returns True if Not Found OK flag has been set
 */
riak_boolean_t
riak_get_options_get_has_notfound_ok(riak_get_options *opt);

/**
 * @brief Access the Not Found OK flag on Get request
 * @param opt Riak Get Option
 * @returns Not Found OK value
 */
riak_boolean_t
riak_get_options_get_notfound_ok(riak_get_options *opt);

/**
 * @brief Determine if the If Modified flag has been set on Get request
 * @param opt Riak Get Option
 * @returns True if the If Modified flag has been set
 */
riak_boolean_t
riak_get_options_get_has_if_modified(riak_get_options *opt);

/**
 * @brief Access the If Modified flag on Get request
 * @param opt Riak Get Option
 * @returns If Modified flag
 */
riak_binary*
riak_get_options_get_if_modified(riak_get_options *opt);

/**
 * @brief Determine if the Get Head flag has been set on Get request
 * @param opt Riak Get Option
 * @returns True if has been set
 */
riak_boolean_t
riak_get_options_get_has_head(riak_get_options *opt);

/**
 * @brief Access the Get Head flag on Get request
 * @param opt Riak Get Option
 * @returns Get Head flag
 */
riak_boolean_t
riak_get_options_get_head(riak_get_options *opt);

/**
 * @brief Determine if Deleted Vector Clock flag has been set on Get request
 * @param opt Riak Get Option
 * @returns True if Deleted Vector Clock flag has been set
 */
riak_boolean_t
riak_get_options_get_has_deletedvclock(riak_get_options *opt);

/**
 * @brief Access the Deleted Vector Clock flag on Get request
 * @param opt Riak Get Option
 * @returns Deleted Vector Clock flag
 */
riak_boolean_t
riak_get_options_get_deletedvclock(riak_get_options *opt);

/**
 * @brief Determine if Time Out has been set on Get request
 * @param opt Riak Get Option
 * @returns True if Time Out has been set
 */
riak_boolean_t
riak_get_options_get_has_timeout(riak_get_options *opt);

/**
 * @brief Access the Time Out on Get request
 * @param opt Riak Get Option
 * @returns Time Out (in ms)
 */
riak_uint32_t
riak_get_options_get_timeout(riak_get_options *opt);

/**
 * @brief Determine if Sloppy Quorum flag has been set on Get request
 * @param opt Riak Get Option
 * @returns True if Sloppy Quorum flag has been set
 */
riak_boolean_t
riak_get_options_get_has_sloppy_quorum(riak_get_options *opt);

/**
 * @brief Access the Sloppy Quorum flag on Get request
 * @param opt Riak Get Option
 * @returns Sloppy Quorum flag
 */
riak_boolean_t
riak_get_options_get_sloppy_quorum(riak_get_options *opt);

/**
 * @brief Determine if N Val has been set on Get request
 * @param opt Riak Get Option
 * @returns True if N Val has been set
 */
riak_boolean_t
riak_get_options_get_has_n_val(riak_get_options *opt);

/**
 * @brief Access N Val on Get request
 * @param opt Riak Get Option
 * @returns N Val
 */
riak_uint32_t
riak_get_options_get_n_val(riak_get_options *opt);

/**
 * @brief Set the Read Quorum
 * @param opt Riak Get Option
 * @param value Read Quorum
 */
void
riak_get_options_set_r(riak_get_options *opt,
                       riak_uint32_t     value);
/**
 * @brief Set the Primary Read Quorum
 * @param opt Riak Get Option
 * @param value Primary Read Quorum
 */
void
riak_get_options_set_pr(riak_get_options *opt,
                        riak_uint32_t     value);
/**
 * @brief Set the Basic Quorum flag
 * @param opt Riak Get Option
 * @param value Basic Quorum flag
 */
void
riak_get_options_set_basic_quorum(riak_get_options *opt,
                                  riak_boolean_t    value);
/**
 * @brief Set the Not Found OK flag
 * @param opt Riak Get Option
 * @param value Not Found OK flag
 */
void
riak_get_options_set_notfound_ok(riak_get_options *opt,
                                 riak_boolean_t    value);
/**
 * @brief Set the If Modified flag
 * @param opt Riak Get Option
 * @param value If Modified flag
 */
void
riak_get_options_set_if_modified(riak_config      *cfg,
                                 riak_get_options *opt,
                                 riak_binary      *value);
/**
 * @brief Set the Return Head Only flag
 * @param opt Riak Get Option
 * @param value Return Head Only flag
 */
void
riak_get_options_set_head(riak_get_options *opt,
                          riak_boolean_t    value);
/**
 * @brief Set the Deleted Vector Clock flag
 * @param opt Riak Get Option
 * @param value Deleted Vector Clock flag
 */
void
riak_get_options_set_deletedvclock(riak_get_options *opt,
                                   riak_boolean_t    value);
/**
 * @brief Set the Get Time Out
 * @param opt Riak Get Option
 * @param value Time Out (in ms)
 */
void
riak_get_options_set_timeout(riak_get_options *opt,
                             riak_uint32_t     value);
/**
 * @brief Set the Sloppy Quorum flag
 * @param opt Riak Get Option
 * @param value Sloppy Quorum flag
 */
void
riak_get_options_set_sloppy_quorum(riak_get_options *opt,
                                   riak_boolean_t    value);
/**
 * @brief Set the N Value
 * @param opt Riak Get Option
 * @param value N Value
 */
void
riak_get_options_set_n_val(riak_get_options *opt,
                           riak_uint32_t     value);

#endif
