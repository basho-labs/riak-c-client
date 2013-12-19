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
#include "riak_bucketprops-internal.h"
#include "riak_print-internal.h"

riak_error
riak_delete_request_encode(riak_operation      *rop,
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
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_delete_response_decode);

    return ERIAK_OK;
}


riak_error
riak_delete_response_decode(riak_operation        *rop,
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
riak_delete_response_free(riak_config           *cfg,
                          riak_delete_response **resp) {
    riak_free(cfg, resp);
}

riak_boolean_t has_vclock;
riak_binary   *vclock;
riak_boolean_t has_w;
riak_uint32_t  w;
riak_boolean_t has_dw;
riak_uint32_t  dw;
riak_boolean_t has_pw;
riak_uint32_t  pw;
riak_boolean_t has_timeout;
riak_uint32_t  timeout;
riak_boolean_t has_sloppy_quorum;
riak_boolean_t sloppy_quorum;
riak_boolean_t has_n_val;
riak_uint32_t  n_val;



//
// D E L E T E   O P T I O N S
//

riak_delete_options*
riak_delete_options_new(riak_config *cfg) {
    riak_delete_options *o = (riak_delete_options*)riak_config_clean_allocate(cfg, sizeof(riak_delete_options));
    return o;
}

int
riak_delete_options_print(riak_delete_options  *opt,
                       char              *target,
                       riak_int32_t       len) {
    riak_int32_t total = 0;
    if (opt->has_vclock) {
        riak_print_binary("Vector Clock", opt->vclock, &target, &len, &total);
    }
    if (opt->has_w) {
        riak_print_int("W", opt->w, &target, &len, &total);
    }
    if (opt->has_dw) {
        riak_print_int("DW", opt->dw, &target, &len, &total);
    }
    if (opt->has_pw) {
        riak_print_int("PW", opt->pw, &target, &len, &total);
    }
    if (opt->has_timeout) {
        riak_print_int("Timeout", opt->timeout, &target, &len, &total);
    }
    if( opt->sloppy_quorum) {
        riak_print_bool("Sloppy Quorum", opt->sloppy_quorum, &target, &len, &total);
    }
    if (opt->has_n_val) {
        riak_print_int("N Values", opt->n_val, &target, &len, &total);
    }

    return total;
}

void
riak_delete_options_free(riak_config  *cfg,
                 riak_delete_options **opt) {
    riak_free(cfg, opt);
}

//
// ACCESSORS
//
riak_boolean_t
riak_delete_options_get_has_vclock(riak_delete_options *opt) {
    return opt->has_vclock;
}
riak_binary*
riak_delete_options_get_vclock(riak_delete_options *opt) {
    return opt->vclock;
}
riak_boolean_t
riak_delete_options_get_has_w(riak_delete_options *opt) {
    return opt->has_w;
}
riak_uint32_t
riak_delete_options_get_w(riak_delete_options *opt) {
    return opt->w;
}
riak_boolean_t
riak_delete_options_get_has_dw(riak_delete_options *opt) {
    return opt->has_dw;
}
riak_uint32_t
riak_delete_options_get_dw(riak_delete_options *opt) {
    return opt->dw;
}
riak_boolean_t
riak_delete_options_get_has_pw(riak_delete_options *opt) {
    return opt->has_pw;
}
riak_uint32_t
riak_delete_options_get_pw(riak_delete_options *opt) {
    return opt->pw;
}
riak_boolean_t
riak_delete_options_get_has_timeout(riak_delete_options *opt) {
    return opt->has_timeout;
}
riak_uint32_t
riak_delete_options_get_timeout(riak_delete_options *opt) {
    return opt->timeout;
}
riak_boolean_t
riak_delete_options_get_has_sloppy_quorum(riak_delete_options *opt) {
    return opt->has_sloppy_quorum;
}
riak_boolean_t
riak_delete_options_get_sloppy_quorum(riak_delete_options *opt) {
    return opt->sloppy_quorum;
}
riak_boolean_t
riak_delete_options_get_has_n_val(riak_delete_options *opt) {
    return opt->has_n_val;
}
riak_uint32_t
riak_delete_options_get_n_val(riak_delete_options *opt) {
    return opt->n_val;
}
void
riak_delete_options_set_vclock(riak_config      *cfg,
                            riak_delete_options *opt,
                            riak_binary      *value) {
    opt->has_vclock = RIAK_TRUE;
    if (opt->vclock) {
        riak_free(cfg, &opt->vclock);
    }
    opt->vclock = riak_binary_deep_new(cfg, value);
}
void
riak_delete_options_set_w(riak_delete_options *opt,
                       riak_uint32_t     value) {
    opt->has_w = RIAK_TRUE;
    opt->w = value;
}
void
riak_delete_options_set_dw(riak_delete_options *opt,
                        riak_uint32_t     value) {
    opt->has_dw = RIAK_TRUE;
    opt->dw = value;
}
void
riak_delete_options_set_pw(riak_delete_options *opt,
                        riak_uint32_t     value) {
    opt->has_pw = RIAK_TRUE;
    opt->pw = value;
}
void
riak_delete_options_set_timeout(riak_delete_options *opt,
                             riak_uint32_t     value) {
    opt->has_timeout = RIAK_TRUE;
    opt->timeout = value;
}
void
riak_delete_options_set_sloppy_quorum(riak_delete_options *opt,
                                   riak_boolean_t    value) {
    opt->has_sloppy_quorum = RIAK_TRUE;
    opt->sloppy_quorum = value;
}
void
riak_delete_options_set_n_val(riak_delete_options *opt,
                           riak_uint32_t     value) {
    opt->has_n_val = RIAK_TRUE;
    opt->n_val = value;
}
