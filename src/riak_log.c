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

#include <zlog.h>
#include "riak.h"
#include "riak_log.h"
#include "riak_messages-internal.h"
#include "riak_context-internal.h"
#include "riak_event-internal.h"


const zlog_level map_level_to_priority[] = {
    ZLOG_LEVEL_FATAL,
    ZLOG_LEVEL_ERROR,
    ZLOG_LEVEL_WARN,
    ZLOG_LEVEL_NOTICE,
    ZLOG_LEVEL_INFO,
    ZLOG_LEVEL_DEBUG,
    0
};

// TODO: Create an async queue of log messages to make threadsafe(r)
void
riak_log_internal(riak_context    *ctx,
                  riak_log_level_t level,
                  const char      *file,
                  size_t           filelen,
                  const char      *func,
                  size_t           funclen,
                  long             line,
                  const char      *format,
                  ...) {
    if (level < RIAK_LOG_FATAL || level > RIAK_LOG_UNKNOWN) level = RIAK_LOG_UNKNOWN;
    // Map to underlying zlog priorities
    zlog_level priority = map_level_to_priority[(int)level];
    riak_boolean_t has_logging = RIAK_FALSE;
    zlog_category_t *category;
    if (ctx->logging_category[0] != '\0') {
        category = zlog_get_category(ctx->logging_category);
        has_logging = RIAK_TRUE;
    }

    va_list va;
    va_start(va, format);
    if (has_logging) {
        vzlog(category,
              file,
              filelen,
              func,
              funclen,
              line,
              priority,
              format,
              va);
    } else {
        vfprintf(stderr, format, va);
    }
    va_end(va);
}
