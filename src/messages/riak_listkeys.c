/*********************************************************************
 *
 * riak_get.c: Riak C Client List Keys Message
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
riak_listkeys_request_encode(riak_operation   *rop,
                             riak_binary      *bucket,
                             riak_binary      *bucket_type,
                             riak_uint32_t     timeout,
                             riak_pb_message **req) {
    riak_config *cfg = riak_operation_get_config(rop);
    RpbListKeysReq listkeysreq = RPB_LIST_KEYS_REQ__INIT;
    if(bucket_type != NULL) {
        riak_binary_copy_to_pb(&listkeysreq.type, bucket_type);
        listkeysreq.has_type = RIAK_TRUE;
    }
    riak_binary_copy_to_pb(&(listkeysreq.bucket), bucket);
    if (timeout > 0) {
        listkeysreq.has_timeout = RIAK_TRUE;
        listkeysreq.timeout = timeout;
    }
    riak_size_t msglen = rpb_list_keys_req__get_packed_size(&listkeysreq);
    riak_uint8_t *msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return 1;
    }
    rpb_list_keys_req__pack(&listkeysreq, msgbuf);

    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBLISTKEYSREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_listkeys_response_decode);

    return ERIAK_OK;

}

// STREAMING MESSAGE
riak_error
riak_listkeys_response_decode(riak_operation          *rop,
                              riak_pb_message         *pbresp,
                              riak_listkeys_response **resp,
                              riak_boolean_t          *done) {
    riak_config *cfg = riak_operation_get_config(rop);
    RpbListKeysResp *listkeyresp = rpb_list_keys_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (listkeyresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }
    int i;
    // Initialize from an existing response
    riak_listkeys_response *response = *resp;
    // If this is NULL, there was no previous message
    if (response == NULL) {
        response = (cfg->malloc_fn)(sizeof(riak_listkeys_response));
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
        if (riak_array_realloc(cfg,
                               (void***)&(response->keys),
                               sizeof(riak_binary*),
                               existing_keys,
                               (additional_keys+existing_keys)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->keys = (riak_binary**)(cfg->malloc_fn)(sizeof(riak_binary*)*additional_keys);
        if (response->keys == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->n_keys += additional_keys;
    for(i = 0; i < additional_keys; i++) {
        ProtobufCBinaryData *binary = &(listkeyresp->keys[i]);
        response->keys[i+existing_keys] = riak_binary_new(cfg, binary->len, binary->data);
        if (response->keys[i+existing_keys]->data == NULL) {
            int j;
            rpb_list_keys_resp__free_unpacked(listkeyresp, cfg->pb_allocator);
            for(j = 0; j < i; j++) {
                riak_free(cfg, &(response->keys[j+existing_keys]));
            }
            riak_free(cfg, &(response->keys));
            riak_free(cfg, resp);

            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->done = RIAK_FALSE;
    if (listkeyresp->has_done) {
        riak_connection *cxn = riak_operation_get_connection(rop);
        riak_log_debug(cxn, "%s", "HAS DONE");
        response->done = listkeyresp->done;
    }
    *done = response->done;

    // Expand the vector of internal RpbListKeysResp links as necessary
    riak_uint32_t existing_pbs = response->n_responses;
    if (existing_pbs > 0) {
        if (riak_array_realloc(cfg,
                               (void***)&(response->_internal),
                               sizeof(RpbListKeysResp*),
                               existing_pbs,
                               (existing_pbs+1)) == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    } else {
        response->_internal = (RpbListKeysResp **)(cfg->malloc_fn)(sizeof(RpbListKeysResp*));
        if (response->_internal == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    response->_internal[existing_pbs] = listkeyresp;
    response->n_responses++;
    *resp = response;

    return ERIAK_OK;
}

riak_int32_t
riak_listkeys_response_print(riak_print_state       *state,
                             riak_listkeys_response *response) {
    riak_int32_t wrote = 0;

    wrote += riak_print_label_int(state, "Num Keys", response->n_keys);
    for(int i = 0; (riak_print_len(state) > 0) && (i < response->n_keys); i++) {
        wrote += riak_print(state, "%d - ", i);
        wrote += riak_print_binary(state, response->keys[i]);
        wrote += riak_print(state, "%s", "\n");
    }
    wrote += riak_print_label_bool(state, "Done", response->done);

    return wrote;
}

void
riak_listkeys_response_free(riak_config             *cfg,
                            riak_listkeys_response **resp) {
    riak_listkeys_response *response = *resp;
    if (response == NULL) return;
    int i;
    for(i = 0; i < response->n_keys; i++) {
        riak_binary_free(cfg, &(response->keys[i]));
    }
    riak_free(cfg, &(response->keys));
    if (response->n_responses > 0) {
        for(i = 0; i < response->n_responses; i++) {
            rpb_list_keys_resp__free_unpacked(response->_internal[i], cfg->pb_allocator);
        }
        riak_free(cfg, &(response->_internal));
    }
    riak_free(cfg, resp);
}

riak_uint32_t
riak_listkeys_get_n_keys(riak_listkeys_response *response) {
    return response->n_keys;
}

/**
 * @brief Access the list of keys in a bucket
 * @param response List Keys response message
 * @returns An array of keys in a bucket
 */
riak_binary**
riak_listkeys_get_keys(riak_listkeys_response *response) {
    return response->keys;
}

