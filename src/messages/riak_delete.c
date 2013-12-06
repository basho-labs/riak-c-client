/*********************************************************************
 *
 * riak_get.c: Riak C Client Delete Message
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
riak_encode_delete_request(riak_operation      *rop,
                           riak_binary         *bucket,
                           riak_binary         *key,
                           riak_delete_options *options,
                           riak_pb_message    **req) {

    riak_config *cfg = riak_operation_get_config(rop);
    RpbDelReq delmsg = RPB_DEL_REQ__INIT;

    riak_binary_to_pb_copy(&delmsg.bucket, bucket);
    riak_binary_to_pb_copy(&delmsg.key, key);

    // process delete options
    if (options != NULL) {
        if (options->has_dw) {
            delmsg.has_dw = RIAK_TRUE;
            delmsg.dw = options->dw;
        }
        if (options->has_n_val) {
            delmsg.has_n_val = RIAK_TRUE;
            delmsg.n_val = options->n_val;
        }
        if (options->has_pw) {
            delmsg.has_pw = RIAK_TRUE;
            delmsg.pw = options->pw;
        }

        if (options->has_sloppy_quorum) {
            delmsg.has_sloppy_quorum = RIAK_TRUE;
            delmsg.sloppy_quorum = options->sloppy_quorum;
        }
        if (options->has_timeout) {
            delmsg.has_timeout = RIAK_TRUE;
            delmsg.timeout = options->timeout;
        }
        if (options->has_vclock) {
            delmsg.has_vclock = RIAK_TRUE;
            riak_binary_to_pb_copy(&delmsg.vclock, options->vclock);
        }
        if (options->has_w) {
            delmsg.has_w = RIAK_TRUE;
            delmsg.w = options->w;
        }
    }

    riak_uint32_t msglen = rpb_del_req__get_packed_size (&delmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_del_req__pack (&delmsg, msgbuf);

    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBDELREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_decode_delete_response);

    return ERIAK_OK;
}


riak_error
riak_decode_delete_response(riak_operation        *rop,
                            riak_pb_message       *pbresp,
                            riak_delete_response **resp,
                            riak_boolean_t        *done) {
    riak_config *cfg = riak_operation_get_config(rop);
    riak_delete_response *response = (riak_delete_response*)(cfg->malloc_fn)(sizeof(riak_delete_response));
    *done = RIAK_TRUE;
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_free_delete_response(riak_config           *cfg,
                          riak_delete_response **resp) {
    riak_free(cfg, resp);
}
