/*********************************************************************
 *
 * riak_utils.h: Riak C Client Utilities
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

#ifndef _RIAK_UTILS_INTERNAL_H
#define _RIAK_UTILS_INTERNAL_H

/**
 * @brief Since strlcpy is not standard everywhere, write our own
 * @param dst Destination
 * @param src Source
 * @param size number of bytes including null terminator
 */
size_t
riak_strlcpy(char       *dst,
             const char *src,
             size_t      size);

/**
 * @brief Since strlcat is not standard everywhere, write our own
 * @param dst Destination
 * @param src Source
 * @param size number of bytes including null terminator
 */
size_t
riak_strlcat(char       *dst,
             const char *src,
             size_t      size);

/**
 * @brief Use Riak Configuration to reallocate memory
 * @param cfg Riak Configuration
 * @param from Location of old array pointer to reallocate
 * @param size Size of new unit to allocate
 * @param oldnum Number of old units allocated
 * @param newnum Number of new units to allocate
 * @return Address of new location or NULL on failure
 */
void**
riak_array_realloc(riak_config *cfg,
                   void       ***from,
                   riak_size_t   size,
                   riak_uint32_t oldnum,
                   riak_uint32_t newnum);

#endif // _RIAK_UTILS_INTERNAL_H
