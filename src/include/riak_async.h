/*********************************************************************
 *
 * riak_async.h: Riak C Client Asynchronous Utilities
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

#ifndef _RIAK_ASYNC_H
#define _RIAK_ASYNC_H

#include <event2/dns.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/thread.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct _riak_libevent riak_libevent;

/**
 * @brief Create a new Libevent Event
 * @param Resulting Riak Libevent Event
 * @param Riak Operation
 * @param Libevent Event Base
 * @returns Error Code
 */
riak_error
riak_libevent_new(riak_libevent    **rev,
                  riak_operation    *rop,
                  struct event_base *base);

/**
 * @brief Free the memory claimed by Riak Libevent Event
 * @param Riak Configuration
 * @param Riak Libevent Event
 */
void
riak_libevent_free(riak_config    *cfg,
                   riak_libevent **rev);

/**
 * @brief Send an asynchronous message
 * @param Riak Operation
 * @param Riak Libevent Event
 * @returns Error Code
 */
riak_error
riak_libevent_send(riak_operation *rop,
                    riak_libevent *rev);

#ifdef __cplusplus
}
#endif

#endif // _RIAK_ASYNC_H
