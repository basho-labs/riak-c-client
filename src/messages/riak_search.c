/*********************************************************************
 *
 * riak_get.c: Riak C Client Legacy Search Message
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
riak_search_request_encode(riak_operation      *rop,
                           riak_binary         *bucket,
                           riak_binary         *query,
                           riak_search_options *search_options,
                           riak_pb_message    **req) {
    riak_config *cfg = riak_operation_get_config(rop);
    RpbSearchQueryReq srchmsg = RPB_SEARCH_QUERY_REQ__INIT;

    riak_operation_set_bucket(rop, bucket);
    riak_binary_copy_to_pb(&srchmsg.q, query);
    riak_binary_copy_to_pb(&srchmsg.index, bucket);

    // process get options
    if(search_options != NULL) {
        srchmsg.has_rows = search_options->has_rows;
        srchmsg.rows = search_options->rows;
        srchmsg.has_start = search_options->has_start;
        srchmsg.start = search_options->start;
        if (search_options->has_sort) {
            srchmsg.has_sort = search_options->has_sort;
            riak_binary_copy_to_pb(&srchmsg.sort, search_options->sort);
        }
        if (search_options->has_filter) {
            srchmsg.has_filter = search_options->has_filter;
            riak_binary_copy_to_pb(&srchmsg.filter, search_options->filter);
        }
        if (search_options->has_df) {
            srchmsg.has_df = search_options->has_df;
            riak_binary_copy_to_pb(&srchmsg.df, search_options->df);
        }
        if (search_options->has_df) {
            srchmsg.has_op = search_options->has_op;
            riak_binary_copy_to_pb(&srchmsg.op, search_options->op);
        }
        if (search_options->has_presort) {
            srchmsg.has_presort = search_options->has_presort;
            riak_binary_copy_to_pb(&srchmsg.presort, search_options->presort);
        }
        if (search_options->n_fl > 0) {
            srchmsg.n_fl = search_options->n_fl;
            srchmsg.fl = (ProtobufCBinaryData*)riak_config_clean_allocate(cfg, sizeof(ProtobufCBinaryData) * search_options->n_fl);
            if (srchmsg.fl == NULL) {
                return ERIAK_OUT_OF_MEMORY;
            }
            for(int i = 0; i < search_options->n_fl; i++) {
                riak_binary_copy_to_pb(&(srchmsg.fl[i]), search_options->fl[i]);
            }
        }
    }
    riak_uint32_t msglen = rpb_search_query_req__get_packed_size (&srchmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)riak_config_clean_allocate(cfg, msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_search_query_req__pack (&srchmsg, msgbuf);
    riak_free(cfg, &srchmsg.fl);
    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBSEARCHQUERYREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_search_response_decode);

    return ERIAK_OK;
}

riak_error
riak_search_response_decode(riak_operation           *rop,
                               riak_pb_message       *pbresp,
                               riak_search_response **resp,
                               riak_boolean_t        *done) {
    // decode the PB response etc
    riak_config *cfg = riak_operation_get_config(rop);
    RpbSearchQueryResp *rpbresp = rpb_search_query_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (rpbresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }

    // Initialize from an existing response
    riak_search_response *response = *resp;
    // If this is NULL, there was no previous message
    if (response == NULL) {
        response = (riak_search_response*)riak_config_clean_allocate(cfg, sizeof(riak_search_response));
        if (response == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    *done = RIAK_TRUE;

    response->has_max_score = rpbresp->has_max_score;
    response->max_score = rpbresp->max_score;
    response->has_num_found = rpbresp->has_num_found;
    response->num_found = rpbresp->num_found;
    response->n_docs = rpbresp->n_docs;
    if (rpbresp->n_docs > 0) {
        response->docs = (riak_search_doc*)riak_config_clean_allocate(cfg, sizeof(riak_search_doc) * rpbresp->n_docs);
        if (response->docs == NULL) {
            riak_free(cfg, &response);
            return ERIAK_OUT_OF_MEMORY;
        }
        for(int i = 0; i < rpbresp->n_docs; i++) {
            riak_error err = riak_pairs_copy_array_from_pb(cfg, &(response->docs[i].fields), rpbresp->docs[i]->fields, rpbresp->docs[i]->n_fields);
            if (err) {
                riak_free(cfg, &(response->docs));
                riak_free(cfg, &response);
                return err;
            }
            response->docs[i].n_fields = rpbresp->docs[i]->n_fields;
        }
    }
    response->_internal = rpbresp;

    *resp = response;

    return ERIAK_OK;
}

riak_int32_t
riak_search_response_print(riak_search_response *response,
                           char                **target,
                           riak_int32_t         *len,
                           riak_int32_t         *total)  {
    riak_int32_t wrote = 0;

    wrote += riak_print_bool("has_max_score", response->has_max_score, target, len, total);
    wrote += riak_print_float("max_score", response->max_score, target, len, total);
    wrote += riak_print_bool("has_num_found", response->has_num_found, target, len, total);
    wrote += riak_print_int("num_found", response->num_found, target, len, total);
    wrote += riak_print_int("num_docs", response->n_docs, target, len, total);
    if (response->docs == NULL) {
        return wrote;
    }
    for(int i = 0; (*len > 0) && (i < response->n_docs); i++) {
        wrote += riak_print_int("Document", i, target, len, total);
        wrote += riak_print_int("n_fields", response->docs[i].n_fields, target, len, total);
        wrote += riak_pairs_print(response->docs[i].fields,response->docs[i].n_fields, target, len, total);
    }

    return wrote;
}

void
riak_search_response_free(riak_config           *cfg,
                          riak_search_response **resp) {
    riak_search_response *response = *resp;

    if (response == NULL) {
        return;
    }
    for(int i = 0; i < response->n_docs; i++) {
        riak_pairs_free(cfg, &(response->docs[i].fields), response->docs[i].n_fields);
    }
    riak_free(cfg, &(response->docs));
    rpb_search_query_resp__free_unpacked(response->_internal, cfg->pb_allocator);
    riak_free(cfg, resp);
}

riak_size_t
riak_search_get_n_docs(riak_search_response *response) {
    return response->n_docs;
}

riak_search_doc*
riak_search_get_docs(riak_search_response *response) {
    return response->docs;
}

riak_boolean_t
riak_search_get_has_max_score(riak_search_response *response) {
    return response->has_max_score;
}

riak_float32_t
riak_search_get_max_score(riak_search_response *response) {
    return response->max_score;
}

riak_boolean_t
riak_search_get_has_num_found(riak_search_response *response) {
    return response->has_num_found;
}

riak_uint32_t
riak_search_get_num_found(riak_search_response *response) {
    return response->num_found;
}


riak_search_options*
riak_search_options_new(riak_config *cfg) {
    return (riak_search_options*)riak_config_clean_allocate(cfg, sizeof(riak_search_options));
}

void
riak_search_options_free(riak_config          *cfg,
                         riak_search_options **options) {
    riak_search_options *opt = *options;
    riak_binary_free(cfg, &opt->sort);
    riak_binary_free(cfg, &opt->filter);
    riak_binary_free(cfg, &opt->df);
    riak_binary_free(cfg, &opt->op);
    riak_binary_free(cfg, &opt->presort);
    for(int i = 0; i < opt->n_fl; i++) {
        riak_binary_free(cfg, &(opt->fl[i]));
    }
    riak_free(cfg, &(opt->fl));
    riak_free(cfg, (void**)options);
}


riak_boolean_t
riak_search_options_get_has_rows(riak_search_options *opt) {
    return opt->has_rows;
}
riak_uint32_t
riak_search_options_get_rows(riak_search_options *opt) {
    return opt->rows;
}
riak_boolean_t
riak_search_options_get_has_start(riak_search_options *opt) {
    return opt->has_start;
}
riak_uint32_t
riak_search_options_get_start(riak_search_options *opt) {
    return opt->start;
}
riak_boolean_t
riak_search_options_get_has_sort(riak_search_options *opt) {
    return opt->has_sort;
}
riak_binary*
riak_search_options_get_sort(riak_search_options *opt) {
    return opt->sort;
}
riak_boolean_t
riak_search_options_get_has_filter(riak_search_options *opt) {
    return opt->has_filter;
}
riak_binary*
riak_search_options_get_filter(riak_search_options *opt) {
    return opt->filter;
}
riak_boolean_t
riak_search_options_get_has_df(riak_search_options *opt) {
    return opt->has_df;
}
riak_binary*
riak_search_options_get_df(riak_search_options *opt) {
    return opt->df;
}
riak_boolean_t
riak_search_options_get_has_op(riak_search_options *opt) {
    return opt->has_op;
}
riak_binary*
riak_search_options_get_op(riak_search_options *opt) {
    return opt->op;
}
riak_size_t
riak_search_options_get_n_fl(riak_search_options *opt) {
    return opt->n_fl;
}
riak_binary**
riak_search_options_get_fl(riak_search_options *opt) {
    return opt->fl;
}
riak_boolean_t
riak_search_options_get_has_presort(riak_search_options *opt) {
    return opt->has_presort;
}
riak_binary*
riak_search_options_get_presort(riak_search_options *opt) {
    return opt->presort;
}

void
riak_search_options_set_rows(riak_search_options *opt,
                                    riak_uint32_t               value) {
    opt->has_rows = RIAK_TRUE;
    opt->rows = value;
}

void
riak_search_options_set_start(riak_search_options *opt,
                                     riak_uint32_t               value) {
    opt->has_start = RIAK_TRUE;
    opt->start = value;
}

riak_error
riak_search_options_set_sort(riak_config                *cfg,
                                    riak_search_options *opt,
                                    riak_binary                *value) {
    opt->has_sort = RIAK_TRUE;
    if (opt->sort) {
        riak_free(cfg, &opt->sort);
    }
    opt->sort =  riak_binary_copy(cfg, value);
    if (opt->sort == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_search_options_set_filter(riak_config                *cfg,
                                      riak_search_options *opt,
                                      riak_binary         *value) {
    opt->has_filter = RIAK_TRUE;
    if (opt->filter) {
        riak_free(cfg, &opt->filter);
    }
    opt->filter =  riak_binary_copy(cfg, value);
    if (opt->filter == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_search_options_set_df(riak_config         *cfg,
                           riak_search_options *opt,
                           riak_binary         *value) {
    opt->has_df = RIAK_TRUE;
    if (opt->df) {
        riak_free(cfg, &opt->df);
    }
    opt->df =  riak_binary_copy(cfg, value);
    if (opt->df == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_search_options_set_op(riak_config                *cfg,
                           riak_search_options *opt,
                           riak_binary         *value)  {
    opt->has_op = RIAK_TRUE;
    if (opt->op) {
        riak_free(cfg, &opt->op);
    }
    opt->op =  riak_binary_copy(cfg, value);
    if (opt->op == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_search_options_add_fl(riak_config         *cfg,
                           riak_search_options *opt,
                           riak_binary         *value) {
    if (opt->fl == NULL) {
        opt->fl = (riak_binary**)riak_config_clean_allocate(cfg, sizeof(riak_binary*));
    } else {
         riak_array_realloc(cfg,
                           (void***)&(opt->fl),
                           sizeof(riak_binary*),
                           opt->n_fl,
                           opt->n_fl +1);
    }
    if (opt->fl == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    opt->fl[opt->n_fl] = riak_binary_copy(cfg, value);
    if (opt->fl[opt->n_fl] == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    } else {
        opt->n_fl++;
    }

    return ERIAK_OK;
}

riak_error
riak_search_options_set_presort(riak_config         *cfg,
                                riak_search_options *opt,
                                riak_binary         *value) {
    opt->has_presort = RIAK_TRUE;
    if (opt->presort) {
        riak_free(cfg, &opt->presort);
    }
    opt->presort =  riak_binary_copy(cfg, value);
    if (opt->presort == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}
