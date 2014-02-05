/*********************************************************************
 *
 * riak_log.h: Riak C Logging
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

#ifndef _RIAK_LOG_H
#define _RIAK_LOG_H

/**
 * @brief Add a record to the Riak log
 * @param cfg Riak Configuration
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
riak_log_internal(riak_config         *cfg,
                  riak_log_level_t     level,
                  const char          *file,
                  riak_size_t          filelen,
                  const char          *func,
                  riak_size_t          funclen,
                  riak_uint32_t        line,
                  const char          *format,
                  ...);

#define riak_log_emergency(cxn,format, ...) \
        riak_log_internal(riak_connection_get_config(cxn), RIAK_LOG_EMERG, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_connection_get_fd(cxn), __VA_ARGS__)
#define riak_log_alert(cxn,format, ...) \
        riak_log_internal(riak_connection_get_config(cxn), RIAK_LOG_ALERT, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_connection_get_fd(cxn), __VA_ARGS__)
#define riak_log_critical(cxn,format, ...) \
        riak_log_internal(riak_connection_get_config(cxn), RIAK_LOG_CRITICAL, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_connection_get_fd(cxn), __VA_ARGS__)
#define riak_log_error(cxn,format, ...) \
        riak_log_internal(riak_connection_get_config(cxn), RIAK_LOG_ERROR, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_connection_get_fd(cxn), __VA_ARGS__)
#define riak_log_warn(cxn,format, ...) \
        riak_log_internal(riak_connection_get_config(cxn), RIAK_LOG_WARN, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_connection_get_fd(cxn), __VA_ARGS__)
#define riak_log_notice(cxn,format, ...) \
        riak_log_internal(riak_connection_get_config(cxn), RIAK_LOG_NOTICE, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_connection_get_fd(cxn), __VA_ARGS__)
#ifdef _RIAK_DEBUG
#define riak_log_debug(cxn,format, ...) \
        riak_log_internal(riak_connection_get_config(cxn), RIAK_LOG_DEBUG, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, ("[%d] " format), riak_connection_get_fd(cxn), __VA_ARGS__)
#else
#define riak_log_debug(cxn,format, ...) ((void)cxn)// No-Op
#endif

#define riak_log_emergency_config(cfg,format, ...) \
        riak_log_internal((cfg), RIAK_LOG_EMERG, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#define riak_log_alert_config(cfg,format, ...) \
        riak_log_internal((cfg), RIAK_LOG_ALERT, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#define riak_log_critical_config(cfg,format, ...) \
        riak_log_internal((cfg), RIAK_LOG_CRITICAL, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#define riak_log_error_config(cxn,format, ...) \
        riak_log_internal((cfg), RIAK_LOG_ERROR, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#define riak_log_warn_config(cxn,format, ...) \
        riak_log_internal(cfg), RIAK_LOG_WARN, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#define riak_log_notice_config(cxn,format, ...) \
        riak_log_internal((cfg), RIAK_LOG_NOTICE, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#ifdef _RIAK_DEBUG
#define riak_log_debug_config(cfg,format, ...) \
        riak_log_internal((cfg), RIAK_LOG_DEBUG, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, (format), __VA_ARGS__)
#else
#define riak_log_debug_config(cxn,format, ...) ((void)cxn)
#endif

#endif //_RIAK_LOG_H
