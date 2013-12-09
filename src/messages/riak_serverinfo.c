/*********************************************************************
 *
 * riak_get.c: Riak C Client Server Information Message
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
#include "riak_bucket_props-internal.h"
#include "riak_print-internal.h"

riak_error
riak_serverinfo_request_encode(riak_operation   *rop,
                               riak_pb_message **req) {
    riak_config *cfg = riak_operation_get_config(rop);
    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBGETSERVERINFOREQ, 0, NULL);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_serverinfo_response_decode);

    return ERIAK_OK;
}

riak_error
riak_serverinfo_response_decode(riak_operation            *rop,
                                riak_pb_message           *pbresp,
                                riak_serverinfo_response **resp,
                                riak_boolean_t            *done) {
    // decode the PB response etc
    riak_config *cfg = riak_operation_get_config(rop);
    RpbGetServerInfoResp *rpbresp = rpb_get_server_info_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_log_debug(cxn, "riak_decode_server_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    *done = RIAK_TRUE;
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_serverinfo_response *response = (riak_serverinfo_response*)(cfg->malloc_fn)(sizeof(riak_serverinfo_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_serverinfo_response));
    response->_internal = rpbresp;

    if (rpbresp->has_node) {
        response->has_node = RIAK_TRUE;
        response->node = riak_binary_populate_from_pb(cfg, &(rpbresp->node));
        if (response->node == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    if (rpbresp->has_server_version) {
        response->has_server_version = RIAK_TRUE;
        response->server_version = riak_binary_populate_from_pb(cfg, &(rpbresp->server_version));
        if (response->server_version == NULL) {
            return ERIAK_OUT_OF_MEMORY;
        }
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_serverinfo_response_print(riak_serverinfo_response *response,
                               char                     *target,
                               riak_size_t               len) {
    riak_int32_t left_to_write = len;
    riak_int32_t wrote;
    char buffer[1024];
    if ((response->has_node) && (left_to_write > 0)) {
        riak_binary_print(response->node, buffer, sizeof(buffer));
        wrote = snprintf(target, left_to_write, "Node: %s\n", buffer);
        left_to_write -= wrote;
        target += wrote;
    }
    if ((response->has_server_version) && (left_to_write > 0)) {
        riak_binary_print(response->server_version, buffer, sizeof(buffer));
        wrote = snprintf(target, left_to_write, "Version: %s\n", buffer);
        left_to_write -= wrote;
        target += wrote;
    }
}

void
riak_serverinfo_response_free(riak_config               *cfg,
                              riak_serverinfo_response **resp) {
    riak_serverinfo_response *response = *resp;
    if (response == NULL) return;
    rpb_get_server_info_resp__free_unpacked(response->_internal, cfg->pb_allocator);
    riak_free(cfg, &(response->node));
    riak_free(cfg, &(response->server_version));
    riak_free(cfg, resp);
}

riak_boolean_t
riak_serverinfo_get_has_node(riak_serverinfo_response *resp) {
    return resp->has_node;
}

riak_binary*
riak_serverinfo_get_node(riak_serverinfo_response *resp) {
    return resp->node;
}

riak_boolean_t
riak_serverinfo_get_has_server_version(riak_serverinfo_response *resp) {
    return resp->has_server_version;
}

riak_binary*
riak_serverinfo_get_server_version(riak_serverinfo_response *resp) {
    return resp->server_version;
}

