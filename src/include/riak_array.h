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
 * @param ra Returned Riak Array (out)
 * @param capacity Initial capacity of the array
 */
riak_error
riak_array_new(riak_config *cfg,
               riak_array **ra,
               riak_size_t  capacity);

/**
 * @brief Free any memory claimed by the array itself
 * NOTE: Elements must be freed separately
 * @param ra Riak Array
 */
void
riak_array_free(riak_array **ra);

/**
 * @brief Access the length of the underlying array
 * @param ra Riak Array
 * @returns The number of elements in the Riak Array
 */
riak_size_t
riak_array_length(riak_array *ra);

/**
 * @brief Add an element to an existing array (shifting other elements forward)
 * @param ra Riak Array
 * @param pos Zero-indexed position within array to insert value
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
 * @param pos Zero-indexed position within array to remove value
 * @returns ERIAK_OUT_OF_RANGE
 */
riak_error
riak_array_remove(riak_array *ra,
                  riak_size_t pos);

/**
 * @brief Add an element to an existing array (at the end) and grows the array
 * @param ra Riak Array
 * @param elem Element to be added
 * @returns ERIAK_OUT_OF_MEMORY
 */
riak_error
riak_array_push(riak_array *ra,
                void       *elem);

/**
 * @brief Remove an element from a Riak Array (from the end)
 * @param ra Riak Array
 * @returns ERIAK_OUT_OF_RANGE
 */
riak_error
riak_array_pop(riak_array *ra,
               void      **elem);

/**
 * @brief Access an element from a Riak Array
 * @param ra Riak Array
 * @param pos Zero-indexed position within array to get value
 * @param elem Address of element to be returned
 * @returns ERIAK_OUT_OF_RANGE
 */
riak_error
riak_array_get(riak_array *ra,
               riak_size_t pos,
               void      **elem);

/**
 * @brief Add an element to an existing array at a given position
 * @param ra Riak Array
 * @param pos Zero-indexed position within array to set value
 * @param elem Element to be added
 * @returns ERIAK_OUT_OF_RANGE or ERIAK_OUT_OF_MEMORY
 */
riak_error
riak_array_set(riak_array *ra,
               riak_size_t pos,
               void       *elem);

typedef int (*riak_array_sort_comparer)(const void *a, const void *b);

/**
 * @brief Sort the array in place using the supplied element comparer
 * @param ra Riak Array
 * @param comparer Function to compare two elements in the array
 */

void
riak_array_sort(riak_array              *ra,
                riak_array_sort_comparer comparer);

#endif // _RIAK_ARRAY_H
