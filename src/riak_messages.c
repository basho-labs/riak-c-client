/*********************************************************************
 *
 * riak_pb_message.c: Riak C Client Protocol Buffer Message
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
#include "riak_context-internal.h"
#include "riak_event-internal.h"
#include "riak_bucket_props-internal.h"

riak_pb_message*
riak_pb_message_new(riak_context *ctx,
                    riak_uint8_t  msgtype,
                    riak_size_t   msglen,
                    riak_uint8_t *buffer) {
    riak_pb_message *pb = (riak_pb_message*)(ctx->malloc_fn)(sizeof(riak_pb_message));
    if (pb != NULL) {
        memset((void*)pb, '\0', sizeof(riak_pb_message));
        pb->msgid   = msgtype;
        pb->len     = msglen;
        pb->data    = buffer;
    }
    return pb;
}

void
riak_pb_message_free(riak_context     *ctx,
                     riak_pb_message **pb) {
    riak_free(ctx, &((*pb)->data));
    riak_free(ctx, pb);
}

riak_error
riak_decode_error_response(riak_event           *rev,
                           riak_pb_message      *pbresp,
                           riak_error_response **resp,
                           riak_boolean_t       *done) {
    riak_context *ctx = (riak_context*)(rev->context);
    RpbErrorResp *errresp = rpb_error_resp__unpack(ctx->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    riak_error_response *response = (riak_error_response*)(ctx->malloc_fn)(sizeof(riak_error_response));
    *done = RIAK_TRUE;
    riak_free(ctx, &pbresp);
    if (response == NULL) {
        rpb_error_resp__free_unpacked(errresp, ctx->pb_allocator);
        return ERIAK_OUT_OF_MEMORY;
    }
    response->_internal = errresp;
    response->errcode = errresp->errcode;
    response->errmsg = riak_binary_populate_from_pb(ctx, &(errresp->errmsg));
    if (response->errmsg == NULL) {
        riak_free(ctx, &response);
        rpb_error_resp__free_unpacked(errresp, ctx->pb_allocator);
        return ERIAK_OUT_OF_MEMORY;
    }

    // Call user's error callback, if present
    if (rev->error_cb) {
        riak_response_callback cb = rev->error_cb;
        (cb)(response, rev->cb_data);
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_free_error_response(riak_context         *ctx,
                         riak_error_response **resp) {
    riak_error_response *response = *resp;
    rpb_error_resp__free_unpacked(response->_internal, ctx->pb_allocator);
    riak_free(ctx, &(response->errmsg));
    riak_free(ctx, resp);
}

riak_error
riak_encode_ping_request(riak_event      *rev,
                        riak_pb_message **req) {
    riak_context *ctx = (riak_context*)(rev->context);
    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBPINGREQ, 0, NULL);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_ping_response);

    return ERIAK_OK;
}


riak_error
riak_decode_ping_response(riak_event          *rev,
                          riak_pb_message     *pbresp,
                          riak_ping_response **resp,
                          riak_boolean_t      *done) {
    riak_context *ctx = (riak_context*)(rev->context);
    riak_ping_response *response = (riak_ping_response*)(ctx->malloc_fn)(sizeof(riak_ping_response));
    *done = RIAK_TRUE;
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    response->success = RIAK_TRUE;
    *resp = response;

    return ERIAK_OK;
}

void
riak_free_ping_response(riak_context        *ctx,
                        riak_ping_response **resp) {
    riak_free(ctx, resp);
}

riak_error
riak_encode_serverinfo_request(riak_event       *rev,
                               riak_pb_message **req) {
    riak_context *ctx = (riak_context*)(rev->context);
    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBGETSERVERINFOREQ, 0, NULL);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_serverinfo_response);

    return ERIAK_OK;
}

riak_error
riak_decode_serverinfo_response(riak_event                *rev,
                                riak_pb_message           *pbresp,
                                riak_serverinfo_response **resp,
                                riak_boolean_t            *done) {
    // decode the PB response etc
    riak_context *ctx = (riak_context*)(rev->context);
    RpbGetServerInfoResp *rpbresp = rpb_get_server_info_resp__unpack(ctx->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    riak_log_debug(rev, "riak_decode_server_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    *done = RIAK_TRUE;
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_serverinfo_response *response = (riak_serverinfo_response*)(ctx->malloc_fn)(sizeof(riak_serverinfo_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_serverinfo_response));
    response->_internal = rpbresp;

    if (rpbresp->has_node) {
        response->has_node = RIAK_TRUE;
        response->node = riak_binary_populate_from_pb(ctx, &(rpbresp->node));
        if (response->node == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (rpbresp->has_server_version) {
        response->has_server_version = RIAK_TRUE;
        response->server_version = riak_binary_populate_from_pb(ctx, &(rpbresp->server_version));
        if (response->server_version == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_print_serverinfo_response(riak_serverinfo_response *response,
                               char                     *target,
                               riak_size_t               len) {
    riak_int32_t left_to_write = len;
    riak_int32_t wrote;
    char buffer[1024];
    if ((response->has_node) && (left_to_write > 0)) {
        riak_binary_print(response->node, buffer, sizeof(buffer));
        wrote = snprintf(target, left_to_write, "Node: %s\n", buffer);
        left_to_write -= wrote;
        target += wrote;
    }
    if ((response->has_server_version) && (left_to_write > 0)) {
        riak_binary_print(response->server_version, buffer, sizeof(buffer));
        wrote = snprintf(target, left_to_write, "Version: %s\n", buffer);
        left_to_write -= wrote;
        target += wrote;
    }
}

void
riak_free_serverinfo_response(riak_context              *ctx,
                              riak_serverinfo_response **resp) {
    riak_serverinfo_response *response = *resp;
    rpb_get_server_info_resp__free_unpacked(response->_internal, ctx->pb_allocator);
    riak_free(ctx, &(response->node));
    riak_free(ctx, &(response->server_version));
    riak_free(ctx, resp);
}

riak_error
riak_encode_get_request(riak_event       *rev,
                        riak_binary      *bucket,
                        riak_binary      *key,
                        riak_get_options *get_options,
                        riak_pb_message **req) {

    riak_context *ctx = (riak_context*)(rev->context);
    RpbGetReq getmsg = RPB_GET_REQ__INIT;

    riak_binary_to_pb_copy(&getmsg.bucket, bucket);
    riak_binary_to_pb_copy(&getmsg.key, key);

    // process get options
    if(get_options != NULL) {
        getmsg.has_r = get_options->has_r;
        getmsg.r = get_options->r;
        getmsg.has_pr = get_options->has_pr;
        getmsg.pr = get_options->pr;
        getmsg.has_basic_quorum = get_options->has_basic_quorum;
        getmsg.basic_quorum = get_options->basic_quorum;
        getmsg.has_notfound_ok = get_options->has_notfound_ok;
        getmsg.notfound_ok = get_options->notfound_ok;
        if (get_options->has_if_modified) {
            getmsg.has_if_modified = get_options->has_if_modified;
            riak_binary_to_pb_copy(&getmsg.if_modified, get_options->if_modified);
        }
        getmsg.has_head = get_options->has_head;
        getmsg.head = get_options->head;
        getmsg.has_deletedvclock = get_options->has_deletedvclock;
        getmsg.deletedvclock = get_options->deletedvclock;
        getmsg.has_timeout = get_options->has_timeout;
        getmsg.timeout = get_options->timeout;
        getmsg.has_sloppy_quorum = get_options->has_sloppy_quorum;
        getmsg.sloppy_quorum = get_options->sloppy_quorum;
        getmsg.has_n_val = get_options->has_n_val;
        getmsg.n_val = get_options->n_val;
    }
    riak_uint32_t msglen = rpb_get_req__get_packed_size (&getmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_get_req__pack (&getmsg, msgbuf);
    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBGETREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_get_response);

    return ERIAK_OK;
}

riak_error
riak_decode_get_response(riak_event         *rev,
                         riak_pb_message    *pbresp,
                         riak_get_response **resp,
                         riak_boolean_t     *done) {
    // decode the PB response etc
    riak_context *ctx = (riak_context*)(rev->context);
    RpbGetResp *rpbresp = rpb_get_resp__unpack(ctx->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    riak_log_debug(rev, "riak_decode_get_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    *done = RIAK_TRUE;
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i = 0;
    riak_get_response *response = (riak_get_response*)(ctx->malloc_fn)(sizeof(riak_get_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_get_response));
    response->_internal = rpbresp;

    if (rpbresp->has_vclock) {
        response->has_vclock = RIAK_TRUE;
        response->vclock = riak_binary_populate_from_pb(ctx, &(rpbresp->vclock));
        if (response->vclock == NULL) {
            riak_free(ctx, &response);
            return ERIAK_OUT_OF_MEMORY;
        }   }
    if (rpbresp->has_unchanged) {
        response->has_unmodified = RIAK_TRUE;
        response->unmodified = rpbresp->unchanged;
    }
    if (rpbresp->n_content > 0) {
        riak_error err = riak_object_new_array(ctx, &(response->content), rpbresp->n_content);
        if (err != ERIAK_OK) {
            riak_free(ctx, &response);
            return 1;
        }
        response->n_content = rpbresp->n_content;
        for(i = 0; i < rpbresp->n_content; i++) {
            err = riak_object_new_from_pb(ctx, &(response->content[i]), rpbresp->content[i]);
            // If any object allocation fails, clean up all previously allocated ones
            if (err != ERIAK_OK) {
                riak_object_free_array(ctx, &(response->content), i);
                riak_free(ctx, &response);
                return err;
            }
        }
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_print_get_response(riak_get_response *response,
                        char              *target,
                        riak_size_t        len) {
    riak_int32_t left_to_write = len;
    riak_int32_t wrote;
    char buffer[1024];
    riak_binary_hex_print(response->vclock, buffer, sizeof(buffer));
    if (left_to_write > 0) {
        wrote = snprintf(target, left_to_write, "V-Clock: %s\n", buffer);
        left_to_write -= wrote;
        target += wrote;
    }
    if (left_to_write > 0) {
        wrote = snprintf(target, left_to_write, "Unmodified: %s\n", (response->unmodified) ? "true" : "false");
        left_to_write -= wrote;
        target += wrote;
    }
    if (left_to_write > 0) {
        wrote = snprintf(target, left_to_write, "Deleted: %s\n", (response->deleted) ? "true" : "false");
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
riak_free_get_response(riak_context       *ctx,
                       riak_get_response **resp) {
    riak_get_response *response = *resp;
    if (response->n_content > 0) {
        riak_object_free_array(ctx, &(response->content), response->n_content);
    }
    riak_free(ctx, &(response->vclock));
    rpb_get_resp__free_unpacked(response->_internal, ctx->pb_allocator);
    riak_free(ctx, resp);
}


riak_error
riak_encode_put_request(riak_event       *rev,
                        riak_object      *riak_obj,
                        riak_put_options *options,
                        riak_pb_message **req) {

    riak_context *ctx = (riak_context*)(rev->context);
    RpbPutReq putmsg = RPB_PUT_REQ__INIT;

    riak_binary_to_pb_copy(&(putmsg.bucket), riak_obj->bucket);

    // Is the Key provided?
    if (riak_obj->has_key) {
        putmsg.has_key = RIAK_TRUE;
        riak_binary_to_pb_copy(&(putmsg.key), riak_obj->key);
    }

    // Data content payload
    RpbContent content;
    riak_object_to_pb_copy(ctx, &content, riak_obj);
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
    riak_uint8_t* msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_put_req__pack (&putmsg, msgbuf);

    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBPUTREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_put_response);

    return ERIAK_OK;
}

void
riak_free_put_request(riak_context       *ctx,
                      riak_put_response **resp) {
    riak_put_response *response = *resp;
    rpb_put_resp__free_unpacked(response->_internal, ctx->pb_allocator);
    riak_free(ctx, resp);
}

riak_error
riak_decode_put_response(riak_event         *rev,
                         riak_pb_message    *pbresp,
                         riak_put_response **resp,
                         riak_boolean_t     *done) {
    // decode the PB response etc
    riak_context *ctx = (riak_context*)(rev->context);
    RpbPutResp *rpbresp = rpb_put_resp__unpack(ctx->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    *done = RIAK_TRUE;
    riak_log_debug(rev, "riak_decode_put_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i = 0;
    riak_put_response *response = (riak_put_response*)(ctx->malloc_fn)(sizeof(riak_put_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_put_response));
    response->_internal = rpbresp;
    if (rpbresp->has_vclock) {
        response->has_vclock = RIAK_TRUE;
        response->vclock = riak_binary_populate_from_pb(ctx, &(rpbresp->vclock));
        if (response->vclock == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (rpbresp->has_key) {
        response->has_key = RIAK_TRUE;
        response->key = riak_binary_populate_from_pb(ctx, &(rpbresp->key));
        if (response->key == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (rpbresp->n_content > 0) {
        riak_error err = riak_object_new_array(ctx, &(response->content), rpbresp->n_content);
        if (err != ERIAK_OK) {
            riak_free(ctx, &response);
            return err;
        }
        response->n_content = rpbresp->n_content;
        for(i = 0; i < rpbresp->n_content; i++) {
            err = riak_object_new_from_pb(ctx, &(response->content[i]), rpbresp->content[i]);
            // If any object allocation fails, clean up all previously allocated ones
            if (err != ERIAK_OK) {
                riak_object_free_array(ctx, &(response->content), i);
                riak_free(ctx, &response);
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
riak_free_put_response(riak_context       *ctx,
                       riak_put_response **resp) {
    riak_put_response *response = *resp;
    if (response->n_content > 0) {
        riak_object_free_array(ctx, &(response->content), response->n_content);
    }
    riak_free(ctx, &(response->key));
    riak_free(ctx, &(response->vclock));
    rpb_put_resp__free_unpacked(response->_internal, ctx->pb_allocator);
    riak_free(ctx, resp);
}

riak_error
riak_encode_delete_request(riak_event          *rev,
                           riak_binary         *bucket,
                           riak_binary         *key,
                           riak_delete_options *options,
                           riak_pb_message    **req) {

    riak_context *ctx = (riak_context*)(rev->context);
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
    riak_uint8_t* msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_del_req__pack (&delmsg, msgbuf);

    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBDELREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_delete_response);

    return ERIAK_OK;
}


riak_error
riak_decode_delete_response(riak_event            *rev,
                            riak_pb_message       *pbresp,
                            riak_delete_response **resp,
                            riak_boolean_t        *done) {
    riak_context *ctx = (riak_context*)(rev->context);
    riak_delete_response *response = (riak_delete_response*)(ctx->malloc_fn)(sizeof(riak_delete_response));
    *done = RIAK_TRUE;
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_free_delete_response(riak_context           *ctx,
                          riak_delete_response **resp) {
    riak_free(ctx, resp);
}

riak_error
riak_encode_listbuckets_request(riak_event       *rev,
                                riak_pb_message **req) {
    riak_context *ctx = (riak_context*)(rev->context);
    RpbListBucketsReq listbucketsreq = RPB_LIST_BUCKETS_REQ__INIT;
    listbucketsreq.stream = RIAK_TRUE;
    listbucketsreq.has_stream = RIAK_TRUE;
    riak_size_t msglen = rpb_list_buckets_req__get_packed_size(&listbucketsreq);
    riak_uint8_t *msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_list_buckets_req__pack(&listbucketsreq, msgbuf);
    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBLISTBUCKETSREQ, msglen, msgbuf);
    if (request == NULL) {
        riak_free(ctx, &msgbuf);
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_listbuckets_response);

    return ERIAK_OK;
}


riak_error
riak_decode_listbuckets_response(riak_event                 *rev,
                                 riak_pb_message            *pbresp,
                                 riak_listbuckets_response **resp,
                                 riak_boolean_t             *done) {
    riak_context *ctx = rev->context;
    riak_log_debug(rev, "%s", "riak_decode_listbuckets_response");
    RpbListBucketsResp *listbucketresp = rpb_list_buckets_resp__unpack(ctx->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    int i;
    // Initialize from an existing response
    riak_listbuckets_response *response = *resp;
    // If this is NULL, there was no previous message
    if (response == NULL) {
        riak_log_debug(rev, "%s", "Initializing listbucket response");
        response = (ctx->malloc_fn)(sizeof(riak_listbuckets_response));
        if (response == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        memset((void*)response, '\0', sizeof(riak_listbuckets_response));
    }
    // Existing buckets need some expansion, so copy the pointers
    // of the old buckets to a new, larger array
    // TODO: Geometric reallocation to minimize extra mallocs, maybe?
    riak_uint32_t existing_buckets   = response->n_buckets;
    riak_uint32_t additional_buckets = listbucketresp->n_buckets;
    if (response->buckets != NULL) {
        if (riak_array_realloc(ctx,
                               (void***)&(response->buckets),
                               sizeof(riak_binary*),
                               existing_buckets,
                               (additional_buckets+existing_buckets)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->buckets = (riak_binary**)(ctx->malloc_fn)(sizeof(riak_binary*)*additional_buckets);
        if (response->buckets == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->n_buckets += additional_buckets;
    for(i = 0; i < additional_buckets; i++) {
        ProtobufCBinaryData *binary = &(listbucketresp->buckets[i]);
        response->buckets[i+existing_buckets] = riak_binary_new(ctx, binary->len, binary->data);
        if (response->buckets[i+existing_buckets]->data == NULL) {
            int j;
            rpb_list_buckets_resp__free_unpacked(listbucketresp, ctx->pb_allocator);
            for(j = 0; j < i; j++) {
                riak_free(ctx, &(response->buckets[j+existing_buckets]));
            }
            riak_free(ctx, &(response->buckets));
            riak_free(ctx, resp);

            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->done = RIAK_FALSE;
    if (listbucketresp->has_done) {
        riak_log_debug(rev, "%s", "HAS DONE");
        response->done = listbucketresp->done;
    }
    *done = response->done;

    // Expand the vector of internal RpbListBucketsResp links as necessary
    riak_uint32_t existing_pbs = response->n_responses;
    if (existing_pbs > 0) {
        if (riak_array_realloc(ctx,
                               (void***)&(response->_internal),
                               sizeof(RpbListBucketsResp*),
                               existing_pbs,
                               (existing_pbs+1)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->_internal = (RpbListBucketsResp **)(ctx->malloc_fn)(sizeof(RpbListBucketsResp*));
        if (response->_internal == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->_internal[existing_pbs] = listbucketresp;
    response->n_responses++;
    *resp = response;

    return ERIAK_OK;
}

void
riak_print_listbuckets_response(riak_listbuckets_response *response,
                                char                      *target,
                                riak_size_t                len) {
    int i;
    riak_size_t wrote = 0;
    riak_int32_t left_to_write = len;
    char name[2048];
    if (left_to_write > 0) {
        wrote = snprintf(target, left_to_write, "n_buckets = %d\n", response->n_buckets);
        left_to_write -= wrote;
        target += wrote;
    }
    for(i = 0; (left_to_write > 0) && (i < response->n_buckets); i++) {
        riak_binary_print(response->buckets[i], name, sizeof(name));
        wrote = snprintf(target, left_to_write, "%d - %s\n", i, name);
        left_to_write -= wrote;
        target += wrote;
    }
    if (left_to_write > 0) {
        snprintf(target, left_to_write, "done = %d", response->done);
    }
}

void
riak_free_listbuckets_response(riak_context               *ctx,
                               riak_listbuckets_response **resp) {
    riak_listbuckets_response *response = *resp;
    int i;
    for(i = 0; i < response->n_buckets; i++) {
        riak_free(ctx, &(response->buckets[i]));
    }
    riak_free(ctx, &(response->buckets));
    if (response->n_responses > 0) {
        for(i = 0; i < response->n_responses; i++) {
            rpb_list_buckets_resp__free_unpacked(response->_internal[i], ctx->pb_allocator);
        }
        riak_free(ctx, &(response->_internal));
    }
    riak_free(ctx, resp);
}


riak_error
riak_encode_listkeys_request(riak_event       *rev,
                             riak_binary      *bucket,
                             riak_uint32_t     timeout,
                             riak_pb_message **req) {
    riak_context *ctx = (riak_context*)(rev->context);
    RpbListKeysReq listkeysreq = RPB_LIST_KEYS_REQ__INIT;

    riak_binary_to_pb_copy(&(listkeysreq.bucket), bucket);
    if (timeout > 0) {
        listkeysreq.has_timeout = RIAK_TRUE;
        listkeysreq.timeout = timeout;
    }
    riak_size_t msglen = rpb_list_keys_req__get_packed_size(&listkeysreq);
    riak_uint8_t *msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return 1;
    }
    rpb_list_keys_req__pack(&listkeysreq, msgbuf);

    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBLISTKEYSREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_listkeys_response);

    return ERIAK_OK;

}

// STREAMING MESSAGE
riak_error
riak_decode_listkeys_response(riak_event              *rev,
                              riak_pb_message         *pbresp,
                              riak_listkeys_response **resp,
                              riak_boolean_t          *done) {
    riak_context *ctx = rev->context;
    riak_log_debug(rev, "%s", "riak_decode_listkeys_response");
    RpbListKeysResp *listkeyresp = rpb_list_keys_resp__unpack(ctx->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    int i;
    // Initialize from an existing response
    riak_listkeys_response *response = *resp;
    // If this is NULL, there was no previous message
    if (response == NULL) {
        riak_log_debug(rev, "%s", "Initializing listkey response");
        response = (ctx->malloc_fn)(sizeof(riak_listkeys_response));
        if (response == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        memset((void*)response, '\0', sizeof(riak_listkeys_response));
    }
    // Existing keys need some expansion, so copy the pointers
    // of the old keys to a new, larger array
    // TODO: Geometric reallocation to minimize extra mallocs, maybe?
    riak_uint32_t existing_keys   = response->n_keys;
    riak_uint32_t additional_keys = listkeyresp->n_keys;
    if (response->keys != NULL) {
        if (riak_array_realloc(ctx,
                               (void***)&(response->keys),
                               sizeof(riak_binary*),
                               existing_keys,
                               (additional_keys+existing_keys)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->keys = (riak_binary**)(ctx->malloc_fn)(sizeof(riak_binary*)*additional_keys);
        if (response->keys == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->n_keys += additional_keys;
    for(i = 0; i < additional_keys; i++) {
        ProtobufCBinaryData *binary = &(listkeyresp->keys[i]);
        response->keys[i+existing_keys] = riak_binary_new(ctx, binary->len, binary->data);
        if (response->keys[i+existing_keys]->data == NULL) {
            int j;
            rpb_list_keys_resp__free_unpacked(listkeyresp, ctx->pb_allocator);
            for(j = 0; j < i; j++) {
                riak_free(ctx, &(response->keys[j+existing_keys]));
            }
            riak_free(ctx, &(response->keys));
            riak_free(ctx, resp);

            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->done = RIAK_FALSE;
    if (listkeyresp->has_done) {
        riak_log_debug(rev, "%s", "HAS DONE");
        response->done = listkeyresp->done;
    }
    *done = response->done;

    // Expand the vector of internal RpbListKeysResp links as necessary
    riak_uint32_t existing_pbs = response->n_responses;
    if (existing_pbs > 0) {
        if (riak_array_realloc(ctx,
                               (void***)&(response->_internal),
                               sizeof(RpbListKeysResp*),
                               existing_pbs,
                               (existing_pbs+1)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->_internal = (RpbListKeysResp **)(ctx->malloc_fn)(sizeof(RpbListKeysResp*));
        if (response->_internal == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->_internal[existing_pbs] = listkeyresp;
    response->n_responses++;
    *resp = response;

    return ERIAK_OK;
}

void
riak_print_listkeys_response(riak_listkeys_response *response,
                             char                   *target,
                             riak_size_t             len) {
    int i;
    riak_size_t wrote = 0;
    riak_int32_t left_to_write = len;
    char name[2048];
    if (left_to_write > 0) {
        wrote = snprintf(target, left_to_write, "n_keys = %d\n", response->n_keys);
        left_to_write -= wrote;
        target += wrote;
    }
    for(i = 0; (left_to_write > 0) && (i < response->n_keys); i++) {
        riak_binary_print(response->keys[i], name, sizeof(name));
        wrote = snprintf(target, left_to_write, "%d - %s\n", i, name);
        left_to_write -= wrote;
        target += wrote;
    }
    if (left_to_write > 0) {
        snprintf(target, left_to_write, "done = %d", response->done);
    }
}

void
riak_free_listkeys_response(riak_context            *ctx,
                            riak_listkeys_response **resp) {
    riak_listkeys_response *response = *resp;
    int i;
    for(i = 0; i < response->n_keys; i++) {
        riak_free(ctx, &(response->keys[i]));
    }
    riak_free(ctx, &(response->keys));
    if (response->n_responses > 0) {
        for(i = 0; i < response->n_responses; i++) {
            rpb_list_keys_resp__free_unpacked(response->_internal[i], ctx->pb_allocator);
        }
        riak_free(ctx, &(response->_internal));
    }
    riak_free(ctx, resp);
}


riak_error
riak_encode_get_clientid_request(riak_event      *rev,
                                 riak_pb_message **req) {
    riak_context *ctx = (riak_context*)(rev->context);
    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBGETCLIENTIDREQ, 0, NULL);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_get_clientid_response);

    return ERIAK_OK;
}


riak_error
riak_decode_get_clientid_response(riak_event                  *rev,
                                  riak_pb_message             *pbresp,
                                  riak_get_clientid_response **resp,
                                  riak_boolean_t              *done) {
    // decode the PB response etc
    riak_context *ctx = (riak_context*)(rev->context);
    RpbGetClientIdResp *rpbresp = rpb_get_client_id_resp__unpack(ctx->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    riak_log_debug(rev, "riak_decode_get_clientid_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    *done = RIAK_TRUE;
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_get_clientid_response *response = (riak_get_clientid_response*)(ctx->malloc_fn)(sizeof(riak_get_clientid_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_get_clientid_response));
    response->_internal = rpbresp;

    response->client_id = riak_binary_populate_from_pb(ctx, &(rpbresp->client_id));
    if (response->client_id == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_print_get_clientid_response(riak_get_clientid_response *response,
                                 char                       *target,
                                 riak_size_t                 len) {
    char buffer[2048];
    riak_binary_hex_print(response->client_id, buffer, sizeof(buffer));
    if (len > 0) {
        snprintf(target, len, "client_id = %s\n", buffer);
    }
}

void
riak_free_get_clientid_response(riak_context                *ctx,
                                riak_get_clientid_response **resp) {
    riak_get_clientid_response *response = *resp;
    riak_free(ctx, &(response->client_id));
    rpb_get_client_id_resp__free_unpacked(response->_internal, ctx->pb_allocator);
    riak_free(ctx, resp);
}

riak_error
riak_decode_set_clientid_response(riak_event                  *rev,
                                  riak_pb_message             *pbresp,
                                  riak_set_clientid_response **resp,
                                  riak_boolean_t              *done) {
    riak_context *ctx = (riak_context*)(rev->context);
    riak_set_clientid_response *response = (riak_set_clientid_response*)(ctx->malloc_fn)(sizeof(riak_set_clientid_response));
    *done = RIAK_TRUE;
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *resp = response;

    return ERIAK_OK;
}

riak_error
riak_encode_set_clientid_request(riak_event       *rev,
                                 riak_binary      *clientid,
                                 riak_pb_message **req) {

    riak_context *ctx = (riak_context*)(rev->context);
    RpbSetClientIdReq clidmsg = RPB_SET_CLIENT_ID_REQ__INIT;
    riak_binary_to_pb_copy(&(clidmsg.client_id), clientid);

    riak_uint32_t msglen = rpb_set_client_id_req__get_packed_size(&clidmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_set_client_id_req__pack (&clidmsg, msgbuf);

    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBSETCLIENTIDREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_set_clientid_response);

    return ERIAK_OK;
}

void
riak_free_set_clientid_response(riak_context                *ctx,
                                riak_set_clientid_response **resp) {
    riak_free(ctx, resp);
}

riak_error
riak_encode_get_bucketprops_request(riak_event       *rev,
                                    riak_binary      *bucket,
                                    riak_pb_message **req) {
    riak_context *ctx = (riak_context*)(rev->context);
    RpbGetBucketReq bucketreq;
    rpb_get_bucket_req__init(&bucketreq);

    riak_binary_to_pb_copy(&(bucketreq.bucket), bucket);
    riak_size_t msglen = rpb_get_bucket_req__get_packed_size(&bucketreq);
    riak_uint8_t *msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return 1;
    }
    rpb_get_bucket_req__pack(&bucketreq, msgbuf);

    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBGETBUCKETREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_get_bucketprops_response);

    return ERIAK_OK;

}

riak_error
riak_decode_get_bucketprops_response(riak_event                     *rev,
                                     riak_pb_message                *pbresp,
                                     riak_get_bucketprops_response **resp,
                                     riak_boolean_t                 *done) {
    // decode the PB response etc
    riak_context *ctx = (riak_context*)(rev->context);
    RpbGetBucketResp *rpbresp = rpb_get_bucket_resp__unpack(ctx->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    riak_log_debug(rev, "riak_decode_get_bucketprops_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    *done = RIAK_TRUE;
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_get_bucketprops_response *response = (riak_get_bucketprops_response*)(ctx->malloc_fn)(sizeof(riak_get_bucketprops_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_get_bucketprops_response));
    response->_internal = rpbresp;

    riak_error err = riak_bucket_props_new_from_pb(ctx, &(response->props), rpbresp->props);
    if (err) {
        rpb_get_bucket_resp__free_unpacked(rpbresp, ctx->pb_allocator);
        riak_free(ctx, &response);
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_free_get_bucketprops_response(riak_context                   *ctx,
                                   riak_get_bucketprops_response **resp) {
    riak_get_bucketprops_response *response = *resp;
    riak_bucket_props_free(ctx, &(response->props));
    riak_free(ctx, resp);
}

void
riak_print_get_bucketprops_response(riak_get_bucketprops_response *response,
                                    char                          *target,
                                    riak_size_t                    len) {
    riak_bucket_props_print(response->props, target, len);
}

riak_error
riak_encode_reset_bucketprops_request(riak_event          *rev,
                                      riak_binary         *bucket,
                                      riak_pb_message    **req) {

    riak_context *ctx = (riak_context*)(rev->context);
    RpbResetBucketReq resetmsg;
    rpb_reset_bucket_req__init(&resetmsg);

    riak_binary_to_pb_copy(&resetmsg.bucket, bucket);

    riak_uint32_t msglen = rpb_reset_bucket_req__get_packed_size(&resetmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_reset_bucket_req__pack(&resetmsg, msgbuf);

    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBSETBUCKETREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_reset_bucketprops_response);

    return ERIAK_OK;
}

riak_error
riak_decode_reset_bucketprops_response(riak_event                       *rev,
                                       riak_pb_message                  *pbresp,
                                       riak_reset_bucketprops_response **resp,
                                       riak_boolean_t                   *done) {
    riak_context *ctx = (riak_context*)(rev->context);
    riak_reset_bucketprops_response *response = (riak_reset_bucketprops_response*)(ctx->malloc_fn)(sizeof(riak_reset_bucketprops_response));
    *done = RIAK_TRUE;
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_free_reset_bucketprops_response(riak_context                     *ctx,
                                     riak_reset_bucketprops_response **resp) {
    riak_free(ctx, resp);
}

riak_error
riak_encode_set_bucketprops_request(riak_event          *rev,
                                    riak_binary         *bucket,
                                    riak_bucket_props   *props,
                                    riak_pb_message    **req) {

    riak_context *ctx = (riak_context*)(rev->context);
    RpbSetBucketReq setmsg;
    rpb_set_bucket_req__init(&setmsg);

    riak_binary_to_pb_copy(&setmsg.bucket, bucket);
    RpbBucketProps pbprops;
    riak_bucket_props_to_pb_copy(ctx, &pbprops, props);
    setmsg.props = &pbprops;

    riak_uint32_t msglen = rpb_set_bucket_req__get_packed_size(&setmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_set_bucket_req__pack(&setmsg, msgbuf);

    riak_pb_message* request = riak_pb_message_new(ctx, MSG_RPBSETBUCKETREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_event_set_response_decoder(rev, (riak_response_decoder)riak_decode_set_bucketprops_response);

    return ERIAK_OK;
}

riak_error
riak_decode_set_bucketprops_response(riak_event                     *rev,
                                     riak_pb_message                *pbresp,
                                     riak_set_bucketprops_response **resp,
                                     riak_boolean_t                 *done) {
    riak_context *ctx = (riak_context*)(rev->context);
    riak_set_bucketprops_response *response = (riak_set_bucketprops_response*)(ctx->malloc_fn)(sizeof(riak_set_bucketprops_response));
    *done = RIAK_TRUE;
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_free_set_bucketprops_response(riak_context                   *ctx,
                                   riak_set_bucketprops_response **resp) {
    riak_free(ctx, resp);
}
