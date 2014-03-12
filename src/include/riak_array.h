/*********************************************************************
 *
 * riak_array.h: Management of a Dynamic Array
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

#ifndef _RIAK_ARRAY_H
#define _RIAK_ARRAY_H

typedef struct _riak_array {
    riak_config *cfg;
    riak_int64_t length;
    riak_int64_t capacity;
    void       **array;
} riak_array;

/**
 * @brief Create yet another generic C data structure
 * @param cfg Riak Configuration
 * @param size Initial size of the array
 */
riak_array*
riak_array_new(riak_config *cfg,
               riak_size_t  size);

/**
 * @brief Free any memory claimed by the array itself
 * NOTE: Elements must be freed separately
 * @param ra Riak Array
 */
void
riak_array_free(riak_array **ra);

/**
 * @brief Access the size of the underlying array
 * @param ra Riak Array
 * @returns The number of elements in the Riak Array
 */
riak_size_t
riak_array_length(riak_array *ra);

/**
 * @brief Add an element to an existing array (shifting other elements forward)
 * @param ra Riak Array
 * @param pos Position within array to set value
 * @param elem Element to be added
 * @returns Error Code
 */
riak_error
riak_array_insert(riak_array *ra,
                  riak_size_t pos,
                  void       *elem);

/**
 * @brief Remove an element from a Riak Array
 * @param ra Riak Array
 * @param pos Position at which to remove an element
 * @returns The selected element
 */
void*
riak_array_remove(riak_array *ra,
                  riak_size_t pos);

/**
 * @brief Add an element to an existing array (at the end)
 * @param ra Riak Array
 * @param elem Element to be added
 * @returns Error Code
 */
riak_error
riak_array_push(riak_array *ra,
                void       *elem);

/**
 * @brief Remove an element from a Riak Array (from the end)
 * @param ra Riak Array
 * @param pos Position at which to remove an element
 * @returns The selected element
 */
void*
riak_array_pop(riak_array *ra);

/**
 * @brief Access an element from a Riak Array
 * @param ra Riak Array
 * @param pos Position at which to retrieve an element
 * @returns The selected element
 */
void*
riak_array_get(riak_array *ra,
               riak_size_t pos);

/**
 * @brief Add an element to an existing array (at a given position)
 * @param ra Riak Array
 * @param pos Position with array to set value
 * @param elem Element to be added
 * @returns Error Code
 */
riak_error
riak_array_set(riak_array *ra,
               riak_size_t pos,
               void       *elem);

#endif // _RIAK_ARRAY_H
