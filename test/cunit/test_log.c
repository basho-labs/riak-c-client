/*********************************************************************
 *
 * test_log.c:  Riak C Unit Testing logger
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
#include <time.h>
#include "riak.h"

void
test_log(void            *ptr,
         riak_log_level_t level,
         const char      *file,
         riak_size_t      filelen,
         const char      *func,
         riak_size_t      funclen,
         riak_uint32_t    line,
         const char      *format,
         va_list          args) {
    time_t ltime;
    struct tm result;
    char stime[32];
    char output[2048];

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    strftime(stime, sizeof(stime), "%F %X %Z", &result);
    riak_size_t wrote = snprintf(output, sizeof(output), "%s %s ", stime, riak_log_level_description(level));
    char *pos = output + wrote;
    wrote += vsnprintf(pos, sizeof(output)-wrote, format, args);
    if (wrote < sizeof(output)-1) {
        strcat(output, "\n");
    }
    // Just spew any output to stderr
    fprintf(stderr, "%s", output);
}
