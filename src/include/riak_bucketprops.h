/*********************************************************************
 *
 * riak_bucketprops.h: Riak C Client Bucket Properties
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

#ifndef _RIAK_BUCKETPROPS_H
#define _RIAK_BUCKETPROPS_H

#ifdef __cplusplus
extern "C" {
#endif
typedef enum _riak_bucket_repl_mode {
    RIAK_BUCKETPROPS_REPL_FALSE = 0,
    RIAK_BUCKETPROPS_REPL_REALTIME = 1,
    RIAK_BUCKETPROPS_REPL_FULLSYNC = 2,
    RIAK_BUCKETPROPS_REPL_TRUE = 3
} riak_bucket_repl_mode;

typedef struct _riak_modfun riak_modfun;
typedef struct _riak_commit_hook riak_commit_hook;
typedef struct _riak_bucketprops riak_bucketprops;

/**
 * @brief Construct a new Riak Bucket Property object
 * @param cfg Riak Configuration
 * @return Riak Bucket Property object
 */
riak_bucketprops*
riak_bucketprops_new(riak_config *cfg);

/**
 * @brief Release claimed memory used by a Riak Bucket Property object
 * @param cfg Riak Configuration
 * @param props Riak Bucket Property object to be freed
 */
void
riak_bucketprops_free(riak_config        *cfg,
                       riak_bucketprops **props);

/**
 * @brief Print contents of a Riak Bucket Property object to a string
 * @param state Riak Print State
 * @param props Bucket Property object to print
 * @return Number of bytes written
 */
riak_int32_t
riak_bucketprops_print(riak_print_state *state,
                       riak_bucketprops *prop);


/**
 * @brief Determine if N Val has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if N Val has been set
 */
riak_boolean_t
riak_bucketprops_get_has_n_val(riak_bucketprops *prop);

/**
 * @brief Access the N Val in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns N Val
 */
riak_uint32_t
riak_bucketprops_get_n_val(riak_bucketprops *prop);

/**
 * @brief Determine if the Allow Multiple flag has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Allow Multiple flag has been set
 */
riak_boolean_t
riak_bucketprops_get_has_allow_mult(riak_bucketprops *prop);

/**
 * @brief Access the Allow Multiple flag in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns The Allow Multiple flag
 */
riak_boolean_t
riak_bucketprops_get_allow_mult(riak_bucketprops *prop);

/**
 * @brief Determine if the Last Writer Wins flag has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Last Writer Wins flag has been set
 */
riak_boolean_t
riak_bucketprops_get_has_last_write_wins(riak_bucketprops *prop);

/**
 * @brief Access the Last Writer Wins flag in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns The Last Writer Wins flag
 */
riak_boolean_t
riak_bucketprops_get_last_write_wins(riak_bucketprops *prop);

/**
 * @brief Determine if Precommit Hooks have been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Precommit Hooks have been set
 */
riak_boolean_t
riak_bucketprops_get_has_precommit(riak_bucketprops *prop);

/**
 * @brief Access the number of Precommit Hooks in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns The number of Precommit Hooks
 */
riak_size_t
riak_bucketprops_get_n_precommit(riak_bucketprops *prop);

/**
 * @brief Access the Precommit Hooks in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns The Precommit Hooks
 */
riak_commit_hook**
riak_bucketprops_get_precommit(riak_bucketprops *prop);

/**
 * @brief Determine if Postcommit Hooks have been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Postcommit Hooks have been set
 */
riak_boolean_t
riak_bucketprops_get_has_postcommit(riak_bucketprops *prop);

/**
 * @brief Access the Number of Postcommit Hooks in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Number of Postcommit Hooks
 */
riak_size_t
riak_bucketprops_get_n_postcommit(riak_bucketprops *prop);

/**
 * @brief Access the Postcommit Hooks in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Postcommit Hooks
 */
riak_commit_hook**
riak_bucketprops_get_postcommit(riak_bucketprops *prop);

/**
 * @brief Access the C-Hash Key/Erlang Function in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns C-Hash Key/Erlang Function
 */
riak_modfun*
riak_bucketprops_get_chash_keyfun(riak_bucketprops *prop);

/**
 * @brief Access the Link Walking Erlang Function in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Link Walking Erlang Function
 */
riak_modfun*
riak_bucketprops_get_linkfun(riak_bucketprops *prop);

/**
 * @brief Determine if Oldest Vector Clock has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Oldest Vector Clock has been set
 */
riak_boolean_t
riak_bucketprops_get_has_old_vclock(riak_bucketprops *prop);

/**
 * @brief Access the Oldest Vector Clock in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Oldest Vector Clock
 */
riak_uint32_t
riak_bucketprops_get_old_vclock(riak_bucketprops *prop);

/**
 * @brief Determine if Youngest Vector Clock has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Youngest Vector Clock has been set
 */
riak_boolean_t
riak_bucketprops_get_has_young_vclock(riak_bucketprops *prop);

/**
 * @brief Access the Youngest Vector Clock in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Youngest Vector Clock
 */
riak_uint32_t
riak_bucketprops_get_young_vclock(riak_bucketprops *prop);

/**
 * @brief Determine if Biggest Vector Clock has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Biggest Vector Clock has been set
 */
riak_boolean_t
riak_bucketprops_get_has_big_vclock(riak_bucketprops *prop);

/**
 * @brief Access the Biggest Vector Clock in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Biggest Vector Clock
 */
riak_uint32_t
riak_bucketprops_get_big_vclock(riak_bucketprops *prop);

/**
 * @brief Determine if Smallest Vector Clock has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Smallest Vector Clock has been set
 */
riak_boolean_t
riak_bucketprops_get_has_small_vclock(riak_bucketprops *prop);

/**
 * @brief Access the Smallest Vector Clock in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Smallest Vector Clock
 */
riak_uint32_t
riak_bucketprops_get_small_vclock(riak_bucketprops *prop);

/**
 * @brief Determine if the Primary Read Quorum has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Primary Read Quorum has been set
 */
riak_boolean_t
riak_bucketprops_get_has_pr(riak_bucketprops *prop);

/**
 * @brief Access the Primary Read Quorum in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Primary Read Quorum
 */
riak_uint32_t
riak_bucketprops_get_pr(riak_bucketprops *prop);

/**
 * @brief Determine if the Read Quorum has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Read Quorum has been set
 */
riak_boolean_t
riak_bucketprops_get_has_r(riak_bucketprops *prop);

/**
 * @brief Determine if the Read Quorum has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Read Quorum has been set
 */
riak_uint32_t
riak_bucketprops_get_r(riak_bucketprops *prop);

/**
 * @brief Determine if the Write Quorum has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Write Quorum has been set
 */
riak_boolean_t
riak_bucketprops_get_has_w(riak_bucketprops *prop);

/**
 * @brief Access the Write Quorum in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns The Write Quorum
 */
riak_uint32_t
riak_bucketprops_get_w(riak_bucketprops *prop);

/**
 * @brief Determine if the Primary Write Quorum has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Primary Write Quorum has been set
 */
riak_boolean_t
riak_bucketprops_get_has_pw(riak_bucketprops *prop);

/**
 * @brief Access the Primary Write Quorum in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns The Primary Write Quorum
 */
riak_uint32_t
riak_bucketprops_get_pw(riak_bucketprops *prop);

/**
 * @brief Determine if the Durable Write Quorum has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Durable Write Quorum has been set
 */
riak_boolean_t
riak_bucketprops_get_has_dw(riak_bucketprops *prop);

/**
 * @brief Access the Durable Write Quorum in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns The Durable Write Quorum
 */
riak_uint32_t
riak_bucketprops_get_dw(riak_bucketprops *prop);

/**
 * @brief Determine if the Read/Write Quorum has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Read/Write Quorum has been set
 */
riak_boolean_t
riak_bucketprops_get_has_rw(riak_bucketprops *prop);

/**
 * @brief Access the Read/Write Quorum in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns The Read/Write Quorum
 */
riak_uint32_t
riak_bucketprops_get_rw(riak_bucketprops *prop);

/**
 * @brief Determine if the Basic Quorum has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if the Basic Quorum has been set
 */
riak_boolean_t
riak_bucketprops_get_has_basic_quorum(riak_bucketprops *prop);

/**
 * @brief Access the Basic Quorum in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns The Basic Quorum
 */
riak_boolean_t
riak_bucketprops_get_basic_quorum(riak_bucketprops *prop);

/**
 * @brief Determine if Not Found OK flag has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Not Found OK has been set
 */
riak_boolean_t
riak_bucketprops_get_has_notfound_ok(riak_bucketprops *prop);

/**
 * @brief Access the Not Found OK flag in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Not Found OK flag
 */
riak_boolean_t
riak_bucketprops_get_notfound_ok(riak_bucketprops *prop);

/**
 * @brief Determine if Backend type has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Backend type has been set
 */
riak_boolean_t
riak_bucketprops_get_has_backend(riak_bucketprops *prop);

/**
 * @brief Access the Backend type in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Backend type
 */
riak_binary*
riak_bucketprops_get_backend(riak_bucketprops *prop);

/**
 * @brief Determine if Legacy Riak Search has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Legacy Riak Search has been set
 */
riak_boolean_t
riak_bucketprops_get_has_search(riak_bucketprops *prop);

/**
 * @brief Access the Legacy Riak Search setting in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Legacy Riak Search setting
 */
riak_boolean_t
riak_bucketprops_get_search(riak_bucketprops *prop);

/**
 * @brief Determine if Replication has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if Replication has been set
 */
riak_boolean_t
riak_bucketprops_get_has_repl(riak_bucketprops *prop);

/**
 * @brief Access the Replication flag in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns Replication flag
 */
riak_bucket_repl_mode
riak_bucketprops_get_repl(riak_bucketprops *prop);

/**
 * @brief Determine if has been set on Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns True if has been set
 */
riak_boolean_t
riak_bucketprops_get_has_search_index(riak_bucketprops *prop);

/**
 * @brief Access the in Bucket Properties
 * @param prop Riak Bucket Properties
 * @returns
 */
riak_binary*
riak_bucketprops_get_search_index(riak_bucketprops *prop);

/**
 * @brief Set the N Val
 * @param prop Riak Bucket Properties
 * @param value N Val
 */
void
riak_bucketprops_set_n_val(riak_bucketprops *prop,
                           riak_uint32_t     value);

/**
 * @brief Set the Allow Multiples flag
 * @param prop Riak Bucket Properties
 * @param value Allow Multiples flag
 */
void
riak_bucketprops_set_allow_mult(riak_bucketprops *prop,
                                riak_boolean_t    value);

/**
 * @brief Set the Last Writer Wins flag
 * @param prop Riak Bucket Properties
 * @param value Last Writer Wins flag
 */
void
riak_bucketprops_set_last_write_wins(riak_bucketprops *prop,
                                     riak_boolean_t    value);

/**
 * @brief Set the Precommit Hooks
 * @param prop Riak Bucket Properties
 * @param value Array of Precommit Hooks
 * @param num Quanity of Hooks
 */
void
riak_bucketprops_set_precommits(riak_bucketprops  *prop,
                                riak_commit_hook **precommit,
                                riak_size_t        num);

/**
 * @brief Set the Postcommit Hooks
 * @param prop Riak Bucket Properties
 * @param value Array of Postcommit Hooks
 * @param num Quanity of Hooks
 */
void
riak_bucketprops_set_postcommits(riak_bucketprops  *prop,
                                 riak_commit_hook **postcommit,
                                 riak_size_t        num);

/**
 * @brief Set the C-Hash Key Erlang Function
 * @param cfg Riak Configuration
 * @param prop Riak Bucket Properties
 * @param value C-Hash Key Erlang Function
 * @returns Error code
 */
riak_error
riak_bucketprops_set_chash_keyfun(riak_config      *cfg,
                                  riak_bucketprops *prop,
                                  riak_modfun      *value);

/**
 * @brief Set the Link Walking Erlang Function
 * @param cfg Riak Configuration
 * @param prop Riak Bucket Properties
 * @param value Link Walking Erlang Function
 * @returns Error code
 */
riak_error
riak_bucketprops_set_linkfun(riak_config      *cfg,
                             riak_bucketprops *prop,
                             riak_modfun      *value);

/**
 * @brief Set the Oldest Vector Clock
 * @param prop Riak Bucket Properties
 * @param value Oldest Vector Clock
 */
void
riak_bucketprops_set_old_vclock(riak_bucketprops *prop,
                                riak_uint32_t     value);

/**
 * @brief Set the Youngest Vector Clock
 * @param prop Riak Bucket Properties
 * @param value Youngest Vector Clock
 */
void
riak_bucketprops_set_young_vclock(riak_bucketprops *prop,
                                  riak_uint32_t     value);

/**
 * @brief Set the Biggest Vector Clock
 * @param prop Riak Bucket Properties
 * @param value Biggest Vector Clock
 */
void
riak_bucketprops_set_big_vclock(riak_bucketprops *prop,
                                riak_uint32_t     value);

/**
 * @brief Set the Smallest Vector Clock
 * @param prop Riak Bucket Properties
 * @param value Smallest Vector Clock
 */
void
riak_bucketprops_set_small_vclock(riak_bucketprops *prop,
                                  riak_uint32_t     value);

/**
 * @brief Set the Primary Read Quorm
 * @param prop Riak Bucket Properties
 * @param value Primary Read Quorm
 */
void
riak_bucketprops_set_pr(riak_bucketprops *prop,
                        riak_uint32_t     value);

/**
 * @brief Set the Read Quorm
 * @param prop Riak Bucket Properties
 * @param value Read Quorm
 */
void
riak_bucketprops_set_r(riak_bucketprops *prop,
                       riak_uint32_t     value);

/**
 * @brief Set the Write Quorum
 * @param prop Riak Bucket Properties
 * @param value Write Quorum
 */
void
riak_bucketprops_set_w(riak_bucketprops *prop,
                       riak_uint32_t     value);

/**
 * @brief Set the Primary Write Quorum
 * @param prop Riak Bucket Properties
 * @param value Primary Write Quorum
 */
void
riak_bucketprops_set_pw(riak_bucketprops *prop,
                        riak_uint32_t     value);

/**
 * @brief Set the Durable Write Quorum
 * @param prop Riak Bucket Properties
 * @param value Durable Write Quorum
 */
void
riak_bucketprops_set_dw(riak_bucketprops *prop,
                        riak_uint32_t     value);

/**
 * @brief Set the Read/Write Quorum
 * @param prop Riak Bucket Properties
 * @param value Read/Write Quorum
 */
void
riak_bucketprops_set_rw(riak_bucketprops *prop,
                        riak_uint32_t     value);

/**
 * @brief Set the Basic Quorum flag
 * @param prop Riak Bucket Properties
 * @param value Basic Quorum flag
 */
void
riak_bucketprops_set_basic_quorum(riak_bucketprops *prop,
                                  riak_boolean_t    value);

/**
 * @brief Set the Not Found OK flag
 * @param prop Riak Bucket Properties
 * @param value Not Found OK flag
 */
void
riak_bucketprops_set_notfound_ok(riak_bucketprops *prop,
                                 riak_boolean_t    value);

/**
 * @brief Set the Backend
 * @param prop Riak Bucket Properties
 * @param value Backend
 */
void
riak_bucketprops_set_backend(riak_bucketprops *prop,
                             riak_binary      *value);

/**
 * @brief Set the Legacy Riak Search flag
 * @param prop Riak Bucket Properties
 * @param value Legacy Riak Search flag
 */
void
riak_bucketprops_set_search(riak_bucketprops *prop,
                            riak_boolean_t    value);

/**
 * @brief Set the Replication Mode
 * @param prop Riak Bucket Properties
 * @param value Replication Mode
 */
void
riak_bucketprops_set_repl(riak_bucketprops     *prop,
                          riak_bucket_repl_mode value);

/**
 * @brief Set the Search Index
 * @param prop Riak Bucket Properties
 * @param value Search Index
 */
void
riak_bucketprops_set_search_index(riak_bucketprops *prop,
                                  riak_binary      *value);

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
riak_commit_hooks_free(riak_config         *cfg,
                       riak_commit_hook  ***hooks,
                       riak_uint32_t        num);

/**
 * @brief Access the Erlang Module/Function
 * @param hook Riak Commit Hook
 * @param value Erlang Module/Function
 */
riak_modfun*
riak_commit_hook_get_modfun(riak_commit_hook *hook);

/**
 * @brief Determine if the Commit Hook Name has been set
 * @param hook Riak Commit Hook
 * @param value True if the Commit Hook Name has been set
 */
riak_boolean_t
riak_commit_hook_get_has_name(riak_commit_hook *hook);

/**
 * @brief Access the Commit Hook Name
 * @param hook Riak Commit Hook
 * @param value Commit Hook Name
 */
riak_binary*
riak_commit_hook_get_name(riak_commit_hook *hook);

/**
 * @brief Set the Erlang Module/Function
 * @param cfg Riak Configuration
 * @param hook Riak Commit Hook
 * @param value Erlang Module/Function
 */
riak_error
riak_commit_hook_set_modfun(riak_config      *cfg,
                            riak_commit_hook *hook,
                            riak_modfun     *value);

/**
 * @brief Set the Commit Hook Name
 * @param cfg Riak Configuration
 * @param hook Riak Commit Hook
 * @param value Commit Hook Name
 */
riak_error
riak_commit_hook_set_name(riak_config      *cfg,
                          riak_commit_hook *hook,
                          riak_binary      *value);

/**
 * @brief Create an Erlang module/function
 * @param cfg Riak Configuration
 * @returns Freshly baked Riak Mod/Fun
 */
riak_modfun*
riak_modfun_new(riak_config *cfg);

/**
 * @brief Create an Erlang module/function from another one
 * @param cfg Riak Configuration
 * @param from Source Riak Module/Function
 * @returns Freshly baked Riak Mod/Fun
 */
riak_modfun*
riak_modfun_deep_new(riak_config *cfg,
                     riak_modfun *from);
/**
 * @brief Free memory claimed by a mod/fun
 * @param cfg Riak Configuration
 * @param mod_fun Pointer to mod/fun
 */
void
riak_modfun_free(riak_config  *cfg,
                 riak_modfun **mod_fun);

/**
 * @brief Access the name of the Module
 * @param mod_fun Riak Erlang Module/Function
 * @returns Name of the Module
 */
riak_binary*
riak_modfun_get_module(riak_modfun *mod_fun);

/**
 * @brief Access the name of the Function
 * @param mod_fun Riak Erlang Module/Function
 * @returns Name of the Function
 */
riak_binary*
riak_modfun_get_function(riak_modfun *mod_fun);

/**
 * @brief Set the Erlang Module
 * @param cfg Riak Configuration
 * @param mod_fun Riak Module/Function
 * @param value Erlang Module
 */
riak_error
riak_modfun_set_module(riak_config *cfg,
                       riak_modfun *mod_fun,
                       riak_binary *value);

/**
 * @brief Set the Erlang Function
 * @param cfg Riak Configuration
 * @param mod_fun Riak Module/Function
 * @param value Erlang Function
 */
riak_error
riak_modfun_set_function(riak_config  *cfg,
                         riak_modfun *mod_fun,
                         riak_binary  *value);

#ifdef __cplusplus
}
#endif

#endif // _RIAK_BUCKETPROPS_H
