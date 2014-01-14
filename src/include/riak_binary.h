/*********************************************************************
 *
 * riak_binary.h: Riak C Client Binary Object Utilities
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


#ifndef _RIAK_BINARY_H
#define _RIAK_BINARY_H

typedef struct _riak_binary riak_binary;

/**
 * @brief Allocate a new `riak_binary` struct
 * @param len Length of binary in bytes
 * @param data Pointer to binary data (shallow copy)
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_new(riak_config *cfg,
                riak_size_t   len,
                riak_uint8_t *data);


/**
 * @brief Allocate a new `riak_binary` struct
 * @param bin Original Riak Binary
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_deep_new(riak_config *cfg,
                     riak_binary *bin);

/**
 * @brief Allocate a new riak_binary and populate from data pointer
 * @param bin Existing `riak_binary` to be shallow copied
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_populate(riak_config *cfg,
                     riak_binary  *bin);

/**
 * @brief Free allocated memory used by `riak_binary`
 */
void
riak_binary_free(riak_config  *cfg,
                 riak_binary  **bin);
void
riak_binary_deep_free(riak_config  *cfg,
                      riak_binary  **bin);

/**
 * @brief Return the length of the binary object
 * @param bin Riak Binary
 * @returns Length in bytes
 */
riak_size_t
riak_binary_len(riak_binary *bin);

/**
 * @brief Get the encapsulated data
 * @param bin Riak Binary
 * @returns Pointer to underlying data
 */
riak_uint8_t*
riak_binary_data(riak_binary *bin);

void
riak_binary_copy(riak_binary *to,
                 riak_binary *from);
riak_error
riak_binary_deep_copy(riak_config *cfg,
                      riak_binary  *to,
                      riak_binary  *from);
int
riak_binary_print(riak_binary  *bin,
                  char         *target,
                  riak_uint32_t len);
int
riak_binary_hex_print(riak_binary  *bin,
                      char         *target,
                      riak_uint32_t len);
void
riak_binary_from_string(riak_binary *to,
                        const char  *from);
riak_error
riak_binary_from_string_deep_copy(riak_config *cfg,
                                  riak_binary  *to,
                                  const char   *from);
riak_binary*
riak_binary_new_from_string(riak_config *cfg,
                            const char   *from);

#endif // _RIAK_BINARY_H
