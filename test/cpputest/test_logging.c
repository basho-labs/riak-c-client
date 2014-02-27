/*********************************************************************
 *
 * test_logging.c: Riak C Integration Logging
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

#include "test_logging.h"
#include <time.h>

// log initialization callback, open files here etc.
riak_int32_t
test_log_init(void *ptr) {
    test_log_data* datum = (test_log_data*)ptr;
    datum->fp = fopen("riak_test.log", "a+");
    if (datum->fp == NULL) {
        return 1;
    }
    return 0;
}

// log cleanup callback, close files here etc.
void
test_log_cleanup(void *ptr) {
    test_log_data* datum = (test_log_data*)ptr;
    fclose(datum->fp);
}

// handle an incoming log message
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
    test_log_data* datum = (test_log_data*)ptr;
    time_t ltime;
    struct tm result;
    char stime[32];

    if (datum->fp == NULL) return;

    ltime = time(NULL);
    localtime_r(&ltime, &result);
    strftime(stime, sizeof(stime), "%F %X %Z", &result);
    fprintf(datum->fp, "%s %s ", stime, riak_log_level_description(level));
    vfprintf(datum->fp, format, args);
    fprintf(datum->fp, "\n");
    fflush(datum->fp);
}
