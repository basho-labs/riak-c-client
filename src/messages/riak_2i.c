/*********************************************************************
 *
 * riak_2i.c: Riak C Client Index Message
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

riak_error
riak_2i_request_encode(riak_operation      *rop,
                           riak_binary         *bucket_type,
                           riak_binary         *bucket,
                           riak_binary         *index,
                           riak_2i_options *index_options,
                           riak_pb_message    **req) {
    riak_config *cfg = riak_operation_get_config(rop);
    RpbIndexReq twoimsg = RPB_INDEX_REQ__INIT;
    riak_operation_set_bucket_type(rop, bucket_type);
    riak_operation_set_bucket(rop, bucket);
    riak_operation_set_index(rop, index);
    riak_binary_copy_to_pb(&twoimsg.bucket, bucket);
    riak_binary_copy_to_pb(&twoimsg.index, index);
    if(bucket_type != NULL) {
        riak_binary_copy_to_pb(&twoimsg.type, bucket_type);
        twoimsg.has_type = RIAK_TRUE;
    }

    // process get options
    if (index_options != NULL) {
        twoimsg.qtype = index_options->qtype;
        twoimsg.has_key = index_options->has_key;
        if (index_options->has_key) {
            riak_binary_copy_to_pb(&twoimsg.key, index_options->key);
        }
        twoimsg.has_range_min = index_options->has_range_min;
        if (index_options->has_range_min) {
            riak_binary_copy_to_pb(&twoimsg.range_min, index_options->range_min);
        }
        twoimsg.has_range_max = index_options->has_range_max;
        if (index_options->has_range_max) {
            riak_binary_copy_to_pb(&twoimsg.range_max, index_options->range_max);
        }
        twoimsg.has_return_terms = index_options->has_return_terms;
        twoimsg.return_terms = index_options->return_terms;
        twoimsg.has_stream = index_options->has_stream;
        twoimsg.stream = index_options->stream;
        twoimsg.has_max_results = index_options->has_max_results;
        twoimsg.max_results = index_options->max_results;
        twoimsg.has_continuation = index_options->has_continuation;
        if (index_options->has_continuation) {
            riak_binary_copy_to_pb(&twoimsg.continuation, index_options->continuation);
        }
        twoimsg.has_timeout = index_options->has_timeout;
        twoimsg.timeout = index_options->timeout;
        twoimsg.has_type = index_options->has_type;
        if (index_options->has_type) {
            riak_binary_copy_to_pb(&twoimsg.type, index_options->type);
        }
        twoimsg.has_term_regex = index_options->has_term_regex;
        if (index_options->has_term_regex) {
            riak_binary_copy_to_pb(&twoimsg.term_regex, index_options->term_regex);
        }
        twoimsg.has_pagination_sort = index_options->has_pagination_sort;
        twoimsg.pagination_sort = index_options->pagination_sort;
    }
    riak_uint32_t msglen = rpb_index_req__get_packed_size(&twoimsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)riak_config_clean_allocate(cfg, msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_index_req__pack(&twoimsg, msgbuf);
    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBINDEXREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_2i_response_decode);

    return ERIAK_OK;
}

riak_error
riak_2i_response_decode(riak_operation        *rop,
                            riak_pb_message       *pbresp,
                            riak_2i_response **resp,
                            riak_boolean_t        *done) {
    // decode the PB response etc
    riak_config *cfg = riak_operation_get_config(rop);
    RpbIndexResp *rpbresp = rpb_index_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (rpbresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }

    // Initialize from an existing response
    riak_2i_response *response = *resp;
    // If this is NULL, there was no previous message
    if (response == NULL) {
        response = (riak_2i_response*)riak_config_clean_allocate(cfg, sizeof(riak_2i_response));
        if (response == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    *done = RIAK_FALSE;
    if (rpbresp->has_done) {
        *done = rpbresp->done;
    }

    // Expand the vector of internal RpbListKeysResp links as necessary
    riak_uint32_t existing_pbs = response->_n_responses;
    if (existing_pbs > 0) {
        if (riak_array_realloc(cfg,
                               (void***)&(response->_internal),
                               sizeof(RpbIndexResp*),
                               existing_pbs,
                               (existing_pbs+1)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->_internal = (RpbIndexResp **)riak_config_allocate(cfg, sizeof(RpbIndexResp*));
        if (response->_internal == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->_internal[existing_pbs] = rpbresp;
    response->_n_responses++;

    // If we are streaming, i.e., returning partial results or if we
    // have the complete message, then assemble a user-consumable response
    if (rop->streaming || *done) {
        int i, j, chunk;
        riak_uint32_t total_keys = 0;
        riak_uint32_t total_results = 0;
        for(i = 0; i < response->_n_responses; i++) {
            total_keys    += response->_internal[i]->n_keys;
            total_results += response->_internal[i]->n_results;
        }
        response->keys = (riak_binary**)riak_config_clean_allocate(cfg, sizeof(riak_binary*) * total_keys);
        if (response->keys == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        response->n_keys = total_keys;
        response->results = riak_config_clean_allocate(cfg, sizeof(riak_pair*) * total_results);
        if (response->results == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        response->n_results = total_results;

        for(i = 0, j = 0, chunk = 0; i < response->n_keys; i++) {
            RpbIndexResp *rpb_response = response->_internal[chunk];
            response->keys[i] = riak_binary_copy_from_pb(cfg, &(rpb_response->keys[j]));
            if (response->keys[i] == NULL) {
                riak_free(cfg, &(response->keys));
                return ERIAK_OUT_OF_MEMORY;
            }
            // If we have run out of keys in the current response,
            // start working on the next response
            if (++j >= response->_internal[chunk]->n_keys) {
                chunk++;
                j = 0;
            }
        }
        for(i = 0, j = 0, chunk = 0; i < response->n_results; i++) {
            RpbIndexResp *rpb_response = response->_internal[chunk];
            riak_error err = riak_pairs_copy_from_pb(cfg, &(response->results[i]), rpb_response->results[j]);
            if (err == ERIAK_OK) {
                riak_free(cfg, &(response->results));
                return ERIAK_OUT_OF_MEMORY;
            }
            // If we have run out of results in the current response,
            // start working on the next response
            if (++j >= response->_internal[chunk]->n_keys) {
                chunk++;
                j = 0;
            }
        }
    }
    // Is this correct?  This would only be from the last message
    RpbIndexResp *rpb_response = response->_internal[response->_n_responses-1];
    response->has_continuation = rpb_response->has_continuation;
    response->continuation = riak_binary_copy_from_pb(cfg, &(rpb_response->continuation));
    response->has_done = rpb_response->has_done;
    response->done = rpb_response->done;

    *resp = response;

    return ERIAK_OK;
}

riak_int32_t
riak_2i_response_print(riak_print_state     *state,
                           riak_2i_response *response) {

    riak_int32_t wrote = 0;

    wrote += riak_print_label_int(state, "n_keys", response->n_keys);
    int i;
    for(i = 0; (riak_print_len(state) > 0) && (i < response->n_keys); i++) {
        wrote += riak_print_label_binary(state, "key", response->keys[i]);
    }
    wrote += riak_print_label_int(state, "n_results", response->n_results);
    wrote += riak_pairs_print(state, response->results, response->n_results);
    wrote += riak_print_label_bool(state, "has_continuation", response->has_continuation);
    wrote += riak_print_label_binary(state, "continuation", response->continuation);
    wrote += riak_print_label_bool(state, "has_done", response->has_done);
    wrote += riak_print_label_bool(state, "done", response->done);

    return wrote;
}

void
riak_2i_response_free(riak_config           *cfg,
                          riak_2i_response **resp) {
    riak_2i_response *response = *resp;
    if (response == NULL) {
        return;
    }
    int i;
    for(i = 0; i < response->n_keys; i++) {
        riak_binary_free(cfg, &(response->keys[i]));
    }
    riak_free(cfg, &(response->keys));
    riak_pairs_free(cfg, &(response->results), response->n_results);
    riak_free(cfg, &(response->results));
    for(i = 0; i < response->_n_responses; i++) {
        rpb_index_resp__free_unpacked(response->_internal[i], cfg->pb_allocator);
    }
    riak_free(cfg, &(response->_internal));
    riak_free(cfg, resp);
}

riak_uint32_t
riak_2i_get_n_keys(riak_2i_response *response) {
    return response->n_keys;
}

riak_binary**
riak_2i_get_keys(riak_2i_response *response) {
    return response->keys;
}

riak_uint32_t
riak_2i_get_n_results(riak_2i_response *response) {
    return response->n_results;
}

riak_pair**
riak_2i_get_results(riak_2i_response *response) {
    return response->results;
}

riak_boolean_t
riak_2i_get_has_continuation(riak_2i_response *response) {
    return response->has_continuation;
}

riak_binary*
riak_2i_get_continuation(riak_2i_response *response) {
    return response->continuation;
}

riak_boolean_t
riak_2i_get_has_done(riak_2i_response *response) {
    return response->has_done;
}

riak_boolean_t
riak_2i_get_done(riak_2i_response *response) {
    return response->done;
}


riak_2i_options*
riak_2i_options_new(riak_config *cfg) {
    riak_2i_options* opt = (riak_2i_options*)riak_config_clean_allocate(cfg, sizeof(riak_2i_options));
    if (opt) {
        // Turn streaming on by default
        riak_2i_options_set_stream(opt, RIAK_TRUE);
    }
    return opt;
}

void
riak_2i_options_free(riak_config          *cfg,
                         riak_2i_options **options) {

    riak_2i_options *opt = *options;

    riak_binary_free(cfg, &(opt->key));
    riak_binary_free(cfg, &(opt->range_min));
    riak_binary_free(cfg, &(opt->range_max));
    riak_binary_free(cfg, &(opt->type));
    riak_binary_free(cfg, &(opt->continuation));
    riak_binary_free(cfg, &(opt->term_regex));
    riak_free(cfg, options);
}

riak_boolean_t
riak_2i_options_get_is_range_query(riak_2i_options *opt) {
    return (opt->qtype == RPB_INDEX_REQ__INDEX_QUERY_TYPE__range) ? RIAK_TRUE : RIAK_FALSE;
}

riak_boolean_t
riak_2i_options_get_has_key(riak_2i_options *opt) {
    return opt->has_key;
}

riak_binary*
riak_2i_options_get_key(riak_2i_options *opt) {
    return opt->key;
}

riak_boolean_t
riak_2i_options_get_has_range_min(riak_2i_options *opt) {
    return opt->has_range_min;
}

riak_binary*
riak_2i_options_get_range_min(riak_2i_options *opt) {
    return opt->range_min;
}

riak_boolean_t
riak_2i_options_get_has_range_max(riak_2i_options *opt) {
    return opt->has_range_max;
}

riak_binary*
riak_2i_options_get_range_max(riak_2i_options *opt) {
    return opt->range_max;
}

riak_boolean_t
riak_2i_options_get_has_return_terms(riak_2i_options *opt) {
    return opt->has_return_terms;
}

riak_boolean_t
riak_2i_options_get_return_terms(riak_2i_options *opt) {
    return opt->return_terms;
}

riak_boolean_t
riak_2i_options_get_has_stream(riak_2i_options *opt) {
    return opt->has_stream;
}

riak_boolean_t
riak_2i_options_get_stream(riak_2i_options *opt) {
    return opt->stream;
}

riak_boolean_t
riak_2i_options_get_has_max_results(riak_2i_options *opt) {
    return opt->has_max_results;
}

riak_uint32_t
riak_2i_options_get_max_results(riak_2i_options *opt) {
    return opt->max_results;
}

riak_boolean_t
riak_2i_options_get_has_continuation(riak_2i_options *opt) {
    return opt->has_continuation;
}

riak_binary*
riak_2i_options_get_continuation(riak_2i_options *opt) {
    return opt->continuation;
}

riak_boolean_t
riak_2i_options_get_has_timeout(riak_2i_options *opt) {
    return opt->has_timeout;
}

riak_uint32_t
riak_2i_options_get_timeout(riak_2i_options *opt) {
    return opt->timeout;
}

riak_boolean_t
riak_2i_options_get_has_type(riak_2i_options *opt) {
    return opt->has_type;
}

riak_binary*
riak_2i_options_get_type(riak_2i_options *opt) {
    return opt->type;
}

riak_boolean_t
riak_2i_options_get_has_term_regex(riak_2i_options *opt) {
    return opt->has_term_regex;
}

riak_binary*
riak_2i_options_get_term_regex(riak_2i_options *opt) {
    return opt->term_regex;
}

riak_boolean_t
riak_2i_options_get_has_pagination_sort(riak_2i_options *opt) {
    return opt->has_pagination_sort;
}

riak_boolean_t
riak_2i_options_get_pagination_sort(riak_2i_options *opt) {
    return opt->pagination_sort;
}

void
riak_2i_options_set_range_query(riak_2i_options *opt) {
    opt->qtype = RPB_INDEX_REQ__INDEX_QUERY_TYPE__range;
}

riak_error
riak_2i_options_set_key(riak_config         *cfg,
                            riak_2i_options *opt,
                            riak_binary         *value) {
    opt->has_key = RIAK_TRUE;
    opt->key = riak_binary_copy(cfg, value);
    if (opt->key == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_2i_options_set_range_min(riak_config         *cfg,
                                  riak_2i_options *opt,
                                  riak_binary         *value) {
    opt->has_range_min = RIAK_TRUE;
    opt->range_min = riak_binary_copy(cfg, value);
    if (opt->range_min == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_2i_options_set_range_max(riak_config         *cfg,
                                  riak_2i_options *opt,
                                  riak_binary         *value) {
    opt->has_range_max = RIAK_TRUE;
    opt->range_max = riak_binary_copy(cfg, value);
    if (opt->range_max == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

void
riak_2i_options_set_return_terms(riak_2i_options *opt,
                                     riak_boolean_t       value) {
    opt->has_return_terms = RIAK_TRUE;
    opt->return_terms = value;
}

void
riak_2i_options_set_stream(riak_2i_options *opt,
                               riak_boolean_t       value) {
    opt->has_stream = RIAK_TRUE;
    opt->stream = value;
}

void
riak_2i_options_set_max_results(riak_2i_options *opt,
                                    riak_uint32_t        value) {
    opt->has_max_results = RIAK_TRUE;
    opt->max_results = value;
}

riak_error
riak_2i_options_set_continuation(riak_config         *cfg,
                                     riak_2i_options *opt,
                                     riak_binary         *value) {
    opt->has_continuation = RIAK_TRUE;
    opt->continuation = riak_binary_copy(cfg, value);
    if (opt->continuation == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

void
riak_2i_options_set_timeout(riak_2i_options *opt,
                                riak_uint32_t        value) {
    opt->has_timeout = RIAK_TRUE;
    opt->timeout = value;
}

riak_error
riak_2i_options_set_type(riak_config         *cfg,
                             riak_2i_options *opt,
                             riak_binary         *value) {
    opt->has_type = RIAK_TRUE;
    opt->type = riak_binary_copy(cfg, value);
    if (opt->type == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

riak_error
riak_2i_options_set_term_regex(riak_config         *cfg,
                                   riak_2i_options *opt,
                                   riak_binary         *value) {
    opt->has_term_regex = RIAK_TRUE;
    opt->term_regex = riak_binary_copy(cfg, value);
    if (opt->term_regex == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    return ERIAK_OK;
}

void
riak_2i_options_set_pagination_sort(riak_2i_options *opt,
                                         riak_boolean_t       value) {
    opt->has_pagination_sort = RIAK_TRUE;
    opt->pagination_sort = value;
}


