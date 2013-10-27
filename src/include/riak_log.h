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
 * @param ctx Riak Context
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
riak_log_internal(riak_context    *ctx,
                  riak_log_level_t level,
                  const char      *file,
                  size_t           filelen,
                  const char      *func,
                  size_t           funclen,
                  long             line,
                  const char      *format,
                  ...);

#define riak_log_fatal(rev,format, ...) \
        riak_log_internal(riak_event_get_context(rev), RIAK_LOG_FATAL, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_event_get_fd(rev), __VA_ARGS__)
#define riak_log_error(rev,format, ...) \
        riak_log_internal(riak_event_get_context(rev), RIAK_LOG_ERROR, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_event_get_fd(rev), __VA_ARGS__)
#define riak_log_warn(rev,format, ...) \
        riak_log_internal(riak_event_get_context(rev), RIAK_LOG_WARN, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_event_get_fd(rev), __VA_ARGS__)
#define riak_log_notice(rev,format, ...) \
        riak_log_internal(riak_event_get_context(rev), RIAK_LOG_NOTICE, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_event_get_fd(rev), __VA_ARGS__)
#ifdef _RIAK_DEBUG
#define riak_log_debug(rev,format, ...) \
        riak_log_internal(riak_event_get_context(rev), RIAK_LOG_DEBUG, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_event_get_fd(rev), __VA_ARGS__)
#else
#define riak_log_debug(rev,format, ...) // No-Op
#endif

#define riak_log_fatal_context(ctx,format, ...) \
        riak_log_internal((ctx), RIAK_LOG_FATAL, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#define riak_log_error_context(rev,format, ...) \
        riak_log_internal((ctx), RIAK_LOG_ERROR, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#define riak_log_warn_context(rev,format, ...) \
        riak_log_internal(ctx), RIAK_LOG_WARN, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#define riak_log_notice_context(rev,format, ...) \
        riak_log_internal((ctx), RIAK_LOG_NOTICE, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#ifdef _RIAK_DEBUG
#define riak_log_debug_context(ctx,format, ...) \
        riak_log_internal((ctx), RIAK_LOG_DEBUG, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#else
#define riak_log_debug_context(rev,format, ...)
#endif

#endif
