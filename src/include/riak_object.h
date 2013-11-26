/*********************************************************************
 *
 * riak_object.h: Riak Object suite
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

#ifndef _RIAK_OBJECT_H
#define _RIAK_OBJECT_H

typedef struct _riak_pair riak_pair;
typedef struct _riak_link riak_link;
typedef struct _riak_object riak_object;

/**
 * @brief Construct a new Riak Object
 * @param cfg Riak Configuration
 * @return Riak Object
 */
riak_object*
riak_object_new(riak_config *cfg);

/**
 * @brief Release claimed memory used by a Riak Object
 * @param cfg Riak Configuration
 * @param obj Riak Object to be freed
 */
void
riak_object_free(riak_config *cfg,
                 riak_object **obj);

/**
 * @brief Print contents of a Riak Object to a string
 * @param obj Object to print
 * @param target Location to write formatted string
 * @param len Number of bytes to write
 * @return Number of bytes written
 */
int
riak_object_print(riak_object  *obj,
                  char         *target,
                  riak_uint32_t len);

/**
 * @brief Allocate an array of `riak_object` pointers
 * @param cfg Riak Configuration
 * @param array Returned array of pointers to `riak_object`s
 * @param len Number of elements to allocate
 *
 * @returns Error Code
 */
riak_error
riak_object_new_array(riak_config  *cfg,
                      riak_object ***array,
                      riak_size_t    len);

/**
 * @brief Clean up memory allocated for an array of `riak_objects`
 * @param cfg Riak Configuration
 * @param array Target of destruction
 * @param len Number of elements to allocate
 */
void
riak_object_free_array(riak_config  *cfg,
                       riak_object ***array,
                       riak_size_t    len);

riak_binary   *riak_object_get_bucket(riak_object *obj);
riak_boolean_t riak_object_get_has_key(riak_object *obj);
riak_binary   *riak_object_get_key(riak_object *obj);
riak_binary   *riak_object_get_value(riak_object *obj);
riak_boolean_t riak_object_get_has_charset(riak_object *obj);
riak_binary   *riak_object_get_charset(riak_object *obj);
riak_boolean_t riak_object_get_has_last_mod(riak_object *obj);
riak_uint32_t  riak_object_get_last_mod(riak_object *obj);
riak_boolean_t riak_object_get_has_last_mod_usecs(riak_object *obj);
riak_uint32_t  riak_object_get_last_mod_usecs(riak_object *obj);
riak_boolean_t riak_object_get_has_content_type(riak_object *obj);
riak_binary   *riak_object_get_content_type(riak_object *obj);
riak_boolean_t riak_object_get_has_content_encoding(riak_object *obj);
riak_binary   *riak_object_get_encoding(riak_object *obj);
riak_boolean_t riak_object_get_has_deleted(riak_object *obj);
riak_boolean_t riak_object_get_deleted(riak_object *obj);
riak_boolean_t riak_object_get_has_vtag(riak_object *obj);
riak_binary   *riak_object_get_vtag(riak_object *obj);
riak_int32_t   riak_object_get_n_links(riak_object *obj);
riak_link    **riak_object_get_links(riak_object *obj);
riak_int32_t   riak_object_get_n_usermeta(riak_object *obj);
riak_pair    **riak_object_get_usermeta(riak_object *obj);
riak_int32_t   riak_object_get_n_indexes(riak_object *obj);
riak_pair    **riak_object_get_indexes(riak_object *obj);
void riak_object_set_bucket(riak_object *obj, riak_binary *value);
void riak_object_set_key(riak_object *obj, riak_binary *value);
void riak_object_set_value(riak_object *obj, riak_binary *value);
void riak_object_set_charset(riak_object *obj, riak_binary *value);
void riak_object_set_last_mod(riak_object *obj, riak_uint32_t value);
void riak_object_set_last_mod_usecs(riak_object *obj, riak_uint32_t value);
void riak_object_set_content_type(riak_object *obj, riak_binary *value);
void riak_object_set_encoding(riak_object *obj, riak_binary *value);
void riak_object_set_has_deleted(riak_object *obj, riak_boolean_t value);
void riak_object_set_deleted(riak_object *obj, riak_boolean_t value);
void riak_object_set_vtag(riak_object *obj, riak_binary *value);
void riak_object_set_n_pairs(riak_object *obj, riak_int32_t value);
void riak_object_set_pairs(riak_object *obj, riak_pair **value);
void riak_object_set_n_usermeta(riak_object *obj, riak_int32_t value);
void riak_object_set_usermeta(riak_object *obj, riak_pair **value);
void riak_object_set_n_indexes(riak_object *obj, riak_int32_t value);
void riak_object_set_indexes(riak_object *obj, riak_pair **value);

/**
 * @brief Construct a new Riak Object
 * @param cfg Riak Configuration
 * @return Riak link
 */
riak_link*
riak_link_new(riak_config *cfg);

/**
 * @brief Allocate an array of `riak_links` pointers
 * @param cfg Riak Configuration
 * @param array Returned array of pointers to `riak_links`s
 * @param len Number of elements to allocate
 *
 * @returns Error Code
 */
riak_error
riak_link_new_array(riak_config  *cfg,
                    riak_link   ***array,
                    riak_size_t    len);

/**
 * @brief Release memory claimed by links
 * @param cfg Riak Configuration
 * @param links Pointer to array of links to be freed
 * @param num_links Number of links in array
 */
void
riak_links_free(riak_config *cfg,
                riak_link  ***links,
                int           num_links);

riak_boolean_t riak_link_get_has_bucket(riak_link *link);
riak_binary   *riak_link_get_bucket(riak_link *link);
riak_boolean_t riak_link_get_has_key(riak_link *link);
riak_binary   *riak_link_get_key(riak_link *link);
riak_boolean_t riak_link_get_has_tag(riak_link *link);
riak_binary   *riak_link_get_tag(riak_link *link);
void riak_link_set_bucket(riak_link *link, riak_binary *value);
void riak_link_set_key(riak_link *link, riak_binary *value);
void riak_link_set_tag(riak_link *link, riak_binary *value);

/**
 * @brief Construct a new Riak Object
 * @param cfg Riak Configuration
 * @return Riak Pair
 */
riak_pair*
riak_pair_new(riak_config *cfg);

/**
 * @brief Allocate an array of `riak_pairs` pointers
 * @param cfg Riak Configuration
 * @param array Returned array of pointers to `riak_pairs`s
 * @param len Number of elements to allocate
 *
 * @returns Error Code
 */
riak_error
riak_pair_new_array(riak_config  *cfg,
                    riak_pair   ***array,
                    riak_size_t    len);

/**
 * @brief Release memory claimed by Pairs
 * @param cfg Riak Configuration
 * @param pairs Pointer to array of pairs to be freed
 * @param num_pairs Number of pairs in array
 */
void
riak_pairs_free(riak_config *cfg,
                riak_pair  ***pair_target,
                int           num_pairs);

riak_binary   *riak_pair_get_key(riak_pair *pair);
riak_boolean_t riak_pair_get_has_value(riak_pair *pair);
riak_binary   *riak_pair_get_value(riak_pair *pair);
void riak_pair_set_key(riak_pair *pair, riak_binary *value);
void riak_pair_set_value(riak_pair *pair, riak_binary *value);

#endif // _RIAK_OBJECT_H
