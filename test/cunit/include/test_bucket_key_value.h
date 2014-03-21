/*********************************************************************
 *
 * test_bucket_key_value.h: Riak C Bucket, Key, Value Tuple
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

#ifndef _TEST_BUCKET_KEY_VALUE_
#define _TEST_BUCKET_KEY_VALUE_

typedef struct _test_bucket_key_value {
    riak_binary *bucket_type;
    riak_binary *bucket;
    riak_binary *key;
    riak_binary *value;
    struct _test_bucket_key_value *next;
} test_bucket_key_value;

/**
 * @brief Add a simple record to the list of Bucket/Key/Values
 * @param cfg Riak Configuration
 * @param root Updated first link in linked list of BKVs
 * @param bucket Name of bucket
 * @param key Name of key
 * @param value Value
 * @returns Error Code
 */
riak_error
test_bkv_add(riak_config            *cfg,
             test_bucket_key_value **root,
             riak_binary            *bucket_type_bin,
             riak_binary            *bucket_bin,
             riak_binary            *key,
             riak_binary            *value);

/**
 * @brief Frees the entire linked list of Bucket/Key/Values
 * @param cfg Riak Configuration
 * @param root First link in linked list of BKVs
 */
void
test_bkv_free(riak_config *cfg,
              test_bucket_key_value **root);
#endif //_TEST_BUCKET_KEY_VALUE_
