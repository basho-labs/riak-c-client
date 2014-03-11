/*********************************************************************
 *
 * example_log.c: Rxample Riak C Client Logging Functions
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

#include "riak.h"
#include "example_log.h"

riak_int32_t
example_log_init(void *ptr) {
    example_log_data* datum = (example_log_data*)ptr;
    datum->fp = fopen("riak.log", "w+");
    if (datum->fp == NULL) {
        return 1;
    }
    return 0;
}

void
example_log_cleanup(void *ptr) {
    example_log_data* datum = (example_log_data*)ptr;
    fclose(datum->fp);
}

void
example_log(void            *ptr,
            riak_log_level_t level,
            const char      *file,
            riak_size_t      filelen,
            const char      *func,
            riak_size_t      funclen,
            riak_uint32_t    line,
            const char      *format,
            va_list          args) {
    example_log_data* datum = (example_log_data*)ptr;
    time_t ltime;
    struct tm result;
    char stime[32];
    char output[2048];

    if (datum->fp == NULL) return;

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    strftime(stime, sizeof(stime), "%F %X %Z", &result);
    riak_size_t wrote = snprintf(output, sizeof(output), "%s %s ", stime, riak_log_level_description(level));
    char *pos = output + wrote;
    wrote += vsnprintf(pos, sizeof(output)-wrote, format, args);
    if (wrote < sizeof(output)-1) {
        strcat(output, "\n");
    }
    fprintf(datum->fp, "%s", output);
    if (level == RIAK_LOG_CRITICAL) {
        fprintf(stderr, "%s", output);
    }
}

