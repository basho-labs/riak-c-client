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
riak_object_free_array(riak_config   *cfg,
                       riak_object ***array,
                       riak_size_t    len);

/**
 * @brief Access the name of the bucket
 * @param obj Riak Object
 * @returns The name of the Bucket
 */
riak_binary*
riak_object_get_bucket(riak_object *obj);

/**
 * @brief Determine if the `Key` has been populated
 * @param obj Riak Object
 * @returns True if the `Key` has been populated
 */
riak_boolean_t
riak_object_get_has_key(riak_object *obj);

/**
 * @brief Access the name of the `Key`
 * @param obj Riak Object
 * @returns The name of the `Key`
 */
riak_binary*
riak_object_get_key(riak_object *obj);

/**
 * @brief Access the object's value
 *      Character set is left to the user
 *      See `riak_object_get_has_charset()/riak_object_get_charset()`
 * @param obj Riak Object
 * @returns The object's value
 */
riak_binary*
riak_object_get_value(riak_object *obj);

/**
 * @brief Determine if the character set has been Set
 * @param obj Riak Object
 * @returns True if the character set has been Set
 */
riak_boolean_t
riak_object_get_has_charset(riak_object *obj);

/**
 * @brief Access the character set
 * @param obj Riak Object
 * @returns The character set
 */
riak_binary*
riak_object_get_charset(riak_object *obj);

/**
 * @brief Determine if the `Last Modified` time has been set
 * @param obj Riak Object
 * @returns True if the `Last Modified` time has been set
 */
riak_boolean_t
riak_object_get_has_last_mod(riak_object *obj);

/**
 * @brief Access the `Last Modified` time
 * @param obj Riak Object
 * @returns The `Last Modified` Time
 */
riak_uint32_t
riak_object_get_last_mod(riak_object *obj);

/**
 * @brief Determine if the `Last Modified uSeconds` has been set
 * @param obj Riak Object
 * @returns True if the `Last Modified uSeconds` has been set
 */
riak_boolean_t
riak_object_get_has_last_mod_usecs(riak_object *obj);

/**
 * @brief Access the `Last Modified uSeconds`
 * @param obj Riak Object
 * @returns The `Last Modified uSeconds`
 */
riak_uint32_t
riak_object_get_last_mod_usecs(riak_object *obj);

/**
 * @brief Determine if the `Content Type` has been set
 * @param obj Riak Object
 * @returns True if the `Content Type` has been set
 */
riak_boolean_t
riak_object_get_has_content_type(riak_object *obj);

/**
 * @brief Access the `Content Type`
 * @param obj Riak Object
 * @returns The `Content Type`
 */
riak_binary*
riak_object_get_content_type(riak_object *obj);

/**
 * @brief Determine if the `Content Encoding` has been set
 * @param obj Riak Object
 * @returns True if the `Content Encoding` has been set
 */
riak_boolean_t
riak_object_get_has_content_encoding(riak_object *obj);

/**
 * @brief Access the `Content Encoding`
 * @param obj Riak Object
 * @returns The `Content Encoding`
 */
riak_binary*
riak_object_get_content_encoding(riak_object *obj);

/**
 * @brief Determine if the `Deleted` flag has been set
 * @param obj Riak Object
 * @returns True if the `Deleted` flag has been set
 */
riak_boolean_t
riak_object_get_has_deleted(riak_object *obj);

/**
 * @brief Determine if the object has been deleted
 * @param obj Riak Object
 * @returns True if the object has been deleted
 */
riak_boolean_t
riak_object_get_deleted(riak_object *obj);

/**
 * @brief Determine if the `VTag` has been set
 * @param obj Riak Object
 * @returns True if the `VTag` has been set
 */
riak_boolean_t
riak_object_get_has_vtag(riak_object *obj);

/**
 * @brief Access the `VTag`
 * @param obj Riak Object
 * @returns The `VTag`
 */
riak_binary*
riak_object_get_vtag(riak_object *obj);

/**
 * @brief Access the number of links
 * @param obj Riak Object
 * @returns The number of links
 */
riak_int32_t
riak_object_get_n_links(riak_object *obj);

/**
 * @brief Access the link-walking links
 * @param obj Riak Object
 * @param link Returned Link
 * @param n Index of Link to Return
 * @returns Error code if index is out of range
 */

riak_error
riak_object_get_link(riak_object  *obj,
                     riak_link   **link,
                     riak_uint32_t n);

/**
 * @brief Access the Index of `User Metadata` items
 * @param obj Riak Object
 * @returns The number of `Metadata items`
 */
riak_int32_t
riak_object_get_n_usermeta(riak_object *obj);

/**
 * @brief Access the `User Metadata`
 * @param obj Riak Object
 * @param meta Returned User Metadata
 * @param n Index of `User Metadata` to return
 * @returns Error code if index is out of range
 */
riak_error
riak_object_get_usermeta(riak_object  *obj,
                         riak_pair   **meta,
                         riak_uint32_t n);

/**
 * @brief Access the number of indexes
 * @param obj Riak Object
 * @returns The number of indexes
 */
riak_int32_t
riak_object_get_n_indexes(riak_object *obj);

/**
 * @brief Access the object indexes
 * @param obj Riak Object
 * @param index Returned User Index
 * @param n Index number of User Index to return
 * @returns Error code if index is out of range
 */
riak_error
riak_object_get_index(riak_object  *obj,
                      riak_pair   **index,
                      riak_uint32_t n);

/**
 * @brief Set the name of the bucket
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @param value Name of the bucket
 * @returns Error code
 */
riak_error
riak_object_set_bucket(riak_config *cfg,
                       riak_object *obj,
                       riak_binary *value);

/**
 * @brief Set the key name
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @param value key name
 * @returns Error code
 */
riak_error
riak_object_set_key(riak_config *cfg,
                    riak_object *obj,
                    riak_binary *value);

/**
 * @brief Set the object's value
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @param value Object's value
 * @returns Error code
 */
riak_error
riak_object_set_value(riak_config *cfg,
                      riak_object *obj,
                      riak_binary *value);

/**
 * @brief Set the object's value without duplicating underlying data
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @param value Value
 * @returns Error code
 */
riak_error
riak_object_set_value_shallow_copy(riak_config *cfg,
                                   riak_object *obj,
                                   riak_binary *value);

/**
 * @brief Set the character set
 * @param obj Riak Object
 * @param value Character set
 * @returns Error code
 */
riak_error
riak_object_set_charset(riak_config *cfg,
                        riak_object *obj,
                        riak_binary *value);

/**
 * @brief Set the `Last Modified` time
 * @param obj Riak Object
 * @param value `Last Modified` time
 */
void
riak_object_set_last_mod(riak_object  *obj,
                         riak_uint32_t value);

/**
 * @brief Set the `Last Modified uSecs` time
 * @param obj Riak Object
 * @param value `Last Modified uSecs` time
 */
void
riak_object_set_last_mod_usecs(riak_object  *obj,
                               riak_uint32_t value);

/**
 * @brief Set the `Content Type`
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @param value `Content Type`
 * @returns Error code
 */
riak_error
riak_object_set_content_type(riak_config *cfg,
                             riak_object *obj,
                             riak_binary *value);

/**
 * @brief Set the content encoding
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @param value Encoding type
 * @returns Error code
 */
riak_error
riak_object_set_content_encoding(riak_config *cfg,
                                 riak_object *obj,
                                 riak_binary *value);

/**
 * @brief Set the `Deleted` flag
 * @param obj Riak Object
 * @param value `Deleted` flag
 */
void
riak_object_set_deleted(riak_object   *obj,
                        riak_boolean_t value);

/**
 * @brief Set the `VTag`
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @param value `VTag`
 * @returns Error code
 */
riak_error
riak_object_set_vtag(riak_config *cfg,
                     riak_object *obj,
                     riak_binary *value);

/**
 * @brief Create a new link-walking key/value pair
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @returns Empty link-walking key/value pair
 */
riak_link*
riak_object_new_link(riak_config *cfg,
                     riak_object *obj);

/**
 * @brief Create a new user metadata key/value pair
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @returns Empty user metadata key/value pair
 */
riak_pair*
riak_object_new_usermeta(riak_config *cfg,
                         riak_object *obj);
/**
 * @brief Create a new index key/value pair
 * @param cfg Riak Configuration
 * @param obj Riak Object
 * @returns Empty index key/value pair
 */
riak_pair*
riak_object_new_index(riak_config *cfg,
                      riak_object *obj);

/**
 * @brief Determine if the link bucket has been set
 * @param link Riak Link-Walking Link
 * @returns True if the link bucket has been set
 */
riak_boolean_t
riak_link_get_has_bucket(riak_link *link);

/**
 * @brief Access the link bucket
 * @param link Riak Link-Walking Link
 * @returns The link bucket
 */
riak_binary*
riak_link_get_bucket(riak_link *link);

/**
 * @brief Determine if the link key has been set
 * @param link Riak Link-Walking Link
 * @returns True if the link key has been set
 */
riak_boolean_t
riak_link_get_has_key(riak_link *link);

/**
 * @brief Access the link key
 * @param link Riak Link-Walking Link
 * @returns The link key
 */
riak_binary*
riak_link_get_key(riak_link *link);

/**
 * @brief Determine if the link tag has been set
 * @param link Riak Link-Walking Link
 * @returns True if the link tag has been set
 */
riak_boolean_t
riak_link_get_has_tag(riak_link *link);

/**
 * @brief Access the link tag
 * @param link Riak Link
 * @returns The link tag
 */
riak_binary*
riak_link_get_tag(riak_link *link);

/**
 * @brief Set the link bucket
 * @param cfg Riak Configuration
 * @param link Riak Link-Walking Link
 * @param value Link Bucket
 * @returns Error Code
 */
riak_error
riak_link_set_bucket(riak_config *cfg,
                     riak_link   *link,
                     riak_binary *value);

/**
 * @brief Set the Link Key
 * @param cfg Riak Configuration
 * @param link Riak Link-Walking Link
 * @param value Link Key
 * @returns Error Code
 */
riak_error
riak_link_set_key(riak_config *cfg,
                  riak_link   *link,
                  riak_binary *value);

/**
 * @brief Set the Link Tag
 * @param cfg Riak Configuration
 * @param link Riak Link-Walking Link
 * @param value Link Tag
 * @returns Error Code
 */
riak_error
riak_link_set_tag(riak_config *cfg,
                  riak_link   *link,
                  riak_binary *value);

/**
 * @brief Access the Key/Value Pair key
 * @param obj Riak Object
 * @returns The Key/Value Pair key
 */
riak_binary*
riak_pair_get_key(riak_pair *pair);

/**
 * @brief Determine if the Key/Value Pair value has been set
 * @param obj Riak Object
 * @returns True if the Key/Value Pair value has been set
 */
riak_boolean_t
riak_pair_get_has_value(riak_pair *pair);

/**
 * @brief Access the Key/Value Pair value
 * @param obj Riak Object
 * @returns The Key/Value Pair value
 */
riak_binary*
riak_pair_get_value(riak_pair *pair);

/**
 * @brief Set the Key/Value Pair key
 * @param cfg Riak Configuration
 * @param pair Key/Value Pair
 * @param value Key/Value Pair key
 * @returns Error Code
 */
riak_error
riak_pair_set_key(riak_config *cfg,
                  riak_pair   *pair,
                  riak_binary *value);

/**
 * @brief Set the Key/Value value
 * @param cfg Riak Configuration
 * @param pair Key/Value Pair
 * @param value Key/Value value
 * @returns Error Code
 */
riak_error
riak_pair_set_value(riak_config *cfg,
                    riak_pair   *pair,
                    riak_binary *value);

#endif // _RIAK_OBJECT_H
