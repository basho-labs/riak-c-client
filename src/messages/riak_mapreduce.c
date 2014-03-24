/*********************************************************************
 *
 * riak_get.c: Riak C Client Map/Reduce Message
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
riak_mapreduce_request_encode(riak_operation   *rop,
                              riak_binary      *content_type,
                              riak_binary      *map_request,
                              riak_pb_message **req) {

    riak_config *cfg = riak_operation_get_config(rop);
    RpbMapRedReq mapmsg = RPB_MAP_RED_REQ__INIT;

    riak_binary_copy_to_pb(&mapmsg.request, map_request);
    riak_binary_copy_to_pb(&mapmsg.content_type, content_type);

    riak_uint32_t msglen = rpb_map_red_req__get_packed_size (&mapmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_map_red_req__pack (&mapmsg, msgbuf);
    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBMAPREDREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_mapreduce_response_decode);

    return ERIAK_OK;
}

riak_error
riak_mapreduce_response_decode(riak_operation           *rop,
                               riak_pb_message          *pbresp,
                               riak_mapreduce_response **resp,
                               riak_boolean_t           *done) {
    // decode the PB response etc
    riak_config *cfg = riak_operation_get_config(rop);
    RpbMapRedResp *rpbresp = rpb_map_red_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (rpbresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }

    // Initialize from an existing response
    riak_mapreduce_response *response = *resp;
    // If this is NULL, there was no previous message
    if (response == NULL) {
        response = (riak_mapreduce_response*)riak_config_clean_allocate(cfg, sizeof(riak_mapreduce_response));
        if (response == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    *done = RIAK_FALSE;
    if (rpbresp->has_done) {
        riak_connection *cxn = riak_operation_get_connection(rop);
        riak_log_debug(cxn, "%s", "HAS DONE");
        *done = rpbresp->done;
    }

    // Expand the vector of internal RpbListKeysResp links as necessary
    riak_uint32_t existing_pbs = response->n_responses;
    if (existing_pbs > 0) {
        if (riak_array_realloc(cfg,
                               (void***)&(response->_internal),
                               sizeof(RpbMapRedResp*),
                               existing_pbs,
                               (existing_pbs+1)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->_internal = (RpbMapRedResp **)riak_config_allocate(cfg, sizeof(RpbMapRedResp*));
        if (response->_internal == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->_internal[existing_pbs] = rpbresp;
    response->n_responses++;

    // If we are streaming, i.e., returning partial results or if we
    // have the complete message, then assemble a user-consumable response
    if (rop->streaming || *done) {
        int i;
        response->msg = (riak_mapreduce_message**)riak_config_clean_allocate(cfg, sizeof(riak_mapreduce_message*) * response->n_responses);
        if (response->msg == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
        for(i = 0; i < response->n_responses; i++) {
            RpbMapRedResp *rpb_response = response->_internal[i];
            response->msg[i] = (riak_mapreduce_message*)riak_config_clean_allocate(cfg, sizeof(riak_mapreduce_message));
            if (response->msg[i] == NULL) {
                riak_free(cfg, &(response->msg));
                return ERIAK_OUT_OF_MEMORY;
            }
            riak_mapreduce_message *msg = response->msg[i];
            msg->has_done = rpb_response->has_done;
            msg->done = rpb_response->done;
            msg->has_phase = rpb_response->has_phase;
            msg->phase = rpb_response->phase;
            msg->has_response = rpb_response->has_response;
            msg->response = riak_binary_copy_from_pb(cfg, &(rpb_response->response));
        }
    }

    *resp = response;

    return ERIAK_OK;
}

riak_int32_t
riak_mapreduce_response_print(riak_print_state        *state,
                              riak_mapreduce_response *response) {
    riak_int32_t wrote = 0;

    wrote += riak_print_label_int(state, "Num Responses", response->n_responses);
    int i;
    if (response->msg == NULL) {
        return wrote;
    }
    for(i = 0; (riak_print_len(state) > 0) && (i < response->n_responses); i++) {
        riak_mapreduce_message *msg = response->msg[i];
        wrote += riak_print_label_int(state, "Response", i);
        if (msg->has_phase) {
            wrote += riak_print_label_int(state, "phase", msg->phase);
        }
        if (msg->has_response) {
            wrote += riak_print_label_binary(state, "response", msg->response);
        }
        if (msg->has_done) {
            wrote += riak_print_label_bool(state, "done", msg->done);
        }
    }

    return wrote;
}

void
riak_mapreduce_response_free(riak_config              *cfg,
                             riak_mapreduce_response **resp) {
    riak_mapreduce_response *response = *resp;
    int i;
    for(i = 0; i < response->n_responses; i++) {
        riak_binary_free(cfg, &(response->msg[i]->response));
        riak_free(cfg, &(response->msg[i]));
        rpb_map_red_resp__free_unpacked(response->_internal[i], cfg->pb_allocator);
    }
    riak_free(cfg, &(response->msg));
    riak_free(cfg, &(response->_internal));
    riak_free(cfg, resp);
}

riak_int32_t
riak_mapreduce_get_n_messages(riak_mapreduce_response *response) {
    return response->n_responses;
}

riak_mapreduce_message**
riak_mapreduce_get_messages(riak_mapreduce_response *response) {
    return response->msg;
}

riak_boolean_t
riak_mapreduce_message_get_has_phase(riak_mapreduce_message *msg) {
    return msg->has_phase;
}

riak_uint32_t
riak_mapreduce_message_get_phase(riak_mapreduce_message *msg) {
    return msg->phase;
}

riak_boolean_t
riak_mapreduce_message_get_has_response(riak_mapreduce_message *msg) {
    return msg->has_response;
}

riak_binary*
riak_mapreduce_message_get_response(riak_mapreduce_message *msg) {
    return msg->response;
}

riak_boolean_t
riak_mapreduce_message_get_has_done(riak_mapreduce_message *msg) {
    return msg->has_done;
}

riak_boolean_t
riak_mapreduce_message_get_done(riak_mapreduce_message *msg) {
    return msg->done;
}
