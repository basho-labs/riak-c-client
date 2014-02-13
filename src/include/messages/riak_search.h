/*********************************************************************
 *
 * riak_search.h: Riak C Client Search Message
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

#ifndef _RIAK_SEARCH_H
#define _RIAK_SEARCH_H

typedef struct _riak_search_options riak_search_options;
typedef struct _riak_search_response riak_search_response;
typedef struct _riak_search_doc riak_search_doc;
typedef void (*riak_search_response_callback)(riak_search_response *response, void *ptr);

/**
 * @brief Free Riak Search response
 * @param cfg Riak Configuration
 * @param resp Riak Search response
 */
void
riak_search_response_free(riak_config           *cfg,
                          riak_search_response **resp);

/**
 * @brief Print out a Riak Search Response
 * @param response the Riak Search response to print
 * @param target Location to write the string (in/out)
 * @param len Remaining available bytes to write (in/out)
 * @param total Running total of bytes written (in/out)
 * @returns Number of bytes written
 */
riak_int32_t
riak_search_response_print(riak_search_response *response,
                           char                **target,
                           riak_int32_t         *len,
                           riak_int32_t         *total);

/**
 * @brief Access the number of documents in a Search response
 * @param response Riak Search Response
 * @returns Number of search result documents
 */
riak_size_t
riak_search_get_n_docs(riak_search_response *response);

/**
 * @brief Access the search documents in a Search response
 * @param response Riak Search Response
 * @param n Index of search document
 * @returns A Riak Key/Value Pair as search results
 */
riak_search_doc*
riak_search_get_doc(riak_search_response *response,
                    riak_uint32_t         n);

/**
 * @brief Determine if the Maximum Score is in Search response
 * @param response Riak Search Response
 * @returns True if Maximum Score is populated
 */
riak_boolean_t
riak_search_get_has_max_score(riak_search_response *response);

/**
 * @brief Access the Maximum Score in a Search response
 * @param response Riak Search Response
 * @returns Maximum Score
 */
riak_float32_t
riak_search_get_max_score(riak_search_response *response);

/**
 * @brief Determine if the Number Found is in Search response
 * @param response Riak Search Response
 * @returns True if Number Found is populated
 */
riak_boolean_t
riak_search_get_has_num_found(riak_search_response *response);

/**
 * @brief Access the number of matches found in a Search response
 * @param response Riak Search Response
 * @returns Total number of Search results found
 */
riak_uint32_t
riak_search_get_num_found(riak_search_response *response);

/**
 * @brief Access the number of fields in a Search Document
 * @param doc Search Document
 * @return Number of fields in document
 */
riak_size_t
riak_search_doc_get_n_fields(riak_search_doc *doc);

/**
 * @brief Access the nth field in a Search Document
 * @param doc Search Document
 * @param n Index in the field array
 * @returns The nth field in a Search Document
 */
riak_pair*
riak_search_doc_get_field(riak_search_doc *doc,
                          riak_uint32_t    n);

/**
 * @brief Construct a new Riak Search Options object
 * @param cfg Riak Configuration
 * @return Riak Object
 */
riak_search_options*
riak_search_options_new(riak_config *cfg);

/**
 * @brief Release claimed memory used by Riak Search Options
 * @param cfg Riak Configuration
 * @param opt Riak Options to be freed
 */
void
riak_search_options_free(riak_config          *cfg,
                         riak_search_options **opt);

/**
 * @brief Determine if the Max Row field is in Search Options
 * @param opt Riak Search Options
 * @returns True if Max Row field is populated
 */
riak_boolean_t
riak_search_options_get_has_rows(riak_search_options *opt);

/**
 * @brief Access the Maximum Number of Rows in Search options
 * @param opt Riak Search Options
 * @returns Maximum Number of Rows to return in a search
 */
riak_uint32_t
riak_search_options_get_rows(riak_search_options *opt);

/**
 * @brief Determine if the Start option is in Search Options
 * @param opt Riak Search Options
 * @returns True if Start row is set
 */
riak_boolean_t
riak_search_options_get_has_start(riak_search_options *opt);

/**
 * @brief Access the Start Row in Search options
 * @param opt Riak Search Options
 * @returns The First Row (offset) to return in a search result
 */
riak_uint32_t
riak_search_options_get_start(riak_search_options *opt);

/**
 * @brief Determine if the Sort is in Search Options
 * @param opt Riak Search Options
 * @returns True if Sort option is set
 */
riak_boolean_t
riak_search_options_get_has_sort(riak_search_options *opt);

/**
 * @brief Access the Sort value in Search options
 * @param opt Riak Search Options
 * @returns Sorting option
 */
riak_binary*
riak_search_options_get_sort(riak_search_options *opt);

/**
 * @brief Determine if Search Filter is in Search Options
 * @param opt Riak Search Options
 * @returns True if Search Filter is set
 */
riak_boolean_t
riak_search_options_get_has_filter(riak_search_options *opt);

/**
 * @brief Access the Search Filter in Search options
 * @param opt Riak Search Options
 * @returns The Search Filter option
 */
riak_binary*
riak_search_options_get_filter(riak_search_options *opt);

/**
 * @brief Determine if Default Field is in Search Options
 * @param opt Riak Search Options
 * @returns True if Default Field is specified
 */
riak_boolean_t
riak_search_options_get_has_df(riak_search_options *opt);

/**
 * @brief Access the Default Field override in Search options
 * @param opt Riak Search Options
 * @returns Default Field override
 */
riak_binary*
riak_search_options_get_df(riak_search_options *opt);

/**
 * @brief Determine if And/Or Operator is in Search Options
 * @param opt Riak Search Options
 * @returns True if And/Or Operator is set
 */
riak_boolean_t
riak_search_options_get_has_op(riak_search_options *opt);

/**
 * @brief Access the value of the And/Or Operator in Search options
 * @param opt Riak Search Options
 * @returns And/Or Operator
 */
riak_binary*
riak_search_options_get_op(riak_search_options *opt);

/**
 * @brief Access the Number of Field Limits in Search options
 * @param opt Riak Search Options
 * @returns Number of Field Limits set
 */
riak_size_t
riak_search_options_get_n_fl(riak_search_options *opt);

/**
 * @brief Access the Field Limits in Search options
 * @param opt Riak Search Options
 * @returns Array of Field Limits
 */
riak_binary**
riak_search_options_get_fl(riak_search_options *opt);

/**
 * @brief Determine if Presort is in Search Options
 * @param opt Riak Search Options
 * @returns True if Presort is set
 */
riak_boolean_t
riak_search_options_get_has_presort(riak_search_options *opt);

/**
 * @brief Access the value of Presort in Search options
 * @param opt Riak Search Options
 * @returns Presort
 */
riak_binary*
riak_search_options_get_presort(riak_search_options *opt);

/**
 * @brief Set the Maximum Number of Rows in result
 * @param opt Riak Search Options
 * @param value Maximum Number of Rows
 */
void
riak_search_options_set_rows(riak_search_options *opt,
                             riak_uint32_t        value);

/**
 * @brief Set the Starting Row Number
 * @param opt Riak Search Options
 * @param value Starting Row Number of result (for paging)
 */
void
riak_search_options_set_start(riak_search_options *opt,
                              riak_uint32_t        value);
/**
 * @brief Set the Sort option
 * @param cfg Riak Configuration
 * @param opt Riak Search Options
 * @param value Result Sorting option
 * @returns Error code
 */
riak_error
riak_search_options_set_sort(riak_config         *cfg,
                             riak_search_options *opt,
                             riak_binary         *value);

/**
 * @brief Set the Search Filter
 * @param cfg Riak Configuration
 * @param opt Riak Search Options
 * @param value Search Filter
 * @returns Error code
 */
riak_error
riak_search_options_set_filter(riak_config         *cfg,
                               riak_search_options *opt,
                               riak_binary         *value);
/**
 * @brief Set the Default Field
 * @param cfg Riak Configuration
 * @param opt Riak Search Options
 * @param value Default Field
 * @returns Error code
 */
riak_error
riak_search_options_set_df(riak_config         *cfg,
                           riak_search_options *opt,
                           riak_binary         *value);

/**
 * @brief Set the And/Or Operator
 * @param cfg Riak Configuration
 * @param opt Riak Search Options
 * @param value And/Or Operator
 * @returns Error code
 */
riak_error
riak_search_options_set_op(riak_config         *cfg,
                           riak_search_options *opt,
                           riak_binary         *value);
/**
 * @brief Add a Field Limit to Search
 * @param opt Riak Search Options
 * @param value A Field Limit to be added to Search
 * @returns Error code
 */
riak_error
riak_search_options_add_fl(riak_config         *cfg,
                           riak_search_options *opt,
                           riak_binary         *value);
/**
 * @brief Set the Presort Option
 * @param cfg Riak Configuration
 * @param opt Riak Search Options
 * @param value Presort Option
 * @returns Error code
 */
riak_error
riak_search_options_set_presort(riak_config         *cfg,
                                riak_search_options *opt,
                                riak_binary         *value);

#endif // _RIAK_SEARCH_H
