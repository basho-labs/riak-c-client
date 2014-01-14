/*********************************************************************
 *
 * riak_log.c: Riak C Logging
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

#define _RIAK_LOG_DESCRIPTION

#include "riak.h"
#include "riak_log.h"
#include "riak_messages-internal.h"
#include "riak_config-internal.h"
#include "riak_connection-internal.h"

static void
riak_log_default(void            *ptr,
                 riak_log_level_t level,
                 const char      *file,
                 riak_size_t      filelen,
                 const char      *func,
                 riak_size_t      funclen,
                 riak_uint32_t    line,
                 const char      *format,
                 va_list          args) {

}

void
riak_log_internal(riak_config         *cfg,
                  riak_log_level_t     level,
                  const char          *file,
                  riak_size_t          filelen,
                  const char          *func,
                  riak_size_t          funclen,
                  riak_uint32_t        line,
                  const char          *format,
                  ...) {
    riak_log_fn log_fn = riak_log_default;
    if (cfg->log_fn) {
        log_fn = cfg->log_fn;
    }

    va_list va;
    va_start(va, format);
    (log_fn)((void*)cfg->log_data,
             level,
             file,
             filelen,
             func,
             funclen,
             line,
             format,
             va);
    va_end(va);
}
