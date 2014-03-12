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

    riak_boolean_t has_bucket_type;
    riak_binary   *bucket_type;
    
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
 * @param to Allocated Riak Object returned to caller
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
riak_error
riak_object_to_pb_copy(riak_config *cfg,
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

//
// P A I R S
//

/**
 * @brief Construct a new Riak Key/Value Pair
 * @param cfg Riak Configuration
 * @return Riak Key/Value Pair
 */
riak_pair*
riak_pair_new(riak_config *cfg);

/**
 * @brief Release memory claimed by Riak Key/Value Pairs
 * @param cfg Riak Configuration
 * @param pairs Pointer to array of pairs to be freed
 * @param num_pairs Number of pairs in array
 */
void
riak_pairs_free(riak_config  *cfg,
                riak_pair  ***pair_target,
                int           num_pairs);

/**
 * @brief Copy a key/value pair from a PB to Riak structure
 * @param cfg Riak Configuration
 * @param pair_target The newly populated Riak Key/Value Pair (out)
 * @param pbpair Protocol Buffer version of Riak Key/Value Pair
 * @returns Any memory allocation error
 */
riak_error
riak_pairs_copy_from_pb(riak_config *cfg,
                        riak_pair  **pair_target,
                        RpbPair     *pbpair);

/**
 * @brief Copy an array of Riak Key/Value Pairs from a PB to Riak structure
 * @param cfg Riak Configuration
 * @param pair_target The newly populated Riak Key/Value Pair array (out)
 * @param pbpair Protocol Buffer version of Riak Key/Value Pair array
 * @param num_pairs Number of Riak Key/Value Pairs
 * @returns Any memory allocation error
 */
riak_error
riak_pairs_copy_array_from_pb(riak_config  *cfg,
                              riak_pair  ***pair_target,
                              RpbPair     **pbpair,
                              int           num_pairs);

/**
 * @brief Print out a series of Riak Key/Value Pair
 * @param state Riak Print State
 * @param pair Array of Riak Key/Value Pair to print
 * @param num_pairs Number of arrays in Riak Key/Value Pair
 * @returns Number of bytes written
 */
riak_int32_t
riak_pairs_print(riak_print_state *state,
                 riak_pair       **pair,
                 riak_uint32_t     num_pairs);

/**
 * @brief Deallocate any claimed memory by Riak Key/Value Pair
 * @param cfg Riak Configuration
 * @param pair_target Pointer to array to be freed
 * @param num_pairs Number of elements in Riak Key/Value Pair array
 */
void
riak_pairs_free(riak_config  *cfg,
                riak_pair  ***pair_target,
                int           num_pairs);

//
// L I N K S
//

/**
 * @brief Construct a new Riak Link-Walking Link
 * @param cfg Riak Configuration
 * @return Riak Link-Walking Link
 */
riak_link*
riak_link_new(riak_config *cfg);

/**
 * @brief Release memory claimed by links
 * @param cfg Riak Configuration
 * @param links Pointer to array of links to be freed
 * @param num_links Number of links in array
 */
void
riak_links_free(riak_config  *cfg,
                riak_link  ***links,
                int           num_links);

#endif // _RIAK_OBJECT_INTERNAL_H
