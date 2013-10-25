/*********************************************************************
 *
 * riak_log.h: Riak C Logging
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

#ifndef RIAK_LOG_H_
#define RIAK_LOG_H_

#include <stdarg.h>

// Pulled from zlog.h but correspond to log levels from syslog(3)
typedef enum {
    /** fatal */    RIAK_LOG_FATAL,
    /** error */    RIAK_LOG_ERROR,
    /** warn */     RIAK_LOG_WARN,
    /** notice */   RIAK_LOG_NOTICE,
    /** info */     RIAK_LOG_INFO,
    /** debug */    RIAK_LOG_DEBUG,
    /** unknown */  RIAK_LOG_UNKNOWN
} riak_log_level_t;


/**
 * @brief Add a record to the Riak log
 * @param rev Riak Event
 * @param level Logging level (see `riak_log_level_t`)
 * @param format Printf-like formatting string
 * @param ... Arguments to `format`
 */

void
riak_log(riak_event *rev,
         riak_log_level_t level,
         const char *format, ...);

/**
 * @brief Add a record to the Riak log
 * @param ctx Riak Context
 * @param level Logging level (see `riak_log_level_t`)
 * @param format Printf-like formatting string
 * @param ... Arguments to `format`
 */
void
riak_log_context(riak_context *ctx,
                 riak_log_level_t level,
                 const char *format, ...);

#endif
