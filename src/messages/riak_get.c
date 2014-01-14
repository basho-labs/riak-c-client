/*********************************************************************
 *
 * riak_get.c: Riak C Client Get Message
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

riak_error
riak_get_request_encode(riak_operation  *rop,
                        riak_binary      *bucket,
                        riak_binary      *key,
                        riak_get_options *get_options,
                        riak_pb_message **req) {

    riak_config *cfg = riak_operation_get_config(rop);
    RpbGetReq getmsg = RPB_GET_REQ__INIT;

    riak_operation_set_bucket(rop, bucket);
    riak_operation_set_key(rop, key);
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
riak_get_response_decode(riak_operation     *rop,
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
        response->vclock = riak_binary_populate_from_pb(cfg, &(rpbresp->vclock));
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
            response->content[i]->bucket  = riak_binary_deep_new(cfg, riak_operation_get_bucket(rop));
            response->content[i]->key     = riak_binary_deep_new(cfg, riak_operation_get_key(rop));
            response->content[i]->has_key = RIAK_TRUE;
        }
    }
    *resp = response;

    return ERIAK_OK;
}

int
riak_print_get_response(riak_get_response *response,
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
riak_get_response_free(riak_config        *cfg,
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

riak_get_options*
riak_get_options_new(riak_config *cfg) {
    return (riak_get_options*)riak_config_clean_allocate(cfg, sizeof(riak_get_options));
}

void
riak_get_options_free(riak_config       *cfg,
                      riak_get_options **opt) {
    riak_free(cfg, opt);
}

int
riak_get_options_print(riak_get_options *opt,
                       char             *target,
                       riak_int32_t      len) {
    riak_int32_t total = 0;
    if (opt->has_r) {
        riak_print_int("R", opt->r, &target, &len, &total);
    }
    if (opt->has_pr) {
        riak_print_int("PR", opt->pr, &target, &len, &total);
    }
    if (opt->has_basic_quorum) {
        riak_print_bool("Basic Quorum", opt->basic_quorum, &target, &len, &total);
    }
    if (opt->has_notfound_ok) {
        riak_print_bool("Not Found OK", opt->notfound_ok, &target, &len, &total);
    }
    if (opt->has_if_modified) {
        riak_print_binary("If Modified", opt->if_modified, &target, &len, &total);
    }
    if (opt->has_head) {
        riak_print_bool("Head", opt->head, &target, &len, &total);
    }
    if (opt->has_deletedvclock) {
        riak_print_bool("Deleted VClock", opt->deletedvclock, &target, &len, &total);
    }
    if (opt->has_timeout) {
        riak_print_int("Timeout", opt->timeout, &target, &len, &total);
    }
    if (opt->has_sloppy_quorum) {
        riak_print_bool("Sloppy Quorum", opt->sloppy_quorum, &target, &len, &total);
    }
    if (opt->has_n_val) {
        riak_print_int("N Val", opt->n_val, &target, &len, &total);
    }

    return total;
}

riak_boolean_t
riak_get_get_has_vclock(riak_get_response *response) {
    return response->has_vclock;
}

riak_binary*
riak_get_get_vclock(riak_get_response *response) {
    return response->vclock;
}

riak_boolean_t
riak_get_get_has_unmodified(riak_get_response *response) {
    return response->has_unmodified;
}

/**
 * @brief Access the Unmodified flag in a Get response
 * @param response Riak Get Response
 * @returns Unmodified flag
 */
riak_boolean_t
riak_get_get_unmodified(riak_get_response *response) {
    return response->unmodified;
}

/**
 * @brief Access the Deleted flag in a Get response
 * @param response Riak Get Response
 * @returns Deleted flag
 */
riak_boolean_t
riak_get_get_deleted(riak_get_response *response) {
    return response->deleted;
}

/**
 * @brief Access the Number of Riak Objects in a Get response
 * @param response Riak Get Response
 * @returns Number of Riak Objects
 */
riak_int32_t
riak_get_get_n_content(riak_get_response *response) {
    return response->n_content;
}

/**
 * @brief Access an array of Riak Objects in a Get response
 * @param response Riak Get Response
 * @returns Array of Riak Objects (siblings)
 */
riak_object**
riak_get_get_content(riak_get_response *response) {
    return response->content;
}

riak_boolean_t
riak_get_options_get_has_r(riak_get_options *opt) {
    return opt->has_r;
}
riak_uint32_t
riak_get_options_get_r(riak_get_options *opt) {
    return opt->r;
}
riak_boolean_t
riak_get_options_get_has_pr(riak_get_options *opt) {
    return opt->has_pr;
}
riak_uint32_t
riak_get_options_get_pr(riak_get_options *opt) {
    return opt->pr;
}
riak_boolean_t
riak_get_options_get_has_basic_quorum(riak_get_options *opt) {
    return opt->has_basic_quorum;
}
riak_boolean_t
riak_get_options_get_basic_quorum(riak_get_options *opt) {
    return opt->basic_quorum;
}
riak_boolean_t
riak_get_options_get_has_notfound_ok(riak_get_options *opt) {
    return opt->has_notfound_ok;
}
riak_boolean_t
riak_get_options_get_notfound_ok(riak_get_options *opt) {
    return opt->notfound_ok;
}
riak_boolean_t
riak_get_options_get_has_if_modified(riak_get_options *opt) {
    return opt->has_if_modified;
}
riak_binary*
riak_get_options_get_if_modified(riak_get_options *opt) {
    return opt->if_modified;
}
riak_boolean_t
riak_get_options_get_has_head(riak_get_options *opt) {
    return opt->has_head;
}
riak_boolean_t
riak_get_options_get_head(riak_get_options *opt) {
    return opt->head;
}
riak_boolean_t
riak_get_options_get_has_deletedvclock(riak_get_options *opt) {
    return opt->has_deletedvclock;
}
riak_boolean_t
riak_get_options_get_deletedvclock(riak_get_options *opt) {
    return opt->deletedvclock;
}
riak_boolean_t
riak_get_options_get_has_timeout(riak_get_options *opt) {
    return opt->has_timeout;
}
riak_uint32_t
riak_get_options_get_timeout(riak_get_options *opt) {
    return opt->timeout;
}
riak_boolean_t
riak_get_options_get_has_sloppy_quorum(riak_get_options *opt) {
    return opt->has_sloppy_quorum;
}
riak_boolean_t
riak_get_options_get_sloppy_quorum(riak_get_options *opt) {
    return opt->sloppy_quorum;
}
riak_boolean_t
riak_get_options_get_has_n_val(riak_get_options *opt) {
    return opt->has_n_val;
}
riak_uint32_t
riak_get_options_get_n_val(riak_get_options *opt) {
    return opt->n_val;
}
void
riak_get_options_set_r(riak_get_options *opt,
                       riak_uint32_t value) {
    opt->has_r = RIAK_TRUE;
    opt->r = value;
}
void
riak_get_options_set_pr(riak_get_options *opt,
                        riak_uint32_t value) {
    opt->has_pr = RIAK_TRUE;
    opt->pr = value;
}
void
riak_get_options_set_basic_quorum(riak_get_options *opt,
                                  riak_boolean_t    value) {
    opt->has_basic_quorum = RIAK_TRUE;
    opt->basic_quorum = value;
}
void
riak_get_options_set_notfound_ok(riak_get_options *opt,
                                 riak_boolean_t    value) {
    opt->has_notfound_ok = RIAK_TRUE;
    opt->notfound_ok = value;
}
void
riak_get_options_set_if_modified(riak_config      *cfg,
                                 riak_get_options *opt,
                                 riak_binary      *value) {
    opt->has_if_modified = RIAK_TRUE;
    if (opt->if_modified) {
        riak_free(cfg, &opt->if_modified);
    }
    opt->if_modified =  riak_binary_deep_new(cfg, value);
}
void
riak_get_options_set_head(riak_get_options *opt,
                          riak_boolean_t    value) {
    opt->has_head = RIAK_TRUE;
    opt->head = value;
}
void
riak_get_options_set_deletedvclock(riak_get_options *opt,
                                   riak_boolean_t    value) {
    opt->has_deletedvclock = RIAK_TRUE;
    opt->deletedvclock = value;
}
void
riak_get_options_set_timeout(riak_get_options *opt,
                             riak_uint32_t     value) {
    opt->has_timeout = RIAK_TRUE;
    opt->timeout = value;
}
void
riak_get_options_set_sloppy_quorum(riak_get_options *opt,
                                   riak_boolean_t    value) {
    opt->has_sloppy_quorum = RIAK_TRUE;
    opt->sloppy_quorum = value;
}
void
riak_get_options_set_n_val(riak_get_options *opt,
                           riak_uint32_t     value) {
    opt->has_n_val = RIAK_TRUE;
    opt->n_val = value;
}
