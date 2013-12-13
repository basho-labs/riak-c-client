/*********************************************************************
 *
 * riak_get.c: Riak C Client Error Message
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

#include <unistd.h>
#include "riak.h"
#include "riak_messages.h"
#include "riak_messages-internal.h"
#include "riak_object-internal.h"
#include "riak_utils-internal.h"
#include "riak_config-internal.h"
#include "riak_operation-internal.h"
#include "riak_bucket_props-internal.h"
#include "riak_print-internal.h"

riak_error
riak_decode_error_response(riak_operation       *rop,
                           riak_pb_message      *pbresp,
                           riak_error_response **resp,
                           riak_boolean_t       *done) {
    riak_config *cfg = riak_operation_get_config(rop);
    RpbErrorResp *errresp = rpb_error_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (errresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }
    riak_error_response *response = (riak_error_response*)(cfg->malloc_fn)(sizeof(riak_error_response));
    *done = RIAK_TRUE;
    riak_free(cfg, &pbresp);
    if (response == NULL) {
        rpb_error_resp__free_unpacked(errresp, cfg->pb_allocator);
        return ERIAK_OUT_OF_MEMORY;
    }
    response->_internal = errresp;
    response->errcode = errresp->errcode;
    response->errmsg = riak_binary_populate_from_pb(cfg, &(errresp->errmsg));
    if (response->errmsg == NULL) {
        riak_free(cfg, &response);
        rpb_error_resp__free_unpacked(errresp, cfg->pb_allocator);
        return ERIAK_OUT_OF_MEMORY;
    }

    // Call user's error callback, if present
    if (rop->error_cb) {
        riak_response_callback cb = rop->error_cb;
        (cb)(response, rop->cb_data);
    }
    *resp = response;

    riak_error err = riak_server_error_new(cfg,
                                           &(rop->error),
                                           response->errcode,
                                           response->errmsg);

    if (err) {
        riak_free(cfg, &response);
        rpb_error_resp__free_unpacked(errresp, cfg->pb_allocator);
        return err;
    }

    return ERIAK_OK;
}

void
riak_free_error_response(riak_config          *cfg,
                         riak_error_response **resp) {
    riak_error_response *response = *resp;
    if (response == NULL) return;
    rpb_error_resp__free_unpacked(response->_internal, cfg->pb_allocator);
    riak_free(cfg, &(response->errmsg));
    riak_free(cfg, resp);
}
