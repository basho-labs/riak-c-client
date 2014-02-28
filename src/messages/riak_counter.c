/*********************************************************************
 *
 * riak_counter.c: Riak C Client 1.4 Counters
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
#include "riak_bucketprops-internal.h"
#include "riak_print-internal.h"


/* ------------------------ */
/* Update Counter functions */
/* ------------------------ */

riak_error
riak_counter_update_request_encode(riak_operation  *rop,
                        riak_binary      *bucket,
                        riak_binary      *key,
                        riak_get_options *get_options,
                        riak_pb_message **req) {

    riak_config *cfg = riak_operation_get_config(rop);
    RpbGetReq getmsg = RPB_GET_REQ__INIT;

    riak_operation_set_bucket(rop, bucket);
    riak_operation_set_key(rop, key);
    riak_binary_copy_to_pb(&getmsg.bucket, bucket);
    riak_binary_copy_to_pb(&getmsg.key, key);

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
            riak_binary_copy_to_pb(&getmsg.if_modified, get_options->if_modified);
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
    riak_uint8_t* msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_get_req__pack (&getmsg, msgbuf);
    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBGETREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_get_response_decode);

    return ERIAK_OK;
}

riak_error
riak_counter_update_response_decode(riak_operation     *rop,
                         riak_pb_message    *pbresp,
                         riak_get_response **resp,
                         riak_boolean_t     *done) {
    // decode the PB response etc
    riak_config *cfg = riak_operation_get_config(rop);
    RpbGetResp *rpbresp = rpb_get_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (rpbresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_log_debug(cxn, "riak_decode_get_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    *done = RIAK_TRUE;
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i = 0;
    riak_get_response *response = (riak_get_response*)(cfg->malloc_fn)(sizeof(riak_get_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_get_response));
    response->_internal = rpbresp;

    if (rpbresp->has_vclock) {
        response->has_vclock = RIAK_TRUE;
        response->vclock = riak_binary_copy_from_pb(cfg, &(rpbresp->vclock));
        if (response->vclock == NULL) {
            riak_free(cfg, &response);
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (rpbresp->has_unchanged) {
        response->has_unmodified = RIAK_TRUE;
        response->unmodified = rpbresp->unchanged;
    }
    if (rpbresp->n_content > 0) {
        riak_error err = riak_object_new_array(cfg, &(response->content), rpbresp->n_content);
        if (err != ERIAK_OK) {
            riak_free(cfg, &response);
            return 1;
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
            response->content[i]->bucket  = riak_binary_copy(cfg, riak_operation_get_bucket(rop));
            response->content[i]->key     = riak_binary_copy(cfg, riak_operation_get_key(rop));
            response->content[i]->has_key = RIAK_TRUE;
        }
    }
    *resp = response;

    return ERIAK_OK;
}

int
riak_print_counter_update_response(riak_get_response *response,
                        char              *target,
                        riak_size_t        len) {
    riak_int32_t left_to_write = len;
    riak_int32_t wrote;
    riak_print_binary_hex("V-Clock", response->vclock, &target, &wrote, &left_to_write);
    riak_print_bool("Unmodified", response->unmodified, &target, &wrote, &left_to_write);
    riak_print_bool("Deleted", response->deleted, &target, &wrote, &left_to_write);
    riak_print_int("Objects", response->n_content, &target, &wrote, &left_to_write);

    riak_uint32_t i;
    for(i = 0; (i < response->n_content) && (left_to_write > 0); i++) {
        wrote = riak_object_print(response->content[i], target, left_to_write);
        left_to_write -= wrote;
        target += wrote;
    }
    return len-left_to_write;
}

void
riak_counter_update_response_free(riak_config        *cfg,
                       riak_get_response **resp) {
    riak_get_response *response = *resp;
    if (response == NULL) return;
    if (response->n_content > 0) {
        riak_object_free_array(cfg, &(response->content), response->n_content);
    }
    riak_free(cfg, &(response->vclock));
    rpb_get_resp__free_unpacked(response->_internal, cfg->pb_allocator);
    riak_free(cfg, resp);
}


/* ------------------------ */
/* Get Counter functions    */
/* ------------------------ */

riak_error
riak_counter_get_request_encode(riak_operation  *rop,
                        riak_binary      *bucket,
                        riak_binary      *key,
                        riak_get_options *get_options,
                        riak_pb_message **req) {

    riak_config *cfg = riak_operation_get_config(rop);
    RpbGetReq getmsg = RPB_GET_REQ__INIT;

    riak_operation_set_bucket(rop, bucket);
    riak_operation_set_key(rop, key);
    riak_binary_copy_to_pb(&getmsg.bucket, bucket);
    riak_binary_copy_to_pb(&getmsg.key, key);

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
            riak_binary_copy_to_pb(&getmsg.if_modified, get_options->if_modified);
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
    riak_uint8_t* msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_get_req__pack (&getmsg, msgbuf);
    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBGETREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_get_response_decode);

    return ERIAK_OK;
}

riak_error
riak_counter_get_response_decode(riak_operation     *rop,
                         riak_pb_message    *pbresp,
                         riak_get_response **resp,
                         riak_boolean_t     *done) {
    // decode the PB response etc
    riak_config *cfg = riak_operation_get_config(rop);
    RpbGetResp *rpbresp = rpb_get_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (rpbresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_log_debug(cxn, "riak_decode_get_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    *done = RIAK_TRUE;
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    int i = 0;
    riak_get_response *response = (riak_get_response*)(cfg->malloc_fn)(sizeof(riak_get_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_get_response));
    response->_internal = rpbresp;

    if (rpbresp->has_vclock) {
        response->has_vclock = RIAK_TRUE;
        response->vclock = riak_binary_copy_from_pb(cfg, &(rpbresp->vclock));
        if (response->vclock == NULL) {
            riak_free(cfg, &response);
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (rpbresp->has_unchanged) {
        response->has_unmodified = RIAK_TRUE;
        response->unmodified = rpbresp->unchanged;
    }
    if (rpbresp->n_content > 0) {
        riak_error err = riak_object_new_array(cfg, &(response->content), rpbresp->n_content);
        if (err != ERIAK_OK) {
            riak_free(cfg, &response);
            return 1;
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
            response->content[i]->bucket  = riak_binary_copy(cfg, riak_operation_get_bucket(rop));
            response->content[i]->key     = riak_binary_copy(cfg, riak_operation_get_key(rop));
            response->content[i]->has_key = RIAK_TRUE;
        }
    }
    *resp = response;

    return ERIAK_OK;
}

int
riak_print_counter_get_response(riak_get_response *response,
                        char              *target,
                        riak_size_t        len) {
    riak_int32_t left_to_write = len;
    riak_int32_t wrote;
    riak_print_binary_hex("V-Clock", response->vclock, &target, &wrote, &left_to_write);
    riak_print_bool("Unmodified", response->unmodified, &target, &wrote, &left_to_write);
    riak_print_bool("Deleted", response->deleted, &target, &wrote, &left_to_write);
    riak_print_int("Objects", response->n_content, &target, &wrote, &left_to_write);

    riak_uint32_t i;
    for(i = 0; (i < response->n_content) && (left_to_write > 0); i++) {
        wrote = riak_object_print(response->content[i], target, left_to_write);
        left_to_write -= wrote;
        target += wrote;
    }
    return len-left_to_write;
}

void
riak_counter_get_response_free(riak_config        *cfg,
                               riak_get_response **resp) {
    riak_get_response *response = *resp;
    if (response == NULL) return;
    if (response->n_content > 0) {
        riak_object_free_array(cfg, &(response->content), response->n_content);
    }
    riak_free(cfg, &(response->vclock));
    rpb_get_resp__free_unpacked(response->_internal, cfg->pb_allocator);
    riak_free(cfg, resp);
}


