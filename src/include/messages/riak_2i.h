/*********************************************************************
 *
 * riak_2i.h: Riak C Client Index Message
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

#ifndef _RIAK_2I_MESSAGE_H
#define _RIAK_2I_MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _riak_2i_response riak_2i_response;
typedef struct _riak_2i_options riak_2i_options;
typedef void (*riak_2i_response_callback)(riak_2i_response *response, void *ptr);

/**
 * @brief Free Secondary Index response
 * @param cfg Riak Configuration
 * @param resp Secondary Index response
 */
void
riak_2i_response_free(riak_config           *cfg,
                          riak_2i_response **resp);

/**
 * @brief Print out a Secondary Index Response
 * @param state Riak Print State
 * @param response the Secondary Index response to print
 * @returns Total number of bytes written
 */
riak_int32_t
riak_2i_response_print(riak_print_state     *state,
                           riak_2i_response *response);

/**
 * @brief Access the number of key in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Number of keys
 */
riak_uint32_t
riak_2i_get_n_keys(riak_2i_response *response);

/**
 * @brief Access the keys in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Array of keys matching the query
 */
riak_binary**
riak_2i_get_keys(riak_2i_response *response);

/**
 * @brief Access the number of key/value pairs in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Number of key/value pairs
 */
riak_uint32_t
riak_2i_get_n_results(riak_2i_response *response);

/**
 * @brief Access the key/value pairs in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Array of key/value `return_terms` results
 */
riak_pair**
riak_2i_get_results(riak_2i_response *response);

/**
 * @brief Determine if a Continuation is in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns True if a Continuation is returned
 */
riak_boolean_t
riak_2i_get_has_continuation(riak_2i_response *response);

/**
 * @brief Access the Continuation in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns True if a Continuation is returned
 */
riak_binary*
riak_2i_get_continuation(riak_2i_response *response);

/**
 * @brief Determine if Vector Clock is in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns True if Vector Clock is returned
 */
riak_boolean_t
riak_2i_get_has_done(riak_2i_response *response);

/**
 * @brief Access the Done flag in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Done flag value
 */
riak_boolean_t
riak_2i_get_done(riak_2i_response *response);

/**
 * @brief Determine if Vector Clock is in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns True if Vector Clock is returned
 */
riak_boolean_t
riak_2i_get_has_vclock(riak_2i_response *response);

/**
 * @brief Access the Vector Clock in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Vector Clock value
 */
riak_binary*
riak_2i_get_vclock(riak_2i_response *response);

/**
 * @brief Determine if Unmodified flag is in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns True if Unmodified flag is returned
 */
riak_boolean_t
riak_2i_get_has_unmodified(riak_2i_response *response);

/**
 * @brief Access the Unmodified flag in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Unmodified flag
 */
riak_boolean_t
riak_2i_get_unmodified(riak_2i_response *response);

/**
 * @brief Access the Deleted flag in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Deleted flag
 */
riak_boolean_t
riak_2i_get_deleted(riak_2i_response *response);

/**
 * @brief Access the Number of Riak Objects in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Number of Riak Objects
 */
riak_int32_t
riak_2i_get_n_content(riak_2i_response *response);

/**
 * @brief Access an array of Riak Objects in a Secondary Index response
 * @param response Riak Secondary Index Response
 * @returns Array of Riak Objects (siblings)
 */
riak_object**
riak_2i_get_content(riak_2i_response *response);

/**
 * @brief Construct a new Riak Secondary Index Options object
 * @param cfg Riak Configuration
 * @return Riak Object
 */
riak_2i_options*
riak_2i_options_new(riak_config *cfg);

/**
 * @brief Release claimed memory used by Riak Secondary Index Options
 * @param cfg Riak Configuration
 * @param obj Riak Object to be freed
 */
void
riak_2i_options_free(riak_config          *cfg,
                         riak_2i_options **opt);

/**
 * @brief Print out a Secondary Index Options
 * @param response the Secondary Index Options to print
 * @param target Location to write the string (in/out)
 * @param len Remaining available bytes to write (in/out)
 * @param total Running total of bytes written (in/out)
 * @returns Total number of bytes written
 */
riak_int32_t
riak_2i_options_print(riak_2i_response *response,
                          char                **target,
                          riak_int32_t         *len,
                          riak_int32_t         *total);

/**
 * @brief Determine if this is a range Secondary Index query
 * @param opt Riak Secondary Index Option
 * @returns True if Secondary Index query (not exact match)
 */
riak_boolean_t
riak_2i_options_get_is_range_query(riak_2i_options *opt);

/**
 * @brief Determine if the key has been set
 * @param opt Riak Secondary Index Option
 * @returns True if key has been set
 */
riak_boolean_t
riak_2i_options_get_has_key(riak_2i_options *opt);

/**
 * @brief Access the key on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Read key value
 */
riak_binary*
riak_2i_options_get_key(riak_2i_options *opt);

/**
 * @brief Determine if the range minimum has been set
 * @param opt Riak Secondary Index Option
 * @returns True if range minimum has been set
 */
riak_boolean_t
riak_2i_options_get_has_range_min(riak_2i_options *opt);

/**
 * @brief Access the range minimum on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Range minimum value
 */
riak_binary*
riak_2i_options_get_range_min(riak_2i_options *opt);

/**
 * @brief Determine if the range maximum has been set
 * @param opt Riak Secondary Index Option
 * @returns True if range maximum has been set
 */
riak_boolean_t
riak_2i_options_get_has_range_max(riak_2i_options *opt);

/**
 * @brief Access the Range maximum on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Range maximum value
 */
riak_binary*
riak_2i_options_get_range_max(riak_2i_options *opt);

/**
 * @brief Determine if the return terms flag has been set
 * @param opt Riak Secondary Index Option
 * @returns True if return terms flag has been set
 */
riak_boolean_t
riak_2i_options_get_has_return_terms(riak_2i_options *opt);

/**
 * @brief Access the Return terms flag on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Return terms flag
 */
riak_boolean_t
riak_2i_options_get_return_terms(riak_2i_options *opt);

/**
 * @brief Determine if the streaming flag has been set
 * @param opt Riak Secondary Index Option
 * @returns True if streaming flag has been set
 */
riak_boolean_t
riak_2i_options_get_has_stream(riak_2i_options *opt);

/**
 * @brief Access the streaming flag on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Streaming flag value
 */
riak_boolean_t
riak_2i_options_get_stream(riak_2i_options *opt);

/**
 * @brief Determine if the maximum results has been set
 * @param opt Riak Secondary Index Option
 * @returns True if maximum results has been set
 */
riak_boolean_t
riak_2i_options_get_has_max_results(riak_2i_options *opt);

/**
 * @brief Access the Maximum results on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Maximum results value
 */
riak_uint32_t
riak_2i_options_get_max_results(riak_2i_options *opt);

/**
 * @brief Determine if the continuation value has been set
 * @param opt Riak Secondary Index Option
 * @returns True if continuation has been set
 */
riak_boolean_t
riak_2i_options_get_has_continuation(riak_2i_options *opt);

/**
 * @brief Access the continuation value on Secondary Index option (for pagaination)
 * @param opt Riak Secondary Index Option
 * @returns Continuations value
 */
riak_binary*
riak_2i_options_get_continuation(riak_2i_options *opt);

/**
 * @brief Determine if the timeout has been set
 * @param opt Riak Secondary Index Option
 * @returns True if timeout has been set
 */
riak_boolean_t
riak_2i_options_get_has_timeout(riak_2i_options *opt);

/**
 * @brief Access the timeout on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Timeoute value
 */
riak_uint32_t
riak_2i_options_get_timeout(riak_2i_options *opt);

/**
 * @brief Determine if the type has been set
 * @param opt Riak Secondary Index Option
 * @returns True if type has been set
 */
riak_boolean_t
riak_2i_options_get_has_type(riak_2i_options *opt);

/**
 * @brief Access the type (binary or string) on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Type value
 */
riak_binary*
riak_2i_options_get_type(riak_2i_options *opt);

/**
 * @brief Determine if a regular expression has been set
 * @param opt Riak Secondary Index Option
 * @returns True if regular expression has been set
 */
riak_boolean_t
riak_2i_options_get_has_term_regex(riak_2i_options *opt);

/**
 * @brief Access the Search Term Regular Expression on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Search Term Regular Expression
 */
riak_binary*
riak_2i_options_get_term_regex(riak_2i_options *opt);

/**
 * @brief Determine if the pagination sort flag has been set
 * @param opt Riak Secondary Index Option
 * @returns True if pagination sort has been set
 */
riak_boolean_t
riak_2i_options_get_has_pagination_sort(riak_2i_options *opt);

/**
 * @brief Access the pagination sort flag on Secondary Index option
 * @param opt Riak Secondary Index Option
 * @returns Pagination sort flag
 */
riak_boolean_t
riak_2i_options_get_pagination_sort(riak_2i_options *opt);

/**
 * @brief Switch query from index to range query
 * @param opt Riak Secondary Index Option
 */
void
riak_2i_options_set_range_query(riak_2i_options *opt);

/**
 * @brief Set the equality query Key on Secondary Index Options
 * @param cfg Riak Configuration
 * @param opt Riak Secondary Index Option
 * @param value Range Minimum
 * @return Error Code
 */
riak_error
riak_2i_options_set_key(riak_config         *cfg,
                            riak_2i_options *opt,
                            riak_binary         *value);

/**
 * @brief Set the Range Minimum on Secondary Index Options
 * @param cfg Riak Configuration
 * @param opt Riak Secondary Index Option
 * @param value Range Minimum
 * @return Error Code
 */
riak_error
riak_2i_options_set_range_min(riak_config         *cfg,
                                  riak_2i_options *opt,
                                  riak_binary         *value);

/**
 * @brief Set the Range Maximum on Secondary Index Options
 * @param cfg Riak Configuration
 * @param opt Riak Secondary Index Option
 * @param value Range Maximum
 * @return Error Code
 */
riak_error
riak_2i_options_set_range_max(riak_config         *cfg,
                                  riak_2i_options *opt,
                                  riak_binary         *value);

/**
 * @brief Set the return terms flag on Secondary Index Options
 * @param opt Riak Secondary Index Option
 * @param value True if you want the terms returned
 */
void
riak_2i_options_set_return_terms(riak_2i_options *opt,
                                     riak_boolean_t       value);
/**
 * @brief Set the streming flag on Secondary Index Options
 * @param opt Riak Secondary Index Option
 * @param value True if you want the results streamed
 */
void
riak_2i_options_set_stream(riak_2i_options *opt,
                               riak_boolean_t       value);
void
/**
 * @brief Set the maximum number of results on Secondary Index Options
 * @param opt Riak Secondary Index Option
 * @param value Maximum returned results
 */
riak_2i_options_set_max_results(riak_2i_options *opt,
                                    riak_uint32_t        value);

/**
 * @brief Set the continuation value on Secondary Index Options (for paging)
 * @param cfg Riak Configuration
 * @param opt Riak Secondary Index Option
 * @param value Continuation from last results
 * @return Error Code
 */
riak_error
riak_2i_options_set_continuation(riak_config         *cfg,
                                     riak_2i_options *opt,
                                     riak_binary         *value);
/**
 * @brief Set the timeout on Secondary Index Options
 * @param opt Riak Secondary Index Option
 * @param value Timeout in ms
 */
void
riak_2i_options_set_timeout(riak_2i_options *opt,
                                riak_uint32_t        value);

/**
 * @brief Set the data type on Secondary Index Options
 * @param cfg Riak Configuration
 * @param opt Riak Secondary Index Option
 * @param value Data Type (binary or string)
 * @return Error Code
 */
riak_error
riak_2i_options_set_type(riak_config         *cfg,
                             riak_2i_options *opt,
                             riak_binary         *value);

/**
 * @brief Set the Search Term Regular Expression on Secondary Index Options
 * @param cfg Riak Configuration
 * @param opt Riak Secondary Index Option
 * @param value Regular Expression
 * @return Error Code
 */
riak_error
riak_2i_options_set_term_regex(riak_config         *cfg,
                                   riak_2i_options *opt,
                                   riak_binary         *value);

/**
 * @brief Set the Pagination Sort flag on Secondary Index Options
 * @param opt Riak Secondary Index Option
 * @param value True if the paginated results are to be sorted
 */
void
riak_2i_options_set_pagination_sort(riak_2i_options *opt,
                                         riak_boolean_t      value);

#ifdef __cplusplus
}
#endif

#endif // _RIAK_2I_MESSAGE_H
