/*********************************************************************
 *
 * test_logging.h: Riak C Integration Logging
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

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "riak.h"

#ifndef _RIAK_TEST_LOGGING_H
#define _RIAK_TEST_LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FILE *fp;
} test_log_data;

/**
 * @brief Set up logging
 * @pararm ptr Opaque pointer to logging state
 * @returns 0 upon success
 */
riak_int32_t
test_log_init(void *ptr);

/**
 * @brief Clean up after logging is complete
 * @param ptr Opaque pointer to logging state
 */
void
test_log_cleanup(void *ptr);

/**
 * @brief Add a record to the Riak test log
 * @param ptr Opaque pointer to logging state
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
test_log(void            *ptr,
         riak_log_level_t level,
         const char      *file,
         riak_size_t      filelen,
         const char      *func,
         riak_size_t      funclen,
         riak_uint32_t    line,
         const char      *format,
         va_list          args);

#ifdef __cplusplus
}
#endif

#endif // _RIAK_TEST_LOGGING_H
