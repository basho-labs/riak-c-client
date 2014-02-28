/*********************************************************************
 *
 * riak_log_config.h: Riak C Logging Configuration
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

#ifndef _RIAK_LOG_CONFIG_H
#define _RIAK_LOG_CONFIG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

// Levels correspond to RFC 5424
// http://en.wikipedia.org/wiki/Syslog#Severity_levels
typedef enum {
    /** emergency */     RIAK_LOG_EMERG,
    /** alert */         RIAK_LOG_ALERT,
    /** critical */      RIAK_LOG_CRITICAL,
    /** error */         RIAK_LOG_ERROR,
    /** warning */       RIAK_LOG_WARN,
    /** notice */        RIAK_LOG_NOTICE,
    /** informational */ RIAK_LOG_INFO,
    /** debug */         RIAK_LOG_DEBUG,
    /** unknown */       RIAK_LOG_UNKNOWN
} riak_log_level_t;

#ifdef _RIAK_LOG_DESCRIPTION
const char* riak_log_description[] = {
    "EMER",
    "ALRT",
    "CRIT",
    "ERRO",
    "WARN",
    "NOTE",
    "INFO",
    "DEBG",
    "UNKN"
};
#else
extern const char *riak_log_description[];
#endif
static inline const char*
riak_log_level_description(riak_log_level_t level) {
    if (level < RIAK_LOG_EMERG || level > RIAK_LOG_UNKNOWN) level = RIAK_LOG_UNKNOWN;
    return riak_log_description[level];
}

typedef riak_int32_t (*riak_log_init_fn)(void *ptr);
typedef void (*riak_log_cleanup_fn)(void *ptr);

/**
 * @brief Definition of logging function
 * @param ptr User-defined logging data
 * @param level Syslog-like logging message priority
 * @param file Name of file where message is issued
 * @param filelen Length of `file` field
 * @param func Name of function where message is issued
 * @param funclen Length of `func` field
 * @param line Line number where message is issued
 * @param format printf-like format string
 * @param args List of arguments for `format`
 */
typedef void (*riak_log_fn)(void            *ptr,
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

#endif // _RIAK_LOG_CONFIG_H
