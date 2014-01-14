/*********************************************************************
 *
 * test_bucketprops.c: Riak C Unit testing for Bucket Properties Messages
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

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include "riak.h"
#include "riak_messages-internal.h"
#include "riak_operation-internal.h"

void
test_bucketprops() {
    riak_config     *cfg;

    riak_error err = riak_config_new_default(&cfg);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    riak_bucketprops *props = riak_bucketprops_new(cfg);
    CU_ASSERT_FATAL(props != NULL)

    // First, set ALL THE THINGS
    riak_bucketprops_set_n_val(props, 5);
    riak_bucketprops_set_allow_mult(props, RIAK_TRUE);
    riak_bucketprops_set_last_write_wins(props, RIAK_TRUE);

    riak_commit_hook **precommits;
    err = riak_commit_hook_new_array(cfg, &precommits, 2);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    precommits[0] = riak_commit_hook_new(cfg);
    CU_ASSERT_FATAL(precommits[0] != NULL)
    precommits[1] = riak_commit_hook_new(cfg);
    CU_ASSERT_FATAL(precommits[1] != NULL)
    riak_binary module;
    module.data = (riak_uint8_t*)"module";
    module.len = 6;
    riak_binary precommit_fun;
    precommit_fun.data = (riak_uint8_t*)"precommit";
    precommit_fun.len = 9;
    riak_modfun *precommit_mf = riak_modfun_new(cfg);
    CU_ASSERT_FATAL(precommit_mf != NULL)
    riak_modfun_set_module(cfg, precommit_mf, &module);
    riak_modfun_set_function(cfg, precommit_mf, &precommit_fun);
    riak_binary precommit_name;
    precommit_name.data = (riak_uint8_t*)"precommit";
    precommit_name.len  = 9;
    riak_commit_hook_set_modfun(cfg, precommits[0], precommit_mf);
    riak_commit_hook_set_name(cfg, precommits[0], &precommit_name);
    riak_commit_hook_set_modfun(cfg, precommits[1], precommit_mf);
    riak_bucketprops_set_precommits(props, precommits, 2);
    riak_commit_hook_set_name(cfg, precommits[1], &precommit_name);
    riak_modfun_free(cfg, &precommit_mf);

    riak_commit_hook **postcommits;
    err = riak_commit_hook_new_array(cfg, &postcommits, 2);
    CU_ASSERT_FATAL(err == ERIAK_OK)
    postcommits[0] = riak_commit_hook_new(cfg);
    CU_ASSERT_FATAL(postcommits[0] != NULL)
    postcommits[1] = riak_commit_hook_new(cfg);
    CU_ASSERT_FATAL(postcommits[1] != NULL)
    riak_binary postcommit_fun;
    postcommit_fun.data = (riak_uint8_t*)"postcommit";
    postcommit_fun.len = 10;
    riak_modfun *postcommit_mf = riak_modfun_new(cfg);
    CU_ASSERT_FATAL(postcommit_mf != NULL)
    riak_modfun_set_module(cfg, postcommit_mf, &module);
    riak_modfun_set_function(cfg, postcommit_mf, &postcommit_fun);
    riak_binary postcommit_name;
    postcommit_name.data = (riak_uint8_t*)"postcommit";
    postcommit_name.len  = 10;
    riak_commit_hook_set_modfun(cfg, postcommits[0], postcommit_mf);
    riak_commit_hook_set_name(cfg, postcommits[0], &postcommit_name);
    riak_commit_hook_set_modfun(cfg, postcommits[1], postcommit_mf);
    riak_bucketprops_set_postcommits(props, postcommits, 2);
    riak_commit_hook_set_name(cfg, postcommits[1], &postcommit_name);
    riak_modfun_free(cfg, &postcommit_mf);

    riak_modfun *chash = riak_modfun_new(cfg);
    CU_ASSERT_FATAL(chash != NULL)
    riak_binary chash_fun;
    chash_fun.data = (riak_uint8_t*)"chash";
    chash_fun.len = 5;
    riak_modfun_set_module(cfg, chash, &module);
    riak_modfun_set_function(cfg, chash, &chash_fun);
    riak_bucketprops_set_chash_keyfun(cfg, props, chash);

    riak_modfun *link = riak_modfun_new(cfg);
    CU_ASSERT_FATAL(link != NULL)
    riak_binary link_fun;
    link_fun.data = (riak_uint8_t*)"link";
    link_fun.len = 4;
    riak_modfun_set_module(cfg, link, &module);
    riak_modfun_set_function(cfg, link, &link_fun);
    riak_bucketprops_set_linkfun(cfg, props, link);

    riak_bucketprops_set_old_vclock(props, 6);
    riak_bucketprops_set_young_vclock(props, 7);
    riak_bucketprops_set_big_vclock(props, 8);
    riak_bucketprops_set_small_vclock(props, 9);
    riak_bucketprops_set_pr(props, 10);
    riak_bucketprops_set_r(props, 11);
    riak_bucketprops_set_w(props, 12);
    riak_bucketprops_set_pw(props, 13);
    riak_bucketprops_set_dw(props, 14);
    riak_bucketprops_set_rw(props, 15);
    riak_bucketprops_set_basic_quorum(props, RIAK_TRUE);
    riak_bucketprops_set_notfound_ok(props, RIAK_TRUE);
    riak_binary backend;
    backend.data = (riak_uint8_t*)"bitcask";
    backend.len = 7;
    riak_bucketprops_set_backend(props, &backend);
    riak_bucketprops_set_search(props, RIAK_TRUE);
    riak_bucketprops_set_repl(props, RIAK_BUCKETPROPS_REPL_FULLSYNC);
    riak_binary index;
    index.data = (riak_uint8_t*)"index";
    index.len = 5;
    riak_bucketprops_set_search_index(props, &index);

    // Next, Read back ALL THE THINGS
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_n_val(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_n_val(props), 5);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_allow_mult(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_allow_mult(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_last_write_wins(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_last_write_wins(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_precommit(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_n_precommit(props), 2);
    riak_commit_hook **precommit_hooks = riak_bucketprops_get_precommit(props);
    CU_ASSERT_FATAL(precommit_hooks != NULL);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_commit_hook_get_name(precommit_hooks[0])), "precommit", 9), 0);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_commit_hook_get_name(precommit_hooks[1])), "precommit", 9), 0);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_postcommit(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_n_postcommit(props), 2);
    riak_commit_hook **postcommit_hooks = riak_bucketprops_get_postcommit(props);
    CU_ASSERT_FATAL(postcommit_hooks != NULL);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_commit_hook_get_name(postcommit_hooks[0])), "postcommit", 10), 0);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(riak_commit_hook_get_name(postcommit_hooks[1])), "postcommit", 10), 0);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_postcommit(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_n_postcommit(props), 2);
    riak_modfun *chash_modfun = riak_bucketprops_get_chash_keyfun(props);
    CU_ASSERT_FATAL(chash_modfun  != NULL);
    riak_binary *chash_fn = riak_modfun_get_function(chash_modfun);
    CU_ASSERT_FATAL(chash_fn != NULL);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(chash_fn), "chash", 5), 0);
    riak_modfun *link_modfun = riak_bucketprops_get_linkfun(props);
    CU_ASSERT_FATAL(link_modfun  != NULL);
    riak_binary *link_fn = riak_modfun_get_function(link_modfun);
    CU_ASSERT_FATAL(link_fn  != NULL);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(link_fn), "link", 4), 0);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_old_vclock(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_old_vclock(props), 6);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_young_vclock(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_young_vclock(props), 7);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_big_vclock(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_big_vclock(props), 8);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_small_vclock(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_small_vclock(props), 9);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_pr(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_pr(props), 10);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_r(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_r(props), 11);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_w(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_w(props), 12);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_pw(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_pw(props), 13);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_dw(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_dw(props), 14);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_rw(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_rw(props), 15);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_basic_quorum(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_basic_quorum(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_notfound_ok(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_notfound_ok(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_backend(props), RIAK_TRUE);
    riak_binary *backend_val = riak_bucketprops_get_backend(props);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(backend_val), "bitcask", 7), 0);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_search(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_search(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_repl(props), RIAK_TRUE);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_repl(props), RIAK_BUCKETPROPS_REPL_FULLSYNC);
    CU_ASSERT_EQUAL_FATAL(riak_bucketprops_get_has_search_index(props), RIAK_TRUE);
    riak_binary* search_index = riak_bucketprops_get_search_index(props);
    CU_ASSERT_EQUAL(memcmp(riak_binary_data(search_index), "index", 5), 0);

    riak_commit_hooks_free(cfg, &precommits, 2);
    riak_commit_hooks_free(cfg, &postcommits, 2);
    riak_modfun_free(cfg, &chash);
    riak_modfun_free(cfg, &link);
    riak_config_free(&cfg);
    CU_PASS("test_bucketprops passed")
}
