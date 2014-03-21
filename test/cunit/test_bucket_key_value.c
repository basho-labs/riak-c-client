/*********************************************************************
 *
 * test_bucket_key_value.c: Riak C Bucket, Key, Value Tuple
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
#include "test.h"

riak_error
test_bkv_add(riak_config            *cfg,
             test_bucket_key_value **root,
             riak_binary            *bucket_type_bin,
             riak_binary            *bucket_bin,
             riak_binary            *key,
             riak_binary            *value) {

    test_bucket_key_value *node = riak_config_clean_allocate(cfg, sizeof(test_bucket_key_value));
    if (node == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    node->next = *root;
    node->bucket_type = bucket_type_bin;
    node->bucket = bucket_bin;
    node->key = key;
    node->value = value;
    *root = node;

    return ERIAK_OK;
}

void
test_bkv_free(riak_config *cfg,
              test_bucket_key_value **root) {
    test_bucket_key_value *node = *root;
    while (node != NULL) {
        riak_binary_free(cfg, &(node->bucket));
        riak_binary_free(cfg, &(node->key));
        riak_binary_free(cfg, &(node->value));
        test_bucket_key_value *next = node->next;
        riak_free(cfg, &node);
        node = next;
    }
    *root = NULL;
 }
