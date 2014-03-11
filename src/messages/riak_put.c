/*********************************************************************
 *
 * riak_get.c: Riak C Client Put Message
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

riak_error
riak_put_request_encode(riak_operation   *rop,
                        riak_object      *riak_obj,
                        riak_put_options *options,
                        riak_pb_message **req) {

    riak_config *cfg = riak_operation_get_config(rop);
    RpbPutReq putmsg = RPB_PUT_REQ__INIT;

    riak_binary_copy_to_pb(&(putmsg.bucket), riak_obj->bucket);

    // Is the Key provided?
    if (riak_obj->has_key) {
        putmsg.has_key = RIAK_TRUE;
        riak_binary_copy_to_pb(&(putmsg.key), riak_obj->key);
    }

    // Data content payload
    RpbContent content = RPB_CONTENT__INIT;

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
            riak_binary_copy_to_pb(&(putmsg.vclock), options->vclock);
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
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_put_response_decode);

    return ERIAK_OK;
}

void
riak_free_put_request(riak_config        *cfg,
                      riak_put_response **resp) {
    riak_put_response *response = *resp;
    if (response == NULL) return;
    riak_binary_free(cfg, &(response->key));
    riak_binary_free(cfg, &(response->vclock));
    rpb_put_resp__free_unpacked(response->_internal, cfg->pb_allocator);
    riak_free(cfg, resp);
}

riak_error
riak_put_response_decode(riak_operation     *rop,
                         riak_pb_message    *pbresp,
                         riak_put_response **resp,
                         riak_boolean_t     *done) {
    // decode the PB response etc
    riak_config *cfg = riak_operation_get_config(rop);
    RpbPutResp *rpbresp = rpb_put_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (rpbresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }
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
        response->vclock = riak_binary_copy_from_pb(cfg, &(rpbresp->vclock));
        if (response->vclock == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (rpbresp->has_key) {
        response->has_key = RIAK_TRUE;
        response->key = riak_binary_copy_from_pb(cfg, &(rpbresp->key));
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

riak_int32_t
riak_put_response_print(riak_print_state  *state,
                        riak_put_response *response) {

    riak_int32_t wrote = 0;
    if (response->has_vclock) {
        wrote += riak_print_label_binary_hex(state, "V-Clock", response->vclock);
    }
    if (response->has_key) {
        wrote += riak_print_label_binary_hex(state, "Key", response->key);
    }
    wrote += riak_print_label_int(state, "Num Objects", response->n_content);
    riak_uint32_t i;
    for(i = 0; (i < response->n_content) && (riak_print_len(state) > 0); i++) {
        wrote += riak_object_print(state, response->content[i]);
    }

    return wrote;
}

void
riak_put_response_free(riak_config        *cfg,
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

riak_boolean_t
riak_put_get_has_vclock(riak_put_response *response) {
    return response->has_vclock;
}

riak_binary*
riak_put_get_vclock(riak_put_response *response) {
    return response->vclock;
}

riak_boolean_t
riak_put_get_has_key(riak_put_response *response) {
    return response->has_key;
}

riak_binary*
riak_put_get_key(riak_put_response *response) {
    return response->key;
}

riak_int32_t
riak_put_get_n_content(riak_put_response *response) {
    return response->n_content;
}

riak_object**
riak_put_get_content(riak_put_response *response) {
    return response->content;
}

//
// P U T    O P T I O N S
//
riak_put_options*
riak_put_options_new(riak_config *cfg) {
    riak_put_options *o = (riak_put_options*)riak_config_clean_allocate(cfg, sizeof(riak_put_options));
    return o;
}

riak_int32_t
riak_put_options_print(riak_print_state *state,
                       riak_put_options *opt) {
    riak_int32_t wrote = 0;
    if (opt->has_vclock) {
        wrote += riak_print_label_binary(state, "Vector Clock", opt->vclock);
    }
    if (opt->has_w) {
        wrote += riak_print_label_int(state, "W", opt->w);
    }
    if (opt->has_dw) {
        wrote += riak_print_label_int(state, "DW", opt->dw);
    }
    if( opt->return_body) {
        wrote += riak_print_label_bool(state, "Return Body", opt->return_body);
    }
    if (opt->has_pw) {
        wrote += riak_print_label_int(state, "PW", opt->pw);
    }
    if( opt->if_not_modified) {
        wrote += riak_print_label_bool(state, "If not Modified", opt->if_not_modified);
    }
    if( opt->if_none_match) {
        wrote += riak_print_label_bool(state, "If None Match", opt->if_none_match);
    }
    if( opt->return_head) {
        wrote += riak_print_label_bool(state, "Return Head", opt->return_head);
    }
    if (opt->has_timeout) {
        wrote += riak_print_label_int(state, "Timeout", opt->timeout);
    }
    if( opt->asis) {
        wrote += riak_print_label_bool(state, "As-is", opt->asis);
    }
    if( opt->sloppy_quorum) {
        wrote += riak_print_label_bool(state, "Sloppy Quorum", opt->sloppy_quorum);
    }
    if (opt->has_n_val) {
        wrote += riak_print_label_int(state, "N Values", opt->n_val);
    }

    return wrote;
}

void
riak_put_options_free(riak_config  *cfg,
                 riak_put_options **opt) {
    riak_free(cfg, opt);
}

//
// ACCESSORS
//
riak_boolean_t
riak_put_options_get_has_vclock(riak_put_options *opt) {
    return opt->has_vclock;
}
riak_binary*
riak_put_options_get_vclock(riak_put_options *opt) {
    return opt->vclock;
}
riak_boolean_t
riak_put_options_get_has_w(riak_put_options *opt) {
    return opt->has_w;
}
riak_uint32_t
riak_put_options_get_w(riak_put_options *opt) {
    return opt->w;
}
riak_boolean_t
riak_put_options_get_has_dw(riak_put_options *opt) {
    return opt->has_dw;
}
riak_uint32_t
riak_put_options_get_dw(riak_put_options *opt) {
    return opt->dw;
}
riak_boolean_t
riak_put_options_get_has_return_body(riak_put_options *opt) {
    return opt->has_return_body;
}
riak_boolean_t
riak_put_options_get_return_body(riak_put_options *opt) {
    return opt->return_body;
}
riak_boolean_t
riak_put_options_get_has_pw(riak_put_options *opt) {
    return opt->has_pw;
}
riak_uint32_t
riak_put_options_get_pw(riak_put_options *opt) {
    return opt->pw;
}
riak_boolean_t
riak_put_options_get_has_if_not_modified(riak_put_options *opt) {
    return opt->has_if_not_modified;
}
riak_boolean_t
riak_put_options_get_if_not_modified(riak_put_options *opt) {
    return opt->if_not_modified;
}
riak_boolean_t
riak_put_options_get_has_if_none_match(riak_put_options *opt) {
    return opt->has_if_none_match;
}
riak_boolean_t
riak_put_options_get_if_none_match(riak_put_options *opt) {
    return opt->if_none_match;
}
riak_boolean_t
riak_put_options_get_has_return_head(riak_put_options *opt) {
    return opt->has_return_head;
}
riak_boolean_t
riak_put_options_get_return_head(riak_put_options *opt) {
    return opt->return_head;
}
riak_boolean_t
riak_put_options_get_has_timeout(riak_put_options *opt) {
    return opt->has_timeout;
}
riak_uint32_t
riak_put_options_get_timeout(riak_put_options *opt) {
    return opt->timeout;
}
riak_boolean_t
riak_put_options_get_has_asis(riak_put_options *opt) {
    return opt->has_asis;
}
riak_boolean_t
riak_put_options_get_asis(riak_put_options *opt) {
    return opt->asis;
}
riak_boolean_t
riak_put_options_get_has_sloppy_quorum(riak_put_options *opt) {
    return opt->has_sloppy_quorum;
}
riak_boolean_t
riak_put_options_get_sloppy_quorum(riak_put_options *opt) {
    return opt->sloppy_quorum;
}
riak_boolean_t
riak_put_options_get_has_n_val(riak_put_options *opt) {
    return opt->has_n_val;
}
riak_uint32_t
riak_put_options_get_n_val(riak_put_options *opt) {
    return opt->n_val;
}
void
riak_put_options_set_vclock(riak_config      *cfg,
                            riak_put_options *opt,
                            riak_binary      *value) {
    opt->has_vclock = RIAK_TRUE;
    if (opt->vclock) {
        riak_free(cfg, &opt->vclock);
    }
    opt->vclock = riak_binary_copy(cfg, value);
}
void
riak_put_options_set_w(riak_put_options *opt,
                       riak_uint32_t     value) {
    opt->has_w = RIAK_TRUE;
    opt->w = value;
}
void
riak_put_options_set_dw(riak_put_options *opt,
                        riak_uint32_t     value) {
    opt->has_dw = RIAK_TRUE;
    opt->dw = value;
}
void
riak_put_options_set_return_body(riak_put_options *opt,
                                 riak_boolean_t    value) {
    opt->has_return_body = RIAK_TRUE;
    opt->return_body = value;
}
void
riak_put_options_set_pw(riak_put_options *opt,
                        riak_uint32_t     value) {
    opt->has_pw = RIAK_TRUE;
    opt->pw = value;
}
void
riak_put_options_set_if_not_modified(riak_put_options *opt,
                                     riak_boolean_t    value) {
    opt->has_if_not_modified = RIAK_TRUE;
    opt->if_not_modified = value;
}
void
riak_put_options_set_if_none_match(riak_put_options *opt,
                                   riak_boolean_t    value) {
    opt->has_if_none_match = RIAK_TRUE;
    opt->if_none_match = value;
}
void
riak_put_options_set_return_head(riak_put_options *opt,
                                 riak_boolean_t    value) {
    opt->has_return_head = RIAK_TRUE;
    opt->return_head = value;
}
void
riak_put_options_set_timeout(riak_put_options *opt,
                             riak_uint32_t     value) {
    opt->has_timeout = RIAK_TRUE;
    opt->timeout = value;
}
void
riak_put_options_set_asis(riak_put_options *opt,
                          riak_boolean_t    value) {
    opt->has_asis = RIAK_TRUE;
    opt->asis = value;
}
void
riak_put_options_set_sloppy_quorum(riak_put_options *opt,
                                   riak_boolean_t    value) {
    opt->has_sloppy_quorum = RIAK_TRUE;
    opt->sloppy_quorum = value;
}
void
riak_put_options_set_n_val(riak_put_options *opt,
                           riak_uint32_t     value) {
    opt->has_n_val = RIAK_TRUE;
    opt->n_val = value;
}
