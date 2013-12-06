/*********************************************************************
 *
 * riak_messages.h: Riak C Client Get Message
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
riak_free_get_response(riak_config        *cfg,
                       riak_get_response **resp);


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

riak_boolean_t
riak_get_options_get_has_r(riak_get_options *opt);
riak_uint32_t
riak_get_options_get_r(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_has_pr(riak_get_options *opt);
riak_uint32_t
riak_get_options_get_pr(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_has_basic_quorum(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_basic_quorum(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_has_notfound_ok(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_notfound_ok(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_has_if_modified(riak_get_options *opt);
riak_binary*
riak_get_options_get_if_modified(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_has_head(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_head(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_has_deletedvclock(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_deletedvclock(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_has_timeout(riak_get_options *opt) ;
riak_uint32_t
riak_get_options_get_timeout(riak_get_options *opt) ;
riak_boolean_t
riak_get_options_get_has_sloppy_quorum(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_sloppy_quorum(riak_get_options *opt);
riak_boolean_t
riak_get_options_get_has_n_val(riak_get_options *opt);
riak_uint32_t
riak_get_options_get_n_val(riak_get_options *opt);
void
riak_get_options_set_r(riak_get_options *opt,
                       riak_uint32_t value);
void
riak_get_options_set_pr(riak_get_options *opt,
                        riak_uint32_t value);
void
riak_get_options_set_basic_quorum(riak_get_options *opt,
                                  riak_boolean_t    value);
void
riak_get_options_set_notfound_ok(riak_get_options *opt,
                                 riak_boolean_t    value);
void
riak_get_options_set_if_modified(riak_config      *cfg,
                                 riak_get_options *opt,
                                 riak_binary      *value);
void
riak_get_options_set_head(riak_get_options *opt,
                          riak_boolean_t    value);
void
riak_get_options_set_deletedvclock(riak_get_options *opt,
                                   riak_boolean_t    value);
void
riak_get_options_set_timeout(riak_get_options *opt,
                             riak_uint32_t     value);
void
riak_get_options_set_sloppy_quorum(riak_get_options *opt,
                                   riak_boolean_t    value);
void
riak_get_options_set_n_val(riak_get_options *opt,
                           riak_uint32_t     value);

#endif
