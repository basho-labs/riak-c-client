/*********************************************************************
 *
 * riak_counter.h: Riak C Client 1.4 Counters
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

#ifndef _RIAK_COUNTER_MESSAGE_H
#define _RIAK_COUNTER_MESSAGE_H

typedef struct _riak_counter_update_response riak_counter_update_response;
typedef struct _riak_counter_get_response riak_counter_get_response;

typedef void (*riak_counter_update_response_callback)(riak_counter_update_response *response, void *ptr);
typedef void (*riak_counter_get_response_callback)(riak_counter_get_response *response, void *ptr);

int
riak_print_counter_update_response(riak_counter_update_response *response,
                                   char              *target,
                                   riak_size_t        len);

void
riak_counter_update_response_free(riak_config        *cfg,
                                  riak_counter_update_response **resp);


int
riak_print_counter_get_response(riak_counter_get_response *response,
                                char              *target,
                                riak_size_t        len);

void
riak_counter_get_response_free(riak_config        *cfg,
                               riak_counter_get_response **resp);
#endif
