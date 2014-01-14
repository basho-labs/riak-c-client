/*********************************************************************
 *
 * riak_get.c: Riak C Client Ping Message
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

#include <unistd.h>
#include "riak.h"
#include "riak_messages.h"
#include "riak_messages-internal.h"
#include "riak_object-internal.h"
#include "riak_utils-internal.h"
#include "riak_config-internal.h"
#include "riak_operation-internal.h"
#include "riak_print-internal.h"

riak_error
riak_encode_ping_request(riak_operation   *rop,
                         riak_pb_message **req) {
    riak_config *cfg = riak_operation_get_config(rop);
    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBPINGREQ, 0, NULL);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_decode_ping_response);

    return ERIAK_OK;
}


riak_error
riak_decode_ping_response(riak_operation      *rop,
                          riak_pb_message     *pbresp,
                          riak_ping_response **resp,
                          riak_boolean_t      *done) {
    riak_config *cfg = riak_operation_get_config(rop);
    riak_ping_response *response = (riak_ping_response*)(cfg->malloc_fn)(sizeof(riak_ping_response));
    *done = RIAK_TRUE;
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    response->success = RIAK_TRUE;
    *resp = response;

    return ERIAK_OK;
}

void
riak_free_ping_response(riak_config         *cfg,
                        riak_ping_response **resp) {
    riak_free(cfg, resp);
}
