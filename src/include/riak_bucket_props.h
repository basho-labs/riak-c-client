/*********************************************************************
 *
 * riak_bucket_props.h: Riak C Client Bucket Properties
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

#ifndef RIAK_BUCKET_PROPS_H
#define RIAK_BUCKET_PROPS_H

typedef enum _riak_bucket_repl_mode {
    RIAK_BUCKET_PROPS_REPL_FALSE = 0,
    RIAK_BUCKET_PROPS_REPL_REALTIME = 1,
    RIAK_BUCKET_PROPS_REPL_FULLSYNC = 2,
    RIAK_BUCKET_PROPS_REPL_TRUE = 3
} riak_bucket_repl_mode;

typedef struct _riak_mod_fun riak_mod_fun;
typedef struct _riak_commit_hook riak_commit_hook;
typedef struct _riak_bucket_props riak_bucket_props;

/**
 * @brief Construct a new Riak Bucket Property object
 * @param cfg Riak Configuration
 * @return Riak Bucket Property object
 */
riak_bucket_props*
riak_bucket_props_new(riak_config *cfg);

/**
 * @brief Release claimed memory used by a Riak Bucket Property object
 * @param cfg Riak Configuration
 * @param props Riak Bucket Property object to be freed
 */
void
riak_bucket_props_free(riak_config       *cfg,
                       riak_bucket_props **props);

/**
 * @brief Print contents of a Riak Bucket Property object to a string
 * @param props Bucket Property object to print
 * @param target Location to write formatted string
 * @param len Maximum number of bytes to write
 * @return Number of bytes written
 */
riak_int32_t
riak_bucket_props_print(riak_bucket_props *prop,
                        char              *target,
                        riak_int32_t       len);

riak_boolean_t   riak_bucket_props_get_has_n_val(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_n_val(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_allow_mult(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_allow_mult(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_last_write_wins(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_last_write_wins(riak_bucket_props *prop);
riak_size_t      riak_bucket_props_get_n_precommit(riak_bucket_props *prop);
riak_commit_hook **riak_bucket_props_get_precommit(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_precommit(riak_bucket_props *prop);
riak_size_t      riak_bucket_props_get_n_postcommit(riak_bucket_props *prop);
riak_commit_hook **riak_bucket_props_get_postcommit(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_postcommit(riak_bucket_props *prop);
riak_mod_fun    *riak_bucket_props_get_chash_keyfun(riak_bucket_props *prop);
riak_mod_fun    *riak_bucket_props_get_linkfun(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_old_vclock(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_old_vclock(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_young_vclock(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_young_vclock(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_big_vclock(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_big_vclock(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_small_vclock(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_small_vclock(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_pr(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_pr(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_r(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_r(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_w(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_w(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_pw(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_pw(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_dw(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_dw(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_rw(riak_bucket_props *prop);
riak_uint32_t    riak_bucket_props_get_rw(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_basic_quorum(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_basic_quorum(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_notfound_ok(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_notfound_ok(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_backend(riak_bucket_props *prop);
riak_binary     *riak_bucket_props_get_backend(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_search(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_search(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_repl(riak_bucket_props *prop);
riak_bucket_repl_mode riak_bucket_props_get_repl(riak_bucket_props *prop);
riak_boolean_t   riak_bucket_props_get_has_yz_index(riak_bucket_props *prop);
riak_binary     *riak_bucket_props_get_yz_index(riak_bucket_props *prop);
void riak_bucket_props_set_n_val(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_allow_mult(riak_bucket_props*prop, riak_boolean_t value);
void riak_bucket_props_set_last_write_wins(riak_bucket_props*prop, riak_boolean_t value);
void riak_bucket_props_set_precommits(riak_bucket_props*prop, riak_commit_hook **precommit, riak_size_t num);
void riak_bucket_props_set_postcommits(riak_bucket_props*prop, riak_commit_hook **postcommit, riak_size_t num);
void riak_bucket_props_set_chash_keyfun(riak_bucket_props*prop, riak_mod_fun *value);
void riak_bucket_props_set_linkfun(riak_bucket_props*prop, riak_mod_fun *value);
void riak_bucket_props_set_old_vclock(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_young_vclock(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_big_vclock(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_small_vclock(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_pr(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_r(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_w(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_pw(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_dw(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_rw(riak_bucket_props*prop, riak_uint32_t value);
void riak_bucket_props_set_basic_quorum(riak_bucket_props*prop, riak_boolean_t value);
void riak_bucket_props_set_notfound_ok(riak_bucket_props*prop, riak_boolean_t value);
void riak_bucket_props_set_backend(riak_bucket_props*prop, riak_binary *value);
void riak_bucket_props_set_search(riak_bucket_props*prop, riak_boolean_t value);
void riak_bucket_props_set_repl(riak_bucket_props*prop, riak_bucket_repl_mode value);
void riak_bucket_props_set_yz_index(riak_bucket_props*prop, riak_binary *value);

/**
 * @brief Create a commit hook
 * @param cfg Riak Configuration
 * @returns Freshly baked Riak Commit Hook
 */
riak_commit_hook*
riak_commit_hook_new(riak_config *cfg);

/**
 * @brief Create an array of commit hooks
 * @param cfg Riak Configuration
 * @param array Freshly baked Riak Commit Hooks (out)
 * @param len Number of hooks to bake
 * @returns Error code
 */

riak_error
riak_commit_hook_new_array(riak_config       *cfg,
                           riak_commit_hook ***array,
                           riak_size_t         len);

/**
 * @brief Free memory claimed by an array of commit hooks
 * @param cfg Riak Configuration
 * @param hooks Pointer to array of hooks
 * @param num Number of hooks to free
 */
void
riak_commit_hooks_free(riak_config        *cfg,
                       riak_commit_hook  ***hooks,
                       riak_uint32_t        num);

riak_mod_fun   *riak_commit_hook_get_modfun(riak_commit_hook *hook);
riak_boolean_t  riak_commit_hook_get_has_name(riak_commit_hook *hook);
riak_binary    *riak_commit_hook_get_name(riak_commit_hook *hook);
void riak_commit_hook_set_modfun(riak_commit_hook *hook, riak_mod_fun *value);
void riak_commit_hook_set_name(riak_commit_hook *hook, riak_binary *value);

/**
 * @brief Create an Erlang module/function
 * @param cfg Riak Configuration
 * @returns Freshly baked Riak Mod/Fun
 */
riak_mod_fun*
riak_mod_fun_new(riak_config *cfg);

/**
 * @brief Free memory claimed by a mod/fun
 * @param cfg Riak Configuration
 * @param mod_fun Pointer to mod/fun
 */
void
riak_mod_fun_free(riak_config  *cfg,
                  riak_mod_fun **mod_fun);

riak_binary *riak_mod_fun_get_module(riak_mod_fun *mod_fun);
riak_binary *riak_mod_fun_get_function(riak_mod_fun *mod_fun);
void riak_mod_fun_set_module(riak_mod_fun *mod_fun, riak_binary *value);
void riak_mod_fun_set_function(riak_mod_fun *mod_fun, riak_binary *value);

#endif // RIAK_BUCKET_PROPS_H
