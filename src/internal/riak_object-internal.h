/*********************************************************************
 *
 * riak_object.h: Riak Object suite
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

#ifndef _RIAK_OBJECT_INTERNAL_H
#define _RIAK_OBJECT_INTERNAL_H

// Based off of RpbLink
struct _riak_link
{
    riak_boolean_t has_bucket;
    riak_binary   *bucket;
    riak_boolean_t has_key;
    riak_binary   *key;
    riak_boolean_t has_tag;
    riak_binary   *tag;
};

// Based off of RpbPair
struct _riak_pair
{
    riak_binary   *key;
    riak_boolean_t has_value;
    riak_binary   *value;
};

// Based off of RpbContent
struct _riak_object {
    riak_binary   *bucket;

    riak_boolean_t has_key;
    riak_binary   *key;

    riak_binary   *value;

    riak_boolean_t has_charset;
    riak_binary   *charset;

    riak_boolean_t has_last_mod;
    riak_uint32_t  last_mod;

    riak_boolean_t has_last_mod_usecs;
    riak_uint32_t  last_mod_usecs;

    riak_boolean_t has_content_type;
    riak_binary   *content_type;

    riak_boolean_t has_content_encoding;
    riak_binary   *encoding;

    riak_boolean_t has_deleted;
    riak_boolean_t deleted;

    riak_boolean_t has_vtag;
    riak_binary   *vtag;

    riak_int32_t   n_links;
    riak_link    **links;

    riak_int32_t   n_usermeta;
    riak_pair    **usermeta;
    riak_int32_t   n_indexes;
    riak_pair    **indexes;
};

/**
 * @brief Shallow copy a Riak Object from a protocol buffer
 * @param cfg Riak Configuration
 * @param to Allocated `riak_object` returned to caller
 * @param from Riak-supplied Protocol buffer source
 *
 * @returns Error code
 */
riak_error
riak_object_new_from_pb(riak_config *cfg,
                        riak_object **to,
                        RpbContent   *from);

/**
 * @brief Copy a Riak Object to a protocol buffer
 * @param cfg Riak Configuration
 * @param to Protocol buffer target
 * @param from User-supplied `riak_object`
 *
 * @returns Error code
 */
int riak_object_to_pb_copy(riak_config *cfg,
                           RpbContent   *to,
                           riak_object  *from);

/**
 * @brief Release claimed memory used by PB Riak Object
 * @param cfg Riak Configuration
 * @param obj PB Riak Object to be freed
 */
void
riak_object_free_pb(riak_config *cfg,
                    RpbContent  *obj);

/**
 * @brief Copy a key/value pair from a PB to Riak structure
 * @param cfg Riak Configuration
 * @param pair_target The newly populated Riak key/value pair (out)
 * @param pbpair Protocol Buffer version of key/value pair
 * @returns Any memory allocation error
 */
riak_error
riak_pairs_copy_from_pb(riak_config *cfg,
                        riak_pair  **pair_target,
                        RpbPair     *pbpair);

/**
 * @brief Copy an array of key/value pairs from a PB to Riak structure
 * @param cfg Riak Configuration
 * @param pair_target The newly populated Riak key/value pair array (out)
 * @param pbpair Protocol Buffer version of key/value pair array
 * @param num_pairs Number of key/value pairs
 * @returns Any memory allocation error
 */
riak_error
riak_pairs_copy_array_from_pb(riak_config  *cfg,
                              riak_pair  ***pair_target,
                              RpbPair     **pbpair,
                              int           num_pairs);

/**
 * @brief Print out a series of key/value pairs
 * @param pair Array of pairs to print
 * @param num_pairs Number of arrays in `pair`
 * @param target Where to write the output (out)
 * @param len Max allowed bytes to write (out)
 * @param total Running total of bytes written (out)
 * @returns Number of bytes written
 */
riak_int32_t
riak_pairs_print(riak_pair   **pair,
                 riak_uint32_t num_pairs,
                 char        **target,
                 riak_int32_t *len,
                 riak_int32_t *total);

/**
 * @brief Deallocate any claimed memory by riak_pairs
 * @param cfg Riak Configuration
 * @param pair_target Pointer to array to be freed
 * @param num_pairs Number of elements in `pair_target`
 */
void
riak_pairs_free(riak_config  *cfg,
                riak_pair  ***pair_target,
                int           num_pairs);

#endif // _RIAK_OBJECT_INTERNAL_H
