/*********************************************************************
 *
 * riak_bucket_props.c: Riak C Bucket Properties
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
#include <stdint.h>
#include "riak.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_binary-internal.h"
#include "riak_bucket_props-internal.h"
#include "riak_config-internal.h"
#include "riak_print-internal.h"

static const char* riak_bucket_props_repl[] = {
    "False",
    "Real time",
    "Full-Sync",
    "True"
};

//
// M O D   F U N S
//

riak_mod_fun*
riak_mod_fun_new(riak_config *cfg) {
    riak_mod_fun *fun = (riak_mod_fun*)(cfg->malloc_fn)(sizeof(riak_mod_fun));
    if (fun) memset(fun, '\0', sizeof(riak_mod_fun));
    return fun;
}

static riak_error
riak_mod_fun_copy_to_pb(riak_config   *cfg,
                        RpbModFun    **pbmod_fun_target,
                        riak_mod_fun  *mod_fun) {
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
riak_mod_fun_copy_from_pb(riak_config    *cfg,
                          riak_mod_fun **mod_fun_target,
                          RpbModFun     *pbmod_fun) {
    riak_mod_fun *mod_fun = (riak_mod_fun*)(cfg->malloc_fn)(sizeof(riak_mod_fun));
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
riak_mod_fun_free_pb(riak_config  *cfg,
                     RpbModFun   **pbmod_fun_target) {
    riak_free(cfg, pbmod_fun_target);
}

void
riak_mod_fun_free(riak_config   *cfg,
                  riak_mod_fun **mod_fun_target) {
    riak_mod_fun *mod_fun = *mod_fun_target;
    riak_free(cfg, &(mod_fun->module));
    riak_free(cfg, &(mod_fun->function));
    riak_free(cfg, mod_fun_target);
}

riak_int32_t
riak_mod_fun_print_internal(char           *name,
                            riak_mod_fun   *mod_fun,
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
riak_mod_fun_print(riak_mod_fun  *mod_fun,
                   char          *target,
                   riak_int32_t   len) {
    riak_int32_t total = 0;
    return riak_mod_fun_print_internal("Mod Fun",
                                       mod_fun,
                                       &target,
                                       &len,
                                       &total);
}

riak_binary*
riak_mod_fun_get_module(riak_mod_fun *mod_fun) {
    return mod_fun->module;
}
riak_binary*
riak_mod_fun_get_function(riak_mod_fun *mod_fun) {
    return mod_fun->function;
}
void
riak_mod_fun_set_module(riak_mod_fun *mod_fun,
                        riak_binary  *value) {
    mod_fun->module = value;
}
void
riak_mod_fun_set_function(riak_mod_fun *mod_fun,
                          riak_binary  *value) {
    mod_fun->function = value;
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
        riak_error err = riak_mod_fun_copy_to_pb(cfg, &(pbhook[i]->modfun), hook[i]->modfun);
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
        riak_error err = riak_mod_fun_copy_from_pb(cfg, &(hook[i]->modfun), pbhook[i]->modfun);
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
        wrote += riak_mod_fun_print_internal("Mod Fun", hooks[i]->modfun, target, len, total);
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
        riak_mod_fun_free_pb(cfg, &(pbhook[i]->modfun));
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
        riak_mod_fun_free(cfg, &(hook[i]->modfun));
        riak_free(cfg, &(hook[i]));
    }
    riak_free(cfg, hook_target);
}

riak_mod_fun*
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
void
riak_commit_hook_set_modfun(riak_commit_hook *hook,
                            riak_mod_fun     *value) {
    hook->modfun = value;
}
void
riak_commit_hook_set_backend(riak_commit_hook *hook,
                             riak_binary      *value) {
    hook->has_name = RIAK_TRUE;
    hook->name = value;
}


//
// R I A K   B U C K E T   P R O P E R T I E S
//
riak_bucket_props*
riak_bucket_props_new(riak_config *cfg) {
    riak_bucket_props *pty = (riak_bucket_props*)(cfg->malloc_fn)(sizeof(riak_bucket_props));
    if (pty) memset(pty, '\0', sizeof(riak_bucket_props));
    return pty;
}


riak_error
riak_bucket_props_to_pb_copy(riak_config       *cfg,
                             RpbBucketProps    *to,
                             riak_bucket_props *from) {

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
    if (from->has_yz_index) {
        to->has_yz_index = RIAK_TRUE;
        riak_binary_to_pb_copy(&(to->yz_index), from->yz_index);
    }

    riak_error err = riak_mod_fun_copy_to_pb(cfg, &(to->chash_keyfun), from->chash_keyfun);
    if (err) {
        return err;
    }
    err = riak_mod_fun_copy_to_pb(cfg, &(to->linkfun), from->linkfun);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}


riak_error
riak_bucket_props_new_from_pb(riak_config        *cfg,
                              riak_bucket_props **target,
                              RpbBucketProps     *from) {
    *target = riak_bucket_props_new(cfg);
    if (*target == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_bucket_props *to = *target;

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
    if (from->has_yz_index) {
        to->has_yz_index = RIAK_TRUE;
        to->yz_index = riak_binary_populate_from_pb(cfg, &(from->yz_index));
        if (to->yz_index == NULL) {
            riak_free(cfg, target);
            return ERIAK_OUT_OF_MEMORY;
        }
    }

    riak_error err = riak_mod_fun_copy_from_pb(cfg, &(to->chash_keyfun), from->chash_keyfun);
    if (err) {
        return err;
    }
    err = riak_mod_fun_copy_from_pb(cfg, &(to->linkfun), from->linkfun);
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

static const char* riak_bucket_props_quorum_decoder[] = {
    "unknown",
    "one",
    "quorum",
    "all",
    "default"
};

char*
riak_bucket_props_quorum(riak_uint32_t q) {
     riak_int32_t offset = UINT32_MAX - q;
     if ((offset < 0) || (offset >= RIAK_MAX_QUORUM)) offset = 0;
     return (char*)riak_bucket_props_quorum_decoder[offset];
}

riak_int32_t
riak_bucket_props_print(riak_bucket_props *prop,
                        char              *target,
                        riak_int32_t       len) {
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
        riak_print_string("PR", riak_bucket_props_quorum(prop->pr), &target, &len, &total);
    }
    if (prop->has_r) {
        riak_print_string("R", riak_bucket_props_quorum(prop->r), &target, &len, &total);
    }
    if (prop->has_w) {
        riak_print_string("W", riak_bucket_props_quorum(prop->w), &target, &len, &total);
    }
    if (prop->has_pw) {
        riak_print_string("PW", riak_bucket_props_quorum(prop->pw), &target, &len, &total);
    }
    if (prop->has_dw) {
        riak_print_string("DW", riak_bucket_props_quorum(prop->dw), &target, &len, &total);
    }
    if (prop->has_rw) {
        riak_print_string("RW", riak_bucket_props_quorum(prop->rw), &target, &len, &total);
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
    if (prop->repl >= RIAK_BUCKET_PROPS_REPL_FALSE &&
        prop->repl <= RIAK_BUCKET_PROPS_REPL_TRUE) {
        riak_print_string("Repl: ", (char*)riak_bucket_props_repl[prop->repl],
                          &target,
                          &len,
                          &total);
    }
    if (prop->has_yz_index) {
        riak_print_binary("YZ Index", prop->yz_index, &target, &len, &total);
    }
    riak_mod_fun_print_internal("C Hash Key Fun", prop->chash_keyfun, &target, &len, &total);
    riak_mod_fun_print_internal("Link Fun", prop->linkfun, &target, &len, &total);

    return total;
}

void
riak_bucket_props_free(riak_config          *cfg,
                         riak_bucket_props **props) {
    riak_bucket_props* prop = *props;
    riak_free(cfg, &(prop->backend));
    riak_mod_fun_free(cfg, &(prop->linkfun));
    riak_commit_hooks_free(cfg, &(prop->precommit), prop->n_precommit);
    riak_commit_hooks_free(cfg, &(prop->postcommit), prop->n_postcommit);
    riak_free(cfg, &(prop->yz_index));
    riak_free(cfg, props);
}

void
riak_bucket_props_free_pb(riak_config     *cfg,
                          RpbBucketProps **props) {
    RpbBucketProps *prop = *props;
    riak_mod_fun_free_pb(cfg, &(prop->chash_keyfun));
    riak_mod_fun_free_pb(cfg, &(prop->linkfun));
    riak_commit_hooks_free_pb(cfg, &(prop->precommit), prop->n_precommit);
    riak_commit_hooks_free_pb(cfg, &(prop->postcommit), prop->n_postcommit);
    riak_free(cfg, props);
}

//
// ACCESSORS
//
riak_boolean_t
riak_bucket_props_get_has_n_val(riak_bucket_props *prop) {
    return prop->has_n_val;
}
riak_uint32_t
riak_bucket_props_get_n_val(riak_bucket_props *prop) {
    return prop->n_val;
}
riak_boolean_t
riak_bucket_props_get_has_allow_mult(riak_bucket_props *prop) {
    return prop->has_allow_mult;
}
riak_boolean_t
riak_bucket_props_get_allow_mult(riak_bucket_props *prop) {
    return prop->allow_mult;
}
riak_boolean_t
riak_bucket_props_get_has_last_write_wins(riak_bucket_props *prop) {
    return prop->has_last_write_wins;
}
riak_boolean_t
riak_bucket_props_get_last_write_wins(riak_bucket_props *prop) {
    return prop->last_write_wins;
}
riak_size_t
riak_bucket_props_get_n_precommit(riak_bucket_props *prop) {
    return prop->n_precommit;
}
riak_commit_hook**
riak_bucket_props_get_precommit(riak_bucket_props *prop) {
    return prop->precommit;
}
riak_boolean_t
riak_bucket_props_get_has_precommit(riak_bucket_props *prop) {
    return prop->has_has_precommit && prop->has_precommit;
}
riak_size_t
riak_bucket_props_get_n_postcommit(riak_bucket_props *prop) {
    return prop->n_postcommit;
}
riak_commit_hook**
riak_bucket_props_get_postcommit(riak_bucket_props *prop) {
    return prop->postcommit;
}
riak_boolean_t
riak_bucket_props_get_has_postcommit(riak_bucket_props *prop) {
    return prop->has_has_postcommit && prop->has_postcommit;
}
riak_mod_fun*
riak_bucket_props_get_chash_keyfun(riak_bucket_props *prop) {
    return prop->chash_keyfun;
}
riak_mod_fun*
riak_bucket_props_get_linkfun(riak_bucket_props *prop) {
    return prop->linkfun;
}
riak_boolean_t
riak_bucket_props_get_has_old_vclock(riak_bucket_props *prop) {
    return prop->has_old_vclock;
}
riak_uint32_t
riak_bucket_props_get_old_vclock(riak_bucket_props *prop) {
    return prop->old_vclock;
}
riak_boolean_t
riak_bucket_props_get_has_young_vclock(riak_bucket_props *prop) {
    return prop->has_young_vclock;
}
riak_uint32_t
riak_bucket_props_get_young_vclock(riak_bucket_props *prop) {
    return prop->young_vclock;
}
riak_boolean_t
riak_bucket_props_get_has_big_vclock(riak_bucket_props *prop) {
    return prop->has_big_vclock;
}
riak_uint32_t
riak_bucket_props_get_big_vclock(riak_bucket_props *prop) {
    return prop->big_vclock;
}
riak_boolean_t
riak_bucket_props_get_has_small_vclock(riak_bucket_props *prop) {
    return prop->has_small_vclock;
}
riak_uint32_t
riak_bucket_props_get_small_vclock(riak_bucket_props *prop) {
    return prop->small_vclock;
}
riak_boolean_t
riak_bucket_props_get_has_pr(riak_bucket_props *prop) {
    return prop->has_pr;
}
riak_uint32_t
riak_bucket_props_get_pr(riak_bucket_props *prop) {
    return prop->pr;
}
riak_boolean_t
riak_bucket_props_get_has_r(riak_bucket_props *prop) {
    return prop->has_r;
}
riak_uint32_t
riak_bucket_props_get_r(riak_bucket_props *prop) {
    return prop->r;
}
riak_boolean_t
riak_bucket_props_get_has_w(riak_bucket_props *prop) {
    return prop->has_w;
}
riak_uint32_t
riak_bucket_props_get_w(riak_bucket_props *prop) {
    return prop->w;
}
riak_boolean_t
riak_bucket_props_get_has_pw(riak_bucket_props *prop) {
    return prop->has_pw;
}
riak_uint32_t
riak_bucket_props_get_pw(riak_bucket_props *prop) {
    return prop->pw;
}
riak_boolean_t
riak_bucket_props_get_has_dw(riak_bucket_props *prop) {
    return prop->has_dw;
}
riak_uint32_t
riak_bucket_props_get_dw(riak_bucket_props *prop) {
    return prop->dw;
}
riak_boolean_t
riak_bucket_props_get_has_rw(riak_bucket_props *prop) {
    return prop->has_rw;
}
riak_uint32_t
riak_bucket_props_get_rw(riak_bucket_props *prop) {
    return prop->rw;
}
riak_boolean_t
riak_bucket_props_get_has_basic_quorum(riak_bucket_props *prop) {
    return prop->has_basic_quorum;
}
riak_boolean_t
riak_bucket_props_get_basic_quorum(riak_bucket_props *prop) {
    return prop->basic_quorum;
}
riak_boolean_t
riak_bucket_props_get_has_notfound_ok(riak_bucket_props *prop) {
    return prop->has_notfound_ok;
}
riak_boolean_t
riak_bucket_props_get_notfound_ok(riak_bucket_props *prop) {
    return prop->notfound_ok;
}
riak_boolean_t
riak_bucket_props_get_has_backend(riak_bucket_props *prop) {
    return prop->has_backend;
}
riak_binary*
riak_bucket_props_get_backend(riak_bucket_props *prop) {
    return prop->backend;
}
riak_boolean_t
riak_bucket_props_get_has_search(riak_bucket_props *prop) {
    return prop->has_search;
}
riak_boolean_t
riak_bucket_props_get_search(riak_bucket_props *prop) {
    return prop->search;
}
riak_boolean_t
riak_bucket_props_get_has_repl(riak_bucket_props *prop) {
    return prop->has_repl;
}
riak_bucket_repl_mode
riak_bucket_props_get_repl(riak_bucket_props *prop) {
    return prop->repl;
}
riak_boolean_t
riak_bucket_props_get_has_yz_index(riak_bucket_props *prop) {
    return prop->has_yz_index;
}
riak_binary*
riak_bucket_props_get_yz_index(riak_bucket_props *prop) {
    return prop->yz_index;
}
void
riak_bucket_props_set_n_val(riak_bucket_props *prop,
                            riak_uint32_t      value) {
    prop->has_n_val = RIAK_TRUE;
    prop->n_val = value;
}
void
riak_bucket_props_set_allow_mult(riak_bucket_props *prop,
                                 riak_boolean_t     value) {
    prop->has_allow_mult = RIAK_TRUE;
    prop->allow_mult = value;
}
void
riak_bucket_props_set_last_write_wins(riak_bucket_props *prop,
                                      riak_boolean_t     value) {
    prop->has_last_write_wins = RIAK_TRUE;
    prop->last_write_wins = value;
}
void
riak_bucket_props_set_precommits(riak_bucket_props *prop,
                                 riak_commit_hook **precommit,
                                 riak_size_t        num) {
    prop->has_has_precommit = RIAK_TRUE;
    prop->has_precommit = RIAK_TRUE;
    prop->n_precommit = num;
    prop->precommit = precommit;
}
void
riak_bucket_props_set_postcommits(riak_bucket_props *prop,
                                  riak_commit_hook **postcommit,
                                  riak_size_t        num) {
    prop->has_has_postcommit = RIAK_TRUE;
    prop->has_postcommit = RIAK_TRUE;
    prop->n_postcommit = num;
    prop->postcommit = postcommit;
}
void
riak_bucket_props_set_chash_keyfun(riak_bucket_props *prop,
                                   riak_mod_fun      *value) {
    prop->chash_keyfun = value;
}
void
riak_bucket_props_set_linkfun(riak_bucket_props *prop,
                              riak_mod_fun      *value) {
    prop->linkfun = value;
}
void
riak_bucket_props_set_old_vclock(riak_bucket_props *prop,
                                 riak_uint32_t      value) {
    prop->has_old_vclock = RIAK_TRUE;
    prop->old_vclock = value;
}
void
riak_bucket_props_set_young_vclock(riak_bucket_props *prop,
                                   riak_uint32_t      value) {
    prop->has_young_vclock = RIAK_TRUE;
    prop->young_vclock = value;
}
void
riak_bucket_props_set_big_vclock(riak_bucket_props *prop,
                                 riak_uint32_t      value) {
    prop->has_big_vclock = RIAK_TRUE;
    prop->big_vclock = value;
}
void
riak_bucket_props_set_small_vclock(riak_bucket_props *prop,
                                   riak_uint32_t      value) {
    prop->has_small_vclock = RIAK_TRUE;
    prop->small_vclock = value;
}
void
riak_bucket_props_set_pr(riak_bucket_props *prop,
                         riak_uint32_t      value) {
    prop->has_pr = RIAK_TRUE;
    prop->pr = value;
}
void
riak_bucket_props_set_r(riak_bucket_props *prop,
                        riak_uint32_t      value) {
    prop->has_r = RIAK_TRUE;
    prop->r = value;
}
void
riak_bucket_props_set_w(riak_bucket_props *prop,
                        riak_uint32_t      value) {
    prop->has_w = RIAK_TRUE;
    prop->w = value;
}
void
riak_bucket_props_set_pw(riak_bucket_props *prop,
                         riak_uint32_t      value) {
    prop->has_pw = RIAK_TRUE;
    prop->pw = value;
}
void
riak_bucket_props_set_dw(riak_bucket_props *prop,
                         riak_uint32_t      value) {
    prop->has_dw = RIAK_TRUE;
    prop->dw = value;
}
void
riak_bucket_props_set_rw(riak_bucket_props *prop,
                         riak_uint32_t      value) {
    prop->has_rw = RIAK_TRUE;
    prop->rw = value;
}
void
riak_bucket_props_set_basic_quorum(riak_bucket_props *prop,
                                   riak_boolean_t     value) {
    prop->has_basic_quorum = RIAK_TRUE;
    prop->basic_quorum = value;
}
void
riak_bucket_props_set_notfound_ok(riak_bucket_props *prop,
                                  riak_boolean_t     value) {
    prop->has_notfound_ok = RIAK_TRUE;
    prop->notfound_ok = value;
}
void
riak_bucket_props_set_backend(riak_bucket_props *prop,
                              riak_binary       *value) {
    prop->has_backend = RIAK_TRUE;
    prop->backend = value;
}
void
riak_bucket_props_set_search(riak_bucket_props *prop,
                            riak_boolean_t      value) {
    prop->has_search = RIAK_TRUE;
    prop->search = value;
}
void
riak_bucket_props_set_repl(riak_bucket_props    *prop,
                           riak_bucket_repl_mode value) {
    prop->has_repl = RIAK_TRUE;
    prop->repl = value;
}
void
riak_bucket_props_set_yz_index(riak_bucket_props *prop,
                               riak_binary       *value) {
    prop->has_yz_index = RIAK_TRUE;
    prop->yz_index = value;
}
