/*********************************************************************
 *
 * riak_get.c: Riak C Client List Buckets Message
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
#include "riak_print-internal.h"

riak_error
riak_listbuckets_request_encode(riak_operation   *rop,
                                riak_pb_message **req) {
    riak_config *cfg = riak_operation_get_config(rop);
    RpbListBucketsReq listbucketsreq = RPB_LIST_BUCKETS_REQ__INIT;
    listbucketsreq.stream = RIAK_TRUE;
    listbucketsreq.has_stream = RIAK_TRUE;
    riak_size_t msglen = rpb_list_buckets_req__get_packed_size(&listbucketsreq);
    riak_uint8_t *msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_list_buckets_req__pack(&listbucketsreq, msgbuf);
    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBLISTBUCKETSREQ, msglen, msgbuf);
    if (request == NULL) {
        riak_free(cfg, &msgbuf);
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_listbuckets_response_decode);

    return ERIAK_OK;
}


riak_error
riak_listbuckets_response_decode(riak_operation             *rop,
                                 riak_pb_message            *pbresp,
                                 riak_listbuckets_response **resp,
                                 riak_boolean_t             *done) {
    riak_config *cfg = riak_operation_get_config(rop);
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_log_debug(cxn, "%s", "riak_decode_listbuckets_response");
    RpbListBucketsResp *listbucketresp = rpb_list_buckets_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (listbucketresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }
    int i;
    // Initialize from an existing response
    riak_listbuckets_response *response = *resp;
    // If this is NULL, there was no propious message
    if (response == NULL) {
        riak_log_debug(cxn, "%s", "Initializing listbucket response");
        response = (cfg->malloc_fn)(sizeof(riak_listbuckets_response));
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
        if (riak_array_realloc(cfg,
                               (void***)&(response->buckets),
                               sizeof(riak_binary*),
                               existing_buckets,
                               (additional_buckets+existing_buckets)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->buckets = (riak_binary**)(cfg->malloc_fn)(sizeof(riak_binary*)*additional_buckets);
        if (response->buckets == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->n_buckets += additional_buckets;
    for(i = 0; i < additional_buckets; i++) {
        ProtobufCBinaryData *binary = &(listbucketresp->buckets[i]);
        response->buckets[i+existing_buckets] = riak_binary_new(cfg, binary->len, binary->data);
        if (response->buckets[i+existing_buckets]->data == NULL) {
            int j;
            rpb_list_buckets_resp__free_unpacked(listbucketresp, cfg->pb_allocator);
            for(j = 0; j < i; j++) {
                riak_free(cfg, &(response->buckets[j+existing_buckets]));
            }
            riak_free(cfg, &(response->buckets));
            riak_free(cfg, resp);

            return ERIAK_OUT_OF_MEMORY;
        }
    }

    response->done = RIAK_FALSE;
    if (listbucketresp->has_done) {
        riak_connection *cxn = riak_operation_get_connection(rop);
        riak_log_debug(cxn, "%s", "HAS DONE");
        response->done = listbucketresp->done;
    }
    *done = response->done;

    // Expand the vector of internal RpbListBucketsResp links as necessary
    riak_uint32_t existing_pbs = response->n_responses;
    if (existing_pbs > 0) {
        if (riak_array_realloc(cfg,
                               (void***)&(response->_internal),
                               sizeof(RpbListBucketsResp*),
                               existing_pbs,
                               (existing_pbs+1)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->_internal = (RpbListBucketsResp **)(cfg->malloc_fn)(sizeof(RpbListBucketsResp*));
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
riak_listbuckets_response_print(riak_listbuckets_response *response,
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
riak_listbuckets_response_free(riak_config                *cfg,
                               riak_listbuckets_response **resp) {
    riak_listbuckets_response *response = *resp;
    if (response == NULL) return;
    int i;
    for(i = 0; i < response->n_buckets; i++) {
        riak_free(cfg, &(response->buckets[i]));
    }
    riak_free(cfg, &(response->buckets));
    if (response->n_responses > 0) {
        for(i = 0; i < response->n_responses; i++) {
            rpb_list_buckets_resp__free_unpacked(response->_internal[i], cfg->pb_allocator);
        }
        riak_free(cfg, &(response->_internal));
    }
    riak_free(cfg, resp);
}

riak_uint32_t
riak_listbuckets_get_n_buckets(riak_listbuckets_response *response) {
    return response->n_buckets;
}

riak_error
riak_listbuckets_get_bucket(riak_listbuckets_response *response,
                            riak_binary **bucket,
                            riak_uint32_t n) {
    if (n >= response->n_buckets) {
        return ERIAK_OUT_OF_RANGE;
    }
    *bucket = response->buckets[n];
    return ERIAK_OK;
}
