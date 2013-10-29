/*********************************************************************
 *
 * riak_options.c: Riak C Options
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
#include <time.h>
#include "riak.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_binary-internal.h"
#include "riak_context-internal.h"
#include "riak_print-internal.h"

riak_put_options*
riak_put_options_new(riak_context *ctx) {
    riak_put_options *o = (riak_put_options*)(ctx->malloc_fn)(sizeof(riak_put_options));
    if (o) memset(o, '\0', sizeof(riak_put_options));
    return o;
}

int
riak_put_options_print(riak_put_options  *opt,
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
    if( opt->return_body) {
        riak_print_bool("Return Body", opt->return_body, &target, &len, &total);
    }
    if (opt->has_pw) {
        riak_print_int("PW", opt->pw, &target, &len, &total);
    }
    if( opt->if_not_modified) {
        riak_print_bool("If not Modified", opt->if_not_modified, &target, &len, &total);
    }
    if( opt->if_none_match) {
        riak_print_bool("If None Match", opt->if_none_match, &target, &len, &total);
    }
    if( opt->return_head) {
        riak_print_bool("Return Head", opt->return_head, &target, &len, &total);
    }
    if (opt->has_timeout) {
        riak_print_int("Timeout", opt->timeout, &target, &len, &total);
    }
    if( opt->asis) {
        riak_print_bool("As-is", opt->asis, &target, &len, &total);
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
riak_put_options_free(riak_context *ctx,
                 riak_put_options **opt) {
    riak_free(ctx, opt);
}

//
// ACCESSORS
//
riak_boolean_t riak_put_options_get_has_vclock(riak_put_options *opt) {
    return opt->has_vclock;
}
riak_binary   *riak_put_options_get_vclock(riak_put_options *opt) {
    return opt->vclock;
}
riak_boolean_t riak_put_options_get_has_w(riak_put_options *opt) {
    return opt->has_w;
}
riak_uint32_t  riak_put_options_get_w(riak_put_options *opt) {
    return opt->w;
}
riak_boolean_t riak_put_options_get_has_dw(riak_put_options *opt) {
    return opt->has_dw;
}
riak_uint32_t  riak_put_options_get_dw(riak_put_options *opt) {
    return opt->dw;
}
riak_boolean_t riak_put_options_get_has_return_body(riak_put_options *opt) {
    return opt->has_return_body;
}
riak_boolean_t riak_put_options_get_return_body(riak_put_options *opt) {
    return opt->return_body;
}
riak_boolean_t riak_put_options_get_has_pw(riak_put_options *opt) {
    return opt->has_pw;
}
riak_uint32_t  riak_put_options_get_pw(riak_put_options *opt) {
    return opt->pw;
}
riak_boolean_t riak_put_options_get_has_if_not_modified(riak_put_options *opt) {
    return opt->has_if_not_modified;
}
riak_boolean_t riak_put_options_get_if_not_modified(riak_put_options *opt) {
    return opt->if_not_modified;
}
riak_boolean_t riak_put_options_get_has_if_none_match(riak_put_options *opt) {
    return opt->has_if_none_match;
}
riak_boolean_t riak_put_options_get_if_none_match(riak_put_options *opt) {
    return opt->if_none_match;
}
riak_boolean_t riak_put_options_get_has_return_head(riak_put_options *opt) {
    return opt->has_return_head;
}
riak_boolean_t riak_put_options_get_return_head(riak_put_options *opt) {
    return opt->return_head;
}
riak_boolean_t riak_put_options_get_has_timeout(riak_put_options *opt) {
    return opt->has_timeout;
}
riak_uint32_t  riak_put_options_get_timeout(riak_put_options *opt) {
    return opt->timeout;
}
riak_boolean_t riak_put_options_get_has_asis(riak_put_options *opt) {
    return opt->has_asis;
}
riak_boolean_t riak_put_options_get_asis(riak_put_options *opt) {
    return opt->asis;
}
riak_boolean_t riak_put_options_get_has_sloppy_quorum(riak_put_options *opt) {
    return opt->has_sloppy_quorum;
}
riak_boolean_t riak_put_options_get_sloppy_quorum(riak_put_options *opt) {
    return opt->sloppy_quorum;
}
riak_boolean_t riak_put_options_get_has_n_val(riak_put_options *opt) {
    return opt->has_n_val;
}
riak_uint32_t  riak_put_options_get_n_val(riak_put_options *opt) {
    return opt->n_val;
}
void riak_put_options_set_vclock(riak_put_options *opt, riak_binary *value) {
    opt->has_vclock = RIAK_TRUE;
    opt->vclock = value;
}
void riak_put_options_set_w(riak_put_options *opt, riak_uint32_t value) {
    opt->has_w = RIAK_TRUE;
    opt->w = value;
}
void riak_put_options_set_dw(riak_put_options *opt, riak_uint32_t value) {
    opt->has_dw = RIAK_TRUE;
    opt->dw = value;
}
void riak_put_options_set_return_body(riak_put_options *opt, riak_boolean_t value) {
    opt->has_return_body = RIAK_TRUE;
    opt->return_body = value;
}
void riak_put_options_set_pw(riak_put_options *opt, riak_uint32_t value) {
    opt->has_pw = RIAK_TRUE;
    opt->pw = value;
}
void riak_put_options_set_if_not_modified(riak_put_options *opt, riak_boolean_t value) {
    opt->has_if_not_modified = RIAK_TRUE;
    opt->if_not_modified = value;
}
void riak_put_options_set_if_none_match(riak_put_options *opt, riak_boolean_t value) {
    opt->has_if_none_match = RIAK_TRUE;
    opt->if_none_match = value;
}
void riak_put_options_set_return_head(riak_put_options *opt, riak_boolean_t value) {
    opt->has_return_head = RIAK_TRUE;
    opt->return_head = value;
}
void riak_put_options_set_timeout(riak_put_options *opt, riak_uint32_t value) {
    opt->has_timeout = RIAK_TRUE;
    opt->timeout = value;
}
void riak_put_options_set_asis(riak_put_options *opt, riak_boolean_t value) {
    opt->has_asis = RIAK_TRUE;
    opt->asis = value;
}
void riak_put_options_set_sloppy_quorum(riak_put_options *opt, riak_boolean_t value) {
    opt->has_sloppy_quorum = RIAK_TRUE;
    opt->sloppy_quorum = value;
}
void riak_put_options_set_n_val(riak_put_options *opt, riak_uint32_t value) {
    opt->has_n_val = RIAK_TRUE;
    opt->n_val = value;
}
