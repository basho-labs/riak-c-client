/*********************************************************************
 *
 * riak_get.c: Riak C Client Put Message
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
riak_encode_put_request(riak_operation   *rop,
                        riak_object      *riak_obj,
                        riak_put_options *options,
                        riak_pb_message **req) {

    riak_config *cfg = riak_operation_get_config(rop);
    RpbPutReq putmsg = RPB_PUT_REQ__INIT;

    riak_binary_to_pb_copy(&(putmsg.bucket), riak_obj->bucket);

    // Is the Key provided?
    if (riak_obj->has_key) {
        putmsg.has_key = RIAK_TRUE;
        riak_binary_to_pb_copy(&(putmsg.key), riak_obj->key);
    }

    // Data content payload
    RpbContent content;
    riak_object_to_pb_copy(cfg, &content, riak_obj);
    putmsg.content = &content;

    // process put options
    if (options != NULL) {
        if (options->has_asis) {
            putmsg.has_asis = RIAK_TRUE;
            putmsg.asis = options->asis;
        }
        if (options->has_dw) {
            putmsg.has_dw = RIAK_TRUE;
            putmsg.dw = options->dw;
        }
        if (options->has_if_none_match) {
            putmsg.has_if_none_match = RIAK_TRUE;
            putmsg.has_if_none_match = options->has_if_none_match;
        }
        if (options->has_if_not_modified) {
            putmsg.has_if_not_modified = RIAK_TRUE;
            putmsg.has_if_not_modified = options->has_if_not_modified;
        }
        if (options->has_n_val) {
            putmsg.has_n_val = RIAK_TRUE;
            putmsg.n_val = options->n_val;
        }
        if (options->has_pw) {
            putmsg.has_pw = RIAK_TRUE;
            putmsg.pw = options->pw;
        }
        if (options->has_return_body) {
            putmsg.has_return_body = RIAK_TRUE;
            putmsg.return_body = options->return_body;
        }
        if (options->has_return_head) {
            putmsg.has_return_head = RIAK_TRUE;
            putmsg.return_head = options->return_head;
        }
        if (options->has_sloppy_quorum) {
            putmsg.has_sloppy_quorum = RIAK_TRUE;
            putmsg.sloppy_quorum = options->sloppy_quorum;
        }
        if (options->has_timeout) {
            putmsg.has_timeout = RIAK_TRUE;
            putmsg.timeout = options->timeout;
        }
        if (options->has_vclock) {
            putmsg.has_vclock = RIAK_TRUE;
            riak_binary_to_pb_copy(&(putmsg.vclock), options->vclock);
        }
        if (options->has_w) {
            putmsg.has_w = RIAK_TRUE;
            putmsg.w = options->w;
        }
    }

    riak_uint32_t msglen = rpb_put_req__get_packed_size (&putmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_put_req__pack (&putmsg, msgbuf);

    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBPUTREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_decode_put_response);

    return ERIAK_OK;
}

void
riak_free_put_request(riak_config        *cfg,
                      riak_put_response **resp) {
    riak_put_response *response = *resp;
    if (response == NULL) return;
    rpb_put_resp__free_unpacked(response->_internal, cfg->pb_allocator);
    riak_free(cfg, resp);
}

riak_error
riak_decode_put_response(riak_operation     *rop,
                         riak_pb_message    *pbresp,
                         riak_put_response **resp,
                         riak_boolean_t     *done) {
    // decode the PB response etc
    riak_config *cfg = riak_operation_get_config(rop);
    RpbPutResp *rpbresp = rpb_put_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    *done = RIAK_TRUE;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_log_debug(cxn, "riak_decode_put_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i = 0;
    riak_put_response *response = (riak_put_response*)(cfg->malloc_fn)(sizeof(riak_put_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_put_response));
    response->_internal = rpbresp;
    if (rpbresp->has_vclock) {
        response->has_vclock = RIAK_TRUE;
        response->vclock = riak_binary_populate_from_pb(cfg, &(rpbresp->vclock));
        if (response->vclock == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (rpbresp->has_key) {
        response->has_key = RIAK_TRUE;
        response->key = riak_binary_populate_from_pb(cfg, &(rpbresp->key));
        if (response->key == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (rpbresp->n_content > 0) {
        riak_error err = riak_object_new_array(cfg, &(response->content), rpbresp->n_content);
        if (err != ERIAK_OK) {
            riak_free(cfg, &response);
            return err;
        }
        response->n_content = rpbresp->n_content;
        for(i = 0; i < rpbresp->n_content; i++) {
            err = riak_object_new_from_pb(cfg, &(response->content[i]), rpbresp->content[i]);
            // If any object allocation fails, clean up all previously allocated ones
            if (err != ERIAK_OK) {
                riak_object_free_array(cfg, &(response->content), i);
                riak_free(cfg, &response);
                return err;
            }
        }
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_print_put_response(riak_put_response *response,
                        char              *target,
                        riak_size_t        len) {

    char buffer[1024];
    riak_int32_t wrote;
    riak_int32_t left_to_write = len;
    if ((left_to_write > 0) && (response->has_vclock)) {
        riak_binary_hex_print(response->vclock, buffer, sizeof(buffer));
        wrote = snprintf(target, left_to_write, "V-Clock: %s\n", buffer);
        left_to_write -= wrote;
        target += wrote;
    }
    if ((left_to_write > 0) && (response->has_key)) {
        riak_binary_print(response->key, buffer, sizeof(buffer));
        wrote = snprintf(target, left_to_write, "Key: %s\n", buffer);
        left_to_write -= wrote;
        target += wrote;
    }
    if (left_to_write > 0) {
        wrote = snprintf(target, left_to_write, "Objects: %d\n", response->n_content);
        left_to_write -= wrote;
        target += wrote;
    }
    riak_uint32_t i;
    for(i = 0; (i < response->n_content) && (left_to_write > 0); i++) {
        wrote = riak_object_print(response->content[i], target, left_to_write);
    }
}

void
riak_free_put_response(riak_config        *cfg,
                       riak_put_response **resp) {
    riak_put_response *response = *resp;
    if (response == NULL) return;
    if (response->n_content > 0) {
        riak_object_free_array(cfg, &(response->content), response->n_content);
    }
    riak_free(cfg, &(response->key));
    riak_free(cfg, &(response->vclock));
    rpb_put_resp__free_unpacked(response->_internal, cfg->pb_allocator);
    riak_free(cfg, resp);
}
