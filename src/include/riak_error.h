/*********************************************************************
 *
 * riak_error.h: Riak C Error Handling
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

#ifndef RIAK_ERROR_H_
#define RIAK_ERROR_H_

typedef enum riak_error_enum {
    ERIAK_OK = 0,
    ERIAK_DNS_RESOLUTION,
    ERIAK_OUT_OF_MEMORY,
    ERIAK_WRITE,
    ERIAK_EVENT,
    ERIAK_NO_PING,
    ERIAK_LOGGING,
    ERIAK_LAST_ERRORNUM
} riak_error;

#ifdef _RIAK_ERROR_DEFINE_MSGS
static const char* errmsgs[] = {
    "No Error",
    "Problems resolving host name/port number",
    "Out of Memory",
    "Write error",
    "Riak Event error",
    "Ping failure error",
    "Logging failure",
    "SENTINEL FOR LAST ERROR MESSAGE"
};
#endif

const char*
riak_strerror(riak_error err);

#endif
