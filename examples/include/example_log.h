/*********************************************************************
 *
 * example_log.h: Rxample Riak C Client Logging Functions
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

#ifndef _RIAK_EXAMPLE_LOG_H
#define _RIAK_EXAMPLE_LOG_H
#include <time.h>

typedef struct {
    FILE *fp;
} example_log_data;

/**
 * @brief Log initialization callback, open files here etc.
 * @param ptr Pointer to user-defined data structure (here example_log_data)
 * @returns 0 on success; 1 on failure
 */
riak_int32_t
example_log_init(void *ptr);

/**
 * @brief Do any maintenance after the log is no longer needed
 * @param ptr Pointer to user-defined data structure (here example_log_data)
 */
void
example_log_cleanup(void *ptr);

/**
 * @brief Handle an incoming log message
 * @param ptr Pointer to user-defined data structure (here example_log_data)
 * @param level Logging level (see `riak_log_level_t`)
 * @param file Filename where log message is written
 * @param filelen Length of above
 * @param func Name of function where log message is
 * @param funlen Length of above
 * @param line Line number where log message is
 * @param format Printf-like formatting string
 * @param ... Arguments to `format`
 */
void
example_log(void            *ptr,
            riak_log_level_t level,
            const char      *file,
            riak_size_t      filelen,
            const char      *func,
            riak_size_t      funclen,
            riak_uint32_t    line,
            const char      *format,
            va_list          args);

#endif // _RIAK_EXAMPLE_LOG_H
