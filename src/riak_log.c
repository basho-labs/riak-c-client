/*********************************************************************
 *
 * riak_log.c: Riak C Logging
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

#include "riak.h"
#include "riak_log.h"
#include "riak_messages-internal.h"
#include "riak_context-internal.h"
#include "riak_event-internal.h"

const log4c_priority_level_t map_level_to_priority[] = {
    LOG4C_PRIORITY_FATAL,
    LOG4C_PRIORITY_ALERT,
    LOG4C_PRIORITY_CRIT,
    LOG4C_PRIORITY_ERROR,
    LOG4C_PRIORITY_WARN,
    LOG4C_PRIORITY_NOTICE,
    LOG4C_PRIORITY_INFO,
    LOG4C_PRIORITY_DEBUG,
    LOG4C_PRIORITY_TRACE,
    LOG4C_PRIORITY_NOTSET,
    LOG4C_PRIORITY_UNKNOWN
};

// TODO: Create an async queue of log messages to make threadsafe(r)
void riak_log(riak_event *rev, riak_log_level_t level, const char *format, ...) {
    if (level < RIAK_LOG_FATAL || level > RIAK_LOG_UNKNOWN) level = RIAK_LOG_UNKNOWN;
    // Map to underlying log4c priorities
    log4c_priority_level_t priority = map_level_to_priority[(int)level];
    log4c_category_t *category = log4c_category_get(rev->context->logging_category);
    // Prefix with the file descriptor
    // TODO: Get rid of copying format string
    char formatted[2048];
    snprintf(formatted, sizeof(formatted), "[%d] %s", rev->fd, format);

    va_list va;
    va_start(va, format);
    log4c_category_vlog(category, priority, formatted, va);
    va_end(va);
}

// TODO: Create an async queue of log messages to make threadsafe(r)
void riak_log_context(riak_context *ctx, riak_log_level_t level, const char *format, ...) {
    if (level < RIAK_LOG_FATAL || level > RIAK_LOG_UNKNOWN) level = RIAK_LOG_UNKNOWN;
    // Map to underlying log4c priorities
    log4c_priority_level_t priority = map_level_to_priority[(int)level];
    log4c_category_t *category = log4c_category_get(ctx->logging_category);

    va_list va;
    va_start(va, format);
    log4c_category_vlog(category, priority, format, va);
    va_end(va);
}
