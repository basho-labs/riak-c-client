/*********************************************************************
 *
 * riak_put_options.h: Riak C Options
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

#ifndef RIAK_OPTIONS_H_
#define RIAK_OPTIONS_H_

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
riak_put_options_free(riak_config *cfg,
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

riak_boolean_t riak_put_options_get_has_vclock(riak_put_options *opt);
riak_binary   *riak_put_options_get_vclock(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_w(riak_put_options *opt);
riak_uint32_t  riak_put_options_get_w(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_dw(riak_put_options *opt);
riak_uint32_t  riak_put_options_get_dw(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_return_body(riak_put_options *opt);
riak_boolean_t riak_put_options_get_return_body(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_pw(riak_put_options *opt);
riak_uint32_t  riak_put_options_get_pw(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_if_not_modified(riak_put_options *opt);
riak_boolean_t riak_put_options_get_if_not_modified(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_if_none_match(riak_put_options *opt);
riak_boolean_t riak_put_options_get_if_none_match(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_return_head(riak_put_options *opt);
riak_boolean_t riak_put_options_get_return_head(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_timeout(riak_put_options *opt);
riak_uint32_t  riak_put_options_get_timeout(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_asis(riak_put_options *opt);
riak_boolean_t riak_put_options_get_asis(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_sloppy_quorum(riak_put_options *opt);
riak_boolean_t riak_put_options_get_sloppy_quorum(riak_put_options *opt);
riak_boolean_t riak_put_options_get_has_n_val(riak_put_options *opt);
riak_uint32_t  riak_put_options_get_n_val(riak_put_options *opt);
void riak_put_options_set_vclock(riak_put_options *opt, riak_binary *value);
void riak_put_options_set_w(riak_put_options *opt, riak_uint32_t value);
void riak_put_options_set_dw(riak_put_options *opt, riak_uint32_t value);
void riak_put_options_set_return_body(riak_put_options *opt, riak_boolean_t value);
void riak_put_options_set_pw(riak_put_options *opt, riak_uint32_t value);
void riak_put_options_set_if_not_modified(riak_put_options *opt, riak_boolean_t value);
void riak_put_options_set_if_none_match(riak_put_options *opt, riak_boolean_t value);
void riak_put_options_set_return_head(riak_put_options *opt, riak_boolean_t value);
void riak_put_options_set_timeout(riak_put_options *opt, riak_uint32_t value);
void riak_put_options_set_asis(riak_put_options *opt, riak_boolean_t value);
void riak_put_options_set_sloppy_quorum(riak_put_options *opt, riak_boolean_t value);
void riak_put_options_set_n_val(riak_put_options *opt, riak_uint32_t value);

#endif /* RIAK_OPTIONS_H_ */
