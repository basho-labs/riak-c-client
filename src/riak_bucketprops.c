/*********************************************************************
 *
 * riak_bucketprops.c: Riak C Bucket Properties
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
#include <stdint.h>
#include "riak.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_binary-internal.h"
#include "riak_bucketprops-internal.h"
#include "riak_config-internal.h"
#include "riak_print-internal.h"

static const char* riak_bucketprops_repl[] = {
    "False",
    "Real time",
    "Full-Sync",
    "True"
};

//
// M O D   F U N S
//

riak_modfun*
riak_modfun_new(riak_config *cfg) {
    riak_modfun *fun = (riak_modfun*)(cfg->malloc_fn)(sizeof(riak_modfun));
    if (fun) memset(fun, '\0', sizeof(riak_modfun));
    return fun;
}

riak_modfun*
riak_modfun_deep_new(riak_config *cfg,
                     riak_modfun *from) {
    riak_modfun *fun = riak_modfun_new(cfg);
    if (fun == NULL) {
        return NULL;
    }
    fun->module = riak_binary_deep_new(cfg, from->module);
    if (fun->module == NULL) {
        riak_free(cfg, &fun);
        return NULL;
    }
    fun->function = riak_binary_deep_new(cfg, from->function);
    if (fun->function == NULL) {
        riak_binary_free(cfg, &(fun->module));
        riak_free(cfg, &fun);
        return NULL;
    }

    return fun;
}

static riak_error
riak_modfun_copy_to_pb(riak_config   *cfg,
                       RpbModFun    **pbmod_fun_target,
                       riak_modfun  *mod_fun) {
    if (mod_fun == NULL) {
        return ERIAK_OK;
    }
    RpbModFun *pbmod_fun = (RpbModFun*)(cfg->malloc_fn)(sizeof(RpbModFun));
    if (pbmod_fun == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_binary_to_pb_copy(&(pbmod_fun->module), mod_fun->module);
    riak_binary_to_pb_copy(&(pbmod_fun->function), mod_fun->function);
    // Finally assign the pointer to the list of mod_fun pointers
    *pbmod_fun_target = pbmod_fun;

    return ERIAK_OK;
}

static riak_error
riak_modfun_copy_from_pb(riak_config   *cfg,
                         riak_modfun **mod_fun_target,
                         RpbModFun     *pbmod_fun) {
    riak_modfun *mod_fun = (riak_modfun*)(cfg->malloc_fn)(sizeof(riak_modfun));
    if (pbmod_fun == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    mod_fun->module = riak_binary_populate_from_pb(cfg, &(pbmod_fun->module));
    if (mod_fun->module == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    mod_fun->function = riak_binary_populate_from_pb(cfg, &(pbmod_fun->function));
    if (mod_fun->function == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }

    // Finally assign the pointer to the list of mod_fun pointers
    *mod_fun_target = mod_fun;

    return ERIAK_OK;
}

void
riak_modfun_free_pb(riak_config  *cfg,
                    RpbModFun   **pbmod_fun_target) {
    riak_free(cfg, pbmod_fun_target);
}

void
riak_modfun_free(riak_config   *cfg,
                  riak_modfun **mod_fun_target) {
    riak_modfun *mod_fun = *mod_fun_target;
    riak_free(cfg, &(mod_fun->module));
    riak_free(cfg, &(mod_fun->function));
    riak_free(cfg, mod_fun_target);
}

riak_int32_t
riak_modfun_print_internal(char           *name,
                           riak_modfun   *mod_fun,
                           char          **target,
                           riak_int32_t   *len,
                           riak_int32_t   *total) {

    riak_int32_t wrote = 0;
    wrote += riak_print_label(name, target, len, total);
    wrote += riak_print_binary("Module", mod_fun->module, target, len, total);
    wrote += riak_print_binary("Function", mod_fun->function, target, len, total);
    return wrote;
}

riak_int32_t
riak_modfun_print(riak_modfun  *mod_fun,
                  char         *target,
                  riak_int32_t  len) {
    riak_int32_t total = 0;
    return riak_modfun_print_internal("Mod Fun",
                                       mod_fun,
                                       &target,
                                       &len,
                                       &total);
}

riak_binary*
riak_modfun_get_module(riak_modfun *mod_fun) {
    return mod_fun->module;
}
riak_binary*
riak_modfun_get_function(riak_modfun *mod_fun) {
    return mod_fun->function;
}

riak_error
riak_modfun_set_module(riak_config  *cfg,
                        riak_modfun *mod_fun,
                        riak_binary *value) {
    mod_fun->module = riak_binary_deep_new(cfg, value);
    if (mod_fun->module == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }

    return ERIAK_OK;
}
riak_error
riak_modfun_set_function(riak_config  *cfg,
                          riak_modfun *mod_fun,
                          riak_binary  *value) {
    mod_fun->function = riak_binary_deep_new(cfg, value);
    if (mod_fun->function == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }

    return ERIAK_OK;
}

//
// C O M M I T   H O O K S
//

riak_commit_hook*
riak_commit_hook_new(riak_config *cfg) {
    riak_commit_hook *hook = (riak_commit_hook*)(cfg->malloc_fn)(sizeof(riak_commit_hook));
    if (hook) memset(hook, '\0', sizeof(riak_commit_hook));
    return hook;
}

riak_error
riak_commit_hook_new_array(riak_config        *cfg,
                           riak_commit_hook ***array,
                           riak_size_t         len) {
    riak_commit_hook **result = (riak_commit_hook**)(cfg->malloc_fn)(sizeof(riak_commit_hook)*len);
    if (result == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset((void*)result, '\0', sizeof(riak_commit_hook)*len);
    *array = result;

    return ERIAK_OK;
}

static riak_error
riak_commit_hooks_copy_to_pb(riak_config       *cfg,
                             RpbCommitHook   ***pbhook_target,
                             riak_commit_hook **hook,
                             riak_uint32_t      num_hooks) {
    if (hook == NULL) {
        return ERIAK_OK;
    }
    RpbCommitHook **pbhook = (RpbCommitHook**)(cfg->malloc_fn)(sizeof(RpbCommitHook*) * num_hooks);
    if (pbhook == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i;
    for(i = 0; i < num_hooks; i++) {
        pbhook[i] = (RpbCommitHook*)(cfg->malloc_fn)(sizeof(RpbCommitHook));
        if (pbhook[i] == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        memset(pbhook[i], '\0', sizeof(RpbCommitHook));
        if (hook[i]->has_name) {
            pbhook[i]->has_name = RIAK_TRUE;
            riak_binary_to_pb_copy(&(pbhook[i]->name), hook[i]->name);
        }
        riak_error err = riak_modfun_copy_to_pb(cfg, &(pbhook[i]->modfun), hook[i]->modfun);
        if (err) {
            return err;
        }
    }
    // Finally assign the pointer to the list of commit_hook pointers
    *pbhook_target = pbhook;

    return ERIAK_OK;
}

static riak_error
riak_commit_hooks_copy_from_pb(riak_config         *cfg,
                                riak_commit_hook ***hook_target,
                                RpbCommitHook     **pbhook,
                                riak_uint32_t       num_hooks) {
    riak_commit_hook **hook = (riak_commit_hook**)(cfg->malloc_fn)(sizeof(riak_commit_hook*) * num_hooks);
    if (hook == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i;
    for(i = 0; i < num_hooks; i++) {
        hook[i] = (riak_commit_hook*)(cfg->malloc_fn)(sizeof(riak_commit_hook));
        if (hook[i] == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        memset(hook[i], '\0', sizeof(riak_commit_hook));
        if (pbhook[i]->has_name) {
            hook[i]->has_name = RIAK_TRUE;
            hook[i]->name = riak_binary_populate_from_pb(cfg, &(pbhook[i]->name));
            if (hook[i]->name == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
        }
        riak_error err = riak_modfun_copy_from_pb(cfg, &(hook[i]->modfun), pbhook[i]->modfun);
        if (err) {
            return err;
        }
    }
    // Finally assign the pointer to the list of commit_hook pointers
    *hook_target = hook;

    return ERIAK_OK;
}

riak_int32_t
riak_commit_hooks_print_internal(char              *name,
                                 riak_commit_hook **hooks,
                                 riak_int32_t       num_hooks,
                                 char             **target,
                                 riak_int32_t      *len,
                                 riak_int32_t      *total) {

    char buffer[256];
    riak_int32_t wrote = 0;
    riak_int32_t i;
    for(i = 0; i < num_hooks; i++) {
        snprintf(buffer, sizeof(buffer), "Hook %d", i);
        wrote += riak_print_label(buffer, target, len, total);
        if (hooks[i]->has_name) {
            wrote += riak_print_binary("Name", hooks[i]->name, target, len, total);
        }
        wrote += riak_modfun_print_internal("Mod Fun", hooks[i]->modfun, target, len, total);
    }
    return wrote;
}

static void
riak_commit_hooks_free_pb(riak_config     *cfg,
                          RpbCommitHook ***pbhook_target,
                          riak_uint32_t    num_hooks) {
    RpbCommitHook **pbhook = *pbhook_target;
    int i;
    for(i = 0; i < num_hooks; i++) {
        riak_modfun_free_pb(cfg, &(pbhook[i]->modfun));
        riak_free(cfg, &(pbhook[i]));
    }
    riak_free(cfg, pbhook_target);
}


void
riak_commit_hooks_free(riak_config        *cfg,
                       riak_commit_hook ***hook_target,
                       riak_uint32_t       num_hooks) {
    riak_commit_hook **hook = *hook_target;
    int i;
    for(i = 0; i < num_hooks; i++) {
        riak_free(cfg, &(hook[i]->name));
        riak_modfun_free(cfg, &(hook[i]->modfun));
        riak_free(cfg, &(hook[i]));
    }
    riak_free(cfg, hook_target);
}

riak_modfun*
riak_commit_hook_get_modfun(riak_commit_hook *hook) {
    return hook->modfun;
}
riak_boolean_t
riak_commit_hook_get_has_name(riak_commit_hook *hook) {
    return hook->has_name;
}
riak_binary*
riak_commit_hook_get_name(riak_commit_hook *hook) {
    return hook->name;
}
riak_error
riak_commit_hook_set_modfun(riak_config      *cfg,
                            riak_commit_hook *hook,
                            riak_modfun     *value) {

    hook->modfun = riak_modfun_deep_new(cfg, value);
    if (hook->modfun == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_commit_hook_set_name(riak_config      *cfg,
                          riak_commit_hook *hook,
                          riak_binary      *value) {
    hook->has_name = RIAK_TRUE;
    hook->name = riak_binary_deep_new(cfg, value);
    if (hook->name == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }

    return ERIAK_OK;
}


//
// R I A K   B U C K E T   P R O P E R T I E S
//
riak_bucketprops*
riak_bucketprops_new(riak_config *cfg) {
    riak_bucketprops *pty = (riak_bucketprops*)(cfg->malloc_fn)(sizeof(riak_bucketprops));
    if (pty) memset(pty, '\0', sizeof(riak_bucketprops));
    return pty;
}


riak_error
riak_bucketprops_to_pb_copy(riak_config       *cfg,
                             RpbBucketProps   *to,
                             riak_bucketprops *from) {

    rpb_bucket_props__init(to);

    if (from->has_n_val) {
        to->has_n_val = RIAK_TRUE;
        to->n_val = from->n_val;
    }
    if (from->has_allow_mult) {
        to->has_allow_mult = RIAK_TRUE;
        to->allow_mult = from->allow_mult;
    }
    if (from->has_last_write_wins) {
        to->has_last_write_wins = RIAK_TRUE;
        to->last_write_wins = from->last_write_wins;
    }
    if (from->has_has_precommit) {
        to->has_has_precommit = RIAK_TRUE;
        if (from->has_precommit) {
            to->has_precommit = RIAK_TRUE;
            to->n_precommit = from->n_precommit;
            riak_error err = riak_commit_hooks_copy_to_pb(cfg,
                                                          &(to->precommit),
                                                          from->precommit,
                                                          from->n_precommit);
            if (err) {
                return err;
            }
        }
    }

    if (from->has_has_postcommit) {
        to->has_has_postcommit = RIAK_TRUE;
        if (from->has_postcommit) {
            to->has_postcommit = RIAK_TRUE;
            to->n_postcommit = from->n_postcommit;
            riak_error err = riak_commit_hooks_copy_to_pb(cfg,
                                                          &(to->postcommit),
                                                          from->postcommit,
                                                          from->n_postcommit);
            if (err) {
                return err;
            }
        }
    }
    if (from->has_old_vclock) {
        to->has_old_vclock = RIAK_TRUE;
        to->old_vclock = from->old_vclock;
    }
    if (from->has_young_vclock) {
        to->has_young_vclock = RIAK_TRUE;
        to->young_vclock = from->young_vclock;
    }
    if (from->has_big_vclock) {
        to->has_big_vclock = RIAK_TRUE;
        to->big_vclock = from->big_vclock;
    }
    if (from->has_small_vclock) {
        to->has_small_vclock = RIAK_TRUE;
        to->small_vclock = from->small_vclock;
    }
    if (from->has_pr) {
        to->has_pr = RIAK_TRUE;
        to->pr = from->pr;
    }
    if (from->has_r) {
        to->has_r = RIAK_TRUE;
        to->r = from->r;
    }
    if (from->has_w) {
        to->has_w = RIAK_TRUE;
        to->w = from->w;
    }
    if (from->has_pw) {
        to->has_pw = RIAK_TRUE;
        to->pw = from->pw;
    }
    if (from->has_dw) {
        to->has_dw = RIAK_TRUE;
        to->dw = from->dw;
    }
    if (from->has_rw) {
        to->has_rw = RIAK_TRUE;
        to->rw = from->rw;
    }
    if (from->has_basic_quorum) {
        to->has_basic_quorum = RIAK_TRUE;
        to->basic_quorum = from->basic_quorum;
    }
    if (from->has_notfound_ok) {
        to->has_notfound_ok = RIAK_TRUE;
        to->notfound_ok = from->notfound_ok;
    }
    if (from->has_backend) {
        to->has_backend = RIAK_TRUE;
        riak_binary_to_pb_copy(&(to->backend), from->backend);
    }
    if (from->has_search) {
        to->has_search = RIAK_TRUE;
        to->search = from->search;
    }
    if (from->has_repl) {
        to->has_repl = RIAK_TRUE;
        to->repl = (RpbBucketProps__RpbReplMode)from->repl;
    }
    if (from->has_search_index) {
        to->has_search_index = RIAK_TRUE;
        riak_binary_to_pb_copy(&(to->search_index), from->search_index);
    }

    riak_error err = riak_modfun_copy_to_pb(cfg, &(to->chash_keyfun), from->chash_keyfun);
    if (err) {
        return err;
    }
    err = riak_modfun_copy_to_pb(cfg, &(to->linkfun), from->linkfun);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}


riak_error
riak_bucketprops_new_from_pb(riak_config        *cfg,
                             riak_bucketprops **target,
                             RpbBucketProps     *from) {
    *target = riak_bucketprops_new(cfg);
    if (*target == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_bucketprops *to = *target;

    if (from->has_n_val) {
        to->has_n_val = RIAK_TRUE;
        to->n_val = from->n_val;
    }
    if (from->has_allow_mult) {
        to->has_allow_mult = RIAK_TRUE;
        to->allow_mult = from->allow_mult;
    }
    if (from->has_last_write_wins) {
        to->has_last_write_wins = RIAK_TRUE;
        to->last_write_wins = from->last_write_wins;
    }
    if (from->has_has_precommit) {
        to->has_has_precommit = RIAK_TRUE;
        to->has_precommit = from->has_precommit;
        if (from->has_precommit) {
            to->n_precommit = from->n_precommit;
            riak_error err = riak_commit_hooks_copy_from_pb(cfg,
                                                            &(to->precommit),
                                                            from->precommit,
                                                            from->n_precommit);
            if (err) {
                return err;
            }
        }
    }
    if (from->has_has_postcommit) {
        to->has_has_postcommit = RIAK_TRUE;
        to->has_postcommit = from->has_postcommit;
        if (from->has_postcommit) {
            to->n_postcommit = from->n_postcommit;
            riak_error err = riak_commit_hooks_copy_from_pb(cfg,
                                                            &(to->postcommit),
                                                            from->postcommit,
                                                            from->n_postcommit);
            if (err) {
                return err;
            }
        }
    }
    if (from->has_old_vclock) {
        to->has_old_vclock = RIAK_TRUE;
        to->old_vclock = from->old_vclock;
    }
    if (from->has_young_vclock) {
        to->has_young_vclock = RIAK_TRUE;
        to->young_vclock = from->young_vclock;
    }
    if (from->has_big_vclock) {
        to->has_big_vclock = RIAK_TRUE;
        to->big_vclock = from->big_vclock;
    }
    if (from->has_small_vclock) {
        to->has_small_vclock = RIAK_TRUE;
        to->small_vclock = from->small_vclock;
    }
    if (from->has_pr) {
        to->has_pr = RIAK_TRUE;
        to->pr = from->pr;
    }
    if (from->has_r) {
        to->has_r = RIAK_TRUE;
        to->r = from->r;
    }
    if (from->has_w) {
        to->has_w = RIAK_TRUE;
        to->w = from->w;
    }
    if (from->has_pw) {
        to->has_pw = RIAK_TRUE;
        to->pw = from->pw;
    }
    if (from->has_dw) {
        to->has_dw = RIAK_TRUE;
        to->dw = from->dw;
    }
    if (from->has_rw) {
        to->has_rw = RIAK_TRUE;
        to->rw = from->rw;
    }
    if (from->has_basic_quorum) {
        to->has_basic_quorum = RIAK_TRUE;
        to->basic_quorum = from->basic_quorum;
    }
    if (from->has_notfound_ok) {
        to->has_notfound_ok = RIAK_TRUE;
        to->notfound_ok = from->notfound_ok;
    }
    if (from->has_backend) {
        to->has_backend = RIAK_TRUE;
        to->backend = riak_binary_populate_from_pb(cfg, &(from->backend));
        if (to->backend == NULL) {
            riak_free(cfg, target);
            return ERIAK_OUT_OF_MEMORY;
        }    }
    if (from->has_search) {
        to->has_search = RIAK_TRUE;
        to->search = from->search;
    }
    if (from->has_repl) {
        to->has_repl = RIAK_TRUE;
        to->repl = (riak_bucket_repl_mode)from->repl;
    }
    if (from->has_search_index) {
        to->has_search_index = RIAK_TRUE;
        to->search_index = riak_binary_populate_from_pb(cfg, &(from->search_index));
        if (to->search_index == NULL) {
            riak_free(cfg, target);
            return ERIAK_OUT_OF_MEMORY;
        }
    }

    riak_error err = riak_modfun_copy_from_pb(cfg, &(to->chash_keyfun), from->chash_keyfun);
    if (err) {
        return err;
    }
    err = riak_modfun_copy_from_pb(cfg, &(to->linkfun), from->linkfun);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

// Magic Quorum Values
//
// From riak_pb_kv_codec.hrl:
//%% Quorum value encodings
//-define(UINT_MAX, 16#ffffffff).
//-define(RIAKPB_RW_ONE, ?UINT_MAX-1).
//-define(RIAKPB_RW_QUORUM, ?UINT_MAX-2).
//-define(RIAKPB_RW_ALL, ?UINT_MAX-3).
//-define(RIAKPB_RW_DEFAULT, ?UINT_MAX-4).
//
// Riak uses magic numbers to encode quorum

#define RIAK_MAX_QUORUM     4

static const char* riak_bucketprops_quorum_decoder[] = {
    "unknown",
    "one",
    "quorum",
    "all",
    "default"
};

char*
riak_bucketprops_quorum(riak_uint32_t q) {
     riak_int32_t offset = UINT32_MAX - q;
     if ((offset < 0) || (offset >= RIAK_MAX_QUORUM)) offset = 0;
     return (char*)riak_bucketprops_quorum_decoder[offset];
}

riak_int32_t
riak_bucketprops_print(riak_bucketprops *prop,
                        char            *target,
                        riak_int32_t     len) {
    riak_int32_t total = 0;

    if (prop->has_n_val) {
        riak_print_int("N", prop->n_val, &target, &len, &total);
    }
    if (prop->has_allow_mult) {
        riak_print_bool("Allow Multiple", prop->allow_mult, &target, &len, &total);
    }
    if (prop->has_last_write_wins) {
        riak_print_bool("LWW", prop->last_write_wins, &target, &len, &total);
    }

    if (prop->has_has_precommit && prop->has_precommit){
        riak_print_int("# Precommit Hooks", prop->n_precommit, &target, &len, &total);
        riak_commit_hooks_print_internal("Precommit Hooks",
                                         prop->precommit,
                                         prop->n_precommit,
                                         &target,
                                         &len,
                                         &total);
    }
    if (prop->has_has_postcommit && prop->has_postcommit) {
        riak_print_int("# Postcommit Hooks", prop->n_postcommit, &target, &len, &total);
        riak_commit_hooks_print_internal("Postcommit Hooks",
                                         prop->postcommit,
                                         prop->n_postcommit,
                                         &target,
                                         &len,
                                         &total);
    }

    if (prop->has_old_vclock) {
        riak_print_int("Old Vclock", prop->old_vclock, &target, &len, &total);
    }
    if (prop->has_young_vclock) {
        riak_print_int("Young Vclock", prop->young_vclock, &target, &len, &total);
    }
    if (prop->has_big_vclock) {
        riak_print_int("Big Vclock", prop->big_vclock, &target, &len, &total);
    }
    if (prop->has_small_vclock) {
        riak_print_int("Small Vclock", prop->small_vclock, &target, &len, &total);
    }
    if (prop->has_pr) {
        riak_print_string("PR", riak_bucketprops_quorum(prop->pr), &target, &len, &total);
    }
    if (prop->has_r) {
        riak_print_string("R", riak_bucketprops_quorum(prop->r), &target, &len, &total);
    }
    if (prop->has_w) {
        riak_print_string("W", riak_bucketprops_quorum(prop->w), &target, &len, &total);
    }
    if (prop->has_pw) {
        riak_print_string("PW", riak_bucketprops_quorum(prop->pw), &target, &len, &total);
    }
    if (prop->has_dw) {
        riak_print_string("DW", riak_bucketprops_quorum(prop->dw), &target, &len, &total);
    }
    if (prop->has_rw) {
        riak_print_string("RW", riak_bucketprops_quorum(prop->rw), &target, &len, &total);
    }
    if (prop->has_basic_quorum) {
        riak_print_bool("Basic Quorum", prop->basic_quorum, &target, &len, &total);
    }
    if (prop->has_notfound_ok) {
        riak_print_bool("Not Found OK", prop->notfound_ok, &target, &len, &total);
    }
    if (prop->has_backend) {
        riak_print_binary("Backend", prop->backend, &target, &len, &total);
    }
    if (prop->has_search) {
        riak_print_bool("Search", prop->search, &target, &len, &total);
    }
    if (prop->repl >= RIAK_BUCKETPROPS_REPL_FALSE &&
        prop->repl <= RIAK_BUCKETPROPS_REPL_TRUE) {
        riak_print_string("Repl: ", (char*)riak_bucketprops_repl[prop->repl],
                          &target,
                          &len,
                          &total);
    }
    if (prop->has_search_index) {
        riak_print_binary("YZ Index", prop->search_index, &target, &len, &total);
    }
    riak_modfun_print_internal("C Hash Key Fun", prop->chash_keyfun, &target, &len, &total);
    riak_modfun_print_internal("Link Fun", prop->linkfun, &target, &len, &total);

    return total;
}

void
riak_bucketprops_free(riak_config       *cfg,
                      riak_bucketprops **props) {
    riak_bucketprops* prop = *props;
    riak_free(cfg, &(prop->backend));
    riak_modfun_free(cfg, &(prop->chash_keyfun));
    riak_modfun_free(cfg, &(prop->linkfun));
    riak_commit_hooks_free(cfg, &(prop->precommit), prop->n_precommit);
    riak_commit_hooks_free(cfg, &(prop->postcommit), prop->n_postcommit);
    riak_free(cfg, &(prop->search_index));
    riak_free(cfg, props);
}

void
riak_bucketprops_free_pb(riak_config     *cfg,
                         RpbBucketProps **props) {
    RpbBucketProps *prop = *props;
    riak_modfun_free_pb(cfg, &(prop->chash_keyfun));
    riak_modfun_free_pb(cfg, &(prop->linkfun));
    riak_commit_hooks_free_pb(cfg, &(prop->precommit), prop->n_precommit);
    riak_commit_hooks_free_pb(cfg, &(prop->postcommit), prop->n_postcommit);
    riak_free(cfg, props);
}

//
// ACCESSORS
//
riak_boolean_t
riak_bucketprops_get_has_n_val(riak_bucketprops *prop) {
    return prop->has_n_val;
}
riak_uint32_t
riak_bucketprops_get_n_val(riak_bucketprops *prop) {
    return prop->n_val;
}
riak_boolean_t
riak_bucketprops_get_has_allow_mult(riak_bucketprops *prop) {
    return prop->has_allow_mult;
}
riak_boolean_t
riak_bucketprops_get_allow_mult(riak_bucketprops *prop) {
    return prop->allow_mult;
}
riak_boolean_t
riak_bucketprops_get_has_last_write_wins(riak_bucketprops *prop) {
    return prop->has_last_write_wins;
}
riak_boolean_t
riak_bucketprops_get_last_write_wins(riak_bucketprops *prop) {
    return prop->last_write_wins;
}
riak_size_t
riak_bucketprops_get_n_precommit(riak_bucketprops *prop) {
    return prop->n_precommit;
}
riak_commit_hook**
riak_bucketprops_get_precommit(riak_bucketprops *prop) {
    return prop->precommit;
}
riak_boolean_t
riak_bucketprops_get_has_precommit(riak_bucketprops *prop) {
    return prop->has_has_precommit && prop->has_precommit;
}
riak_size_t
riak_bucketprops_get_n_postcommit(riak_bucketprops *prop) {
    return prop->n_postcommit;
}
riak_commit_hook**
riak_bucketprops_get_postcommit(riak_bucketprops *prop) {
    return prop->postcommit;
}
riak_boolean_t
riak_bucketprops_get_has_postcommit(riak_bucketprops *prop) {
    return prop->has_has_postcommit && prop->has_postcommit;
}
riak_modfun*
riak_bucketprops_get_chash_keyfun(riak_bucketprops *prop) {
    return prop->chash_keyfun;
}
riak_modfun*
riak_bucketprops_get_linkfun(riak_bucketprops *prop) {
    return prop->linkfun;
}
riak_boolean_t
riak_bucketprops_get_has_old_vclock(riak_bucketprops *prop) {
    return prop->has_old_vclock;
}
riak_uint32_t
riak_bucketprops_get_old_vclock(riak_bucketprops *prop) {
    return prop->old_vclock;
}
riak_boolean_t
riak_bucketprops_get_has_young_vclock(riak_bucketprops *prop) {
    return prop->has_young_vclock;
}
riak_uint32_t
riak_bucketprops_get_young_vclock(riak_bucketprops *prop) {
    return prop->young_vclock;
}
riak_boolean_t
riak_bucketprops_get_has_big_vclock(riak_bucketprops *prop) {
    return prop->has_big_vclock;
}
riak_uint32_t
riak_bucketprops_get_big_vclock(riak_bucketprops *prop) {
    return prop->big_vclock;
}
riak_boolean_t
riak_bucketprops_get_has_small_vclock(riak_bucketprops *prop) {
    return prop->has_small_vclock;
}
riak_uint32_t
riak_bucketprops_get_small_vclock(riak_bucketprops *prop) {
    return prop->small_vclock;
}
riak_boolean_t
riak_bucketprops_get_has_pr(riak_bucketprops *prop) {
    return prop->has_pr;
}
riak_uint32_t
riak_bucketprops_get_pr(riak_bucketprops *prop) {
    return prop->pr;
}
riak_boolean_t
riak_bucketprops_get_has_r(riak_bucketprops *prop) {
    return prop->has_r;
}
riak_uint32_t
riak_bucketprops_get_r(riak_bucketprops *prop) {
    return prop->r;
}
riak_boolean_t
riak_bucketprops_get_has_w(riak_bucketprops *prop) {
    return prop->has_w;
}
riak_uint32_t
riak_bucketprops_get_w(riak_bucketprops *prop) {
    return prop->w;
}
riak_boolean_t
riak_bucketprops_get_has_pw(riak_bucketprops *prop) {
    return prop->has_pw;
}
riak_uint32_t
riak_bucketprops_get_pw(riak_bucketprops *prop) {
    return prop->pw;
}
riak_boolean_t
riak_bucketprops_get_has_dw(riak_bucketprops *prop) {
    return prop->has_dw;
}
riak_uint32_t
riak_bucketprops_get_dw(riak_bucketprops *prop) {
    return prop->dw;
}
riak_boolean_t
riak_bucketprops_get_has_rw(riak_bucketprops *prop) {
    return prop->has_rw;
}
riak_uint32_t
riak_bucketprops_get_rw(riak_bucketprops *prop) {
    return prop->rw;
}
riak_boolean_t
riak_bucketprops_get_has_basic_quorum(riak_bucketprops *prop) {
    return prop->has_basic_quorum;
}
riak_boolean_t
riak_bucketprops_get_basic_quorum(riak_bucketprops *prop) {
    return prop->basic_quorum;
}
riak_boolean_t
riak_bucketprops_get_has_notfound_ok(riak_bucketprops *prop) {
    return prop->has_notfound_ok;
}
riak_boolean_t
riak_bucketprops_get_notfound_ok(riak_bucketprops *prop) {
    return prop->notfound_ok;
}
riak_boolean_t
riak_bucketprops_get_has_backend(riak_bucketprops *prop) {
    return prop->has_backend;
}
riak_binary*
riak_bucketprops_get_backend(riak_bucketprops *prop) {
    return prop->backend;
}
riak_boolean_t
riak_bucketprops_get_has_search(riak_bucketprops *prop) {
    return prop->has_search;
}
riak_boolean_t
riak_bucketprops_get_search(riak_bucketprops *prop) {
    return prop->search;
}
riak_boolean_t
riak_bucketprops_get_has_repl(riak_bucketprops *prop) {
    return prop->has_repl;
}
riak_bucket_repl_mode
riak_bucketprops_get_repl(riak_bucketprops *prop) {
    return prop->repl;
}
riak_boolean_t
riak_bucketprops_get_has_search_index(riak_bucketprops *prop) {
    return prop->has_search_index;
}
riak_binary*
riak_bucketprops_get_search_index(riak_bucketprops *prop) {
    return prop->search_index;
}
void
riak_bucketprops_set_n_val(riak_bucketprops *prop,
                           riak_uint32_t     value) {
    prop->has_n_val = RIAK_TRUE;
    prop->n_val = value;
}
void
riak_bucketprops_set_allow_mult(riak_bucketprops *prop,
                                riak_boolean_t    value) {
    prop->has_allow_mult = RIAK_TRUE;
    prop->allow_mult = value;
}
void
riak_bucketprops_set_last_write_wins(riak_bucketprops *prop,
                                     riak_boolean_t    value) {
    prop->has_last_write_wins = RIAK_TRUE;
    prop->last_write_wins = value;
}
void
riak_bucketprops_set_precommits(riak_bucketprops  *prop,
                                riak_commit_hook **precommit,
                                riak_size_t        num) {
    prop->has_has_precommit = RIAK_TRUE;
    prop->has_precommit = RIAK_TRUE;
    prop->n_precommit = num;
    prop->precommit = precommit;
}
void
riak_bucketprops_set_postcommits(riak_bucketprops  *prop,
                                 riak_commit_hook **postcommit,
                                 riak_size_t        num) {
    prop->has_has_postcommit = RIAK_TRUE;
    prop->has_postcommit = RIAK_TRUE;
    prop->n_postcommit = num;
    prop->postcommit = postcommit;
}
riak_error
riak_bucketprops_set_chash_keyfun(riak_config      *cfg,
                                  riak_bucketprops *prop,
                                  riak_modfun      *value) {
    prop->chash_keyfun = riak_modfun_deep_new(cfg, value);
    if (prop->chash_keyfun) {
        return ERIAK_OUT_OF_MEMORY;
    }

    return ERIAK_OK;
}
riak_error
riak_bucketprops_set_linkfun(riak_config      *cfg,
                             riak_bucketprops *prop,
                             riak_modfun      *value) {
    prop->linkfun = riak_modfun_deep_new(cfg, value);
    if (prop->linkfun) {
        return ERIAK_OUT_OF_MEMORY;
    }

    return ERIAK_OK;
}
void
riak_bucketprops_set_old_vclock(riak_bucketprops *prop,
                                riak_uint32_t     value) {
    prop->has_old_vclock = RIAK_TRUE;
    prop->old_vclock = value;
}
void
riak_bucketprops_set_young_vclock(riak_bucketprops *prop,
                                  riak_uint32_t     value) {
    prop->has_young_vclock = RIAK_TRUE;
    prop->young_vclock = value;
}
void
riak_bucketprops_set_big_vclock(riak_bucketprops *prop,
                                riak_uint32_t     value) {
    prop->has_big_vclock = RIAK_TRUE;
    prop->big_vclock = value;
}
void
riak_bucketprops_set_small_vclock(riak_bucketprops *prop,
                                  riak_uint32_t     value) {
    prop->has_small_vclock = RIAK_TRUE;
    prop->small_vclock = value;
}
void
riak_bucketprops_set_pr(riak_bucketprops *prop,
                        riak_uint32_t     value) {
    prop->has_pr = RIAK_TRUE;
    prop->pr = value;
}
void
riak_bucketprops_set_r(riak_bucketprops *prop,
                       riak_uint32_t     value) {
    prop->has_r = RIAK_TRUE;
    prop->r = value;
}
void
riak_bucketprops_set_w(riak_bucketprops *prop,
                       riak_uint32_t     value) {
    prop->has_w = RIAK_TRUE;
    prop->w = value;
}
void
riak_bucketprops_set_pw(riak_bucketprops *prop,
                        riak_uint32_t     value) {
    prop->has_pw = RIAK_TRUE;
    prop->pw = value;
}
void
riak_bucketprops_set_dw(riak_bucketprops *prop,
                        riak_uint32_t     value) {
    prop->has_dw = RIAK_TRUE;
    prop->dw = value;
}
void
riak_bucketprops_set_rw(riak_bucketprops *prop,
                        riak_uint32_t     value) {
    prop->has_rw = RIAK_TRUE;
    prop->rw = value;
}
void
riak_bucketprops_set_basic_quorum(riak_bucketprops *prop,
                                  riak_boolean_t    value) {
    prop->has_basic_quorum = RIAK_TRUE;
    prop->basic_quorum = value;
}
void
riak_bucketprops_set_notfound_ok(riak_bucketprops *prop,
                                 riak_boolean_t    value) {
    prop->has_notfound_ok = RIAK_TRUE;
    prop->notfound_ok = value;
}
void
riak_bucketprops_set_backend(riak_bucketprops *prop,
                             riak_binary      *value) {
    prop->has_backend = RIAK_TRUE;
    prop->backend = value;
}
void
riak_bucketprops_set_search(riak_bucketprops *prop,
                            riak_boolean_t    value) {
    prop->has_search = RIAK_TRUE;
    prop->search = value;
}
void
riak_bucketprops_set_repl(riak_bucketprops     *prop,
                          riak_bucket_repl_mode value) {
    prop->has_repl = RIAK_TRUE;
    prop->repl = value;
}
void
riak_bucketprops_set_search_index(riak_bucketprops *prop,
                                  riak_binary      *value) {
    prop->has_search_index = RIAK_TRUE;
    prop->search_index = value;
}
