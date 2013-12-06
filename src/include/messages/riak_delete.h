/*********************************************************************
 *
 * riak_messages.h: Riak C Client Delete Message
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

#ifndef _RIAK_DELETE_MESSAGE_H
#define _RIAK_DELETE_MESSAGE_H

typedef struct _riak_delete_options riak_delete_options;
typedef struct _riak_delete_response riak_delete_response;
typedef void (*riak_delete_response_callback)(riak_delete_response *response, void *ptr);

/**
 * @brief Free memory from response
 * @param cfg Riak Configuration
 * @param resp Delete PBC Response
 */
void
riak_free_delete_response(riak_config           *cfg,
                          riak_delete_response **resp);

#endif
