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
#include "riak_utils-internal.h"

riak_error
test_bkv_add(riak_config            *cfg,
             test_bucket_key_value **root,
             riak_binary            *bucket,
             riak_binary            *key,
             riak_object            *obj) {
    // Is this a sibling?
    test_bucket_key_value *node = *root;
    while (obj && node) {
        // If this is a sibling, just add object (value will mismatch)
        if (riak_binary_compare(node->bucket, bucket) == 0 &&
            riak_binary_compare(node->key, key)) {
            if (riak_array_realloc(cfg, (void***)&node->objs, sizeof(riak_object*), node->n_objs, node->n_objs+1) == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
            node->objs[node->n_objs++] = obj;
            return ERIAK_OK;
        }
        node = node->next;
    }
    // If not a sibling, add a new node
    node = riak_config_clean_allocate(cfg, sizeof(test_bucket_key_value));
    if (node == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    node->next   = *root;
    node->bucket = riak_binary_copy(cfg, bucket);
    node->key    = riak_binary_copy(cfg, key);
    node->n_objs = 0;
    node->objs   = riak_config_clean_allocate(cfg, sizeof(riak_object*));
    if (node->objs == NULL) {
        riak_free(cfg, &node);
        return ERIAK_OUT_OF_MEMORY;
    }
    if (obj) {
        node->objs[0] = obj;
        node->n_objs++;
    }
    *root = node;

    return ERIAK_OK;
}

void
test_bkv_free(riak_config            *cfg,
              test_bucket_key_value **root) {
    test_bucket_key_value *node = *root;
    while (node != NULL) {
        riak_binary_free(cfg, &(node->bucket));
        riak_binary_free(cfg, &(node->key));
        for(int i = 0; i < node->n_objs; i++) {
            riak_object_free(cfg, &(node->objs[i]));
        }
        riak_free(cfg, &node->objs);
        test_bucket_key_value *next = node->next;
        riak_free(cfg, &node);
        node = next;
    }
    *root = NULL;
 }
