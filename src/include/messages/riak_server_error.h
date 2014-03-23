/*********************************************************************
 *
 * riak_error.h: Riak C Client Error Message
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

#ifndef _RIAK_ERROR_MESSAGE_H
#define _RIAK_ERROR_MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _riak_server_error_response riak_server_error_response;

/**
 * @brief Free memory used by an error response
 * @param cfg Riak Configuration
 * @param resp Error structure to be freed
 */
void
riak_server_error_response_free(riak_config                 *cfg,
                                riak_server_error_response **resp);

#ifdef __cplusplus
}
#endif

#endif
