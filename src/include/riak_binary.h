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

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _riak_binary riak_binary;

/**
 * @brief Allocate a new `riak_binary` struct
 * @param cfg Riak Configuration
 * @param len Length of binary in bytes
 * @param data Pointer to binary data
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_new(riak_config  *cfg,
                riak_size_t   len,
                riak_uint8_t *data);

/**
 * @brief Allocate a new `riak_binary` struct (shallow copy)
 * @param cfg Riak Configuration
 * @param len Length of binary in bytes
 * @param data Pointer to binary data
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_new_shallow(riak_config  *cfg,
                        riak_size_t   len,
                        riak_uint8_t *data);

/**
 * @brief Determine if two non-NULL binaries have the same value
 * @param bin1 First Riak Binary
 * @param bin2 Second Riak Binary
 * @returns 0 if equal, -1 if bin1<bin2, 1 if bin1>bin2
 */
int
riak_binary_compare(riak_binary *bin1,
                    riak_binary *bin2);

/**
 * @brief Determine if a binary and a string have the same value
 * @param bin Riak Binary
 * @param str NULL-terminated ASCII string
 * @returns 0 if equal, -1 if bin<str, 1 if bin>str
 */
int
riak_binary_compare_string(riak_binary *bin,
                           const char  *str);

/**
 * @brief Allocate a new `riak_binary` struct
 * @param cfg Riak Configuration
 * @param bin Original Riak Binary
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_copy(riak_config *cfg,
                 riak_binary *bin);

/**
 * @brief Allocate a new `riak_binary` struct (shallow copy)
 * @param cfg Riak Configuration
 * @param bin Original Riak Binary
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_copy_shallow(riak_config *cfg,
                         riak_binary *bin);

/**
 * @brief Create a new `riak_binary` from a string
 * @param cfg Riak Configuration
 * @param from NULL-terminated string
 * @returns pointer to newly created `riak_binary` struct
 */
riak_binary*
riak_binary_copy_from_string(riak_config *cfg,
                             const char  *from);

/**
 * @brief Free allocated memory used by `riak_binary`
 * @param cfg Riak Configuration
 * @param bin Existing `riak_binary`
 */
void
riak_binary_free(riak_config  *cfg,
                 riak_binary **bin);

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

/**
 * @brief User-readable representation of a binary object
 * @param bin Riak Binary
 * @param target Where to write the output
 * @param len Maximum allowed number of bytes to write
 * @returns Number of bytes written
 */
riak_size_t
riak_binary_print(riak_binary  *bin,
                  char         *target,
                  riak_uint32_t len);

/**
 * @brief Hexadecimal representation of a binary object
 * @param bin Riak Binary
 * @param target Where to write the output
 * @param len Maximum allowed number of bytes to write
 * @returns Number of bytes written
 */
riak_size_t
riak_binary_hex_print(riak_binary  *bin,
                      char         *target,
                      riak_uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // _RIAK_BINARY_H
