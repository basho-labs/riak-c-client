/*********************************************************************
 *
 * riak_get.c: Riak C Client Get Bucket Properties Message
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
riak_get_bucketprops_request_encode(riak_operation   *rop,
                                    riak_binary      *bucket,
                                    riak_pb_message **req) {
    riak_config *cfg = riak_operation_get_config(rop);
    RpbGetBucketReq bucketreq;
    rpb_get_bucket_req__init(&bucketreq);

    riak_binary_copy_to_pb(&(bucketreq.bucket), bucket);
    riak_size_t msglen = rpb_get_bucket_req__get_packed_size(&bucketreq);
    riak_uint8_t *msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return 1;
    }
    rpb_get_bucket_req__pack(&bucketreq, msgbuf);

    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBGETBUCKETREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_get_bucketprops_response_decode);

    return ERIAK_OK;

}

riak_error
riak_get_bucketprops_response_decode(riak_operation                 *rop,
                                     riak_pb_message                *pbresp,
                                     riak_get_bucketprops_response **resp,
                                     riak_boolean_t                 *done) {
    // decode the PB response etc
    riak_config *cfg = riak_operation_get_config(rop);
    RpbGetBucketResp *rpbresp = rpb_get_bucket_resp__unpack(cfg->pb_allocator, (pbresp->len)-1, (uint8_t*)((pbresp->data)+1));
    if (rpbresp == NULL) {
        return ERIAK_MESSAGE_FORMAT;
    }
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_log_debug(cxn, "riak_decode_get_bucketprops_response len=%d/pb unpack = 0x%lx\n", pbresp->len, (long)(rpbresp));
    *done = RIAK_TRUE;
    if (rpbresp == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    riak_get_bucketprops_response *response = (riak_get_bucketprops_response*)(cfg->malloc_fn)(sizeof(riak_get_bucketprops_response));
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    memset(response, '\0', sizeof(riak_get_bucketprops_response));
    response->_internal = rpbresp;

    riak_error err = riak_bucketprops_new_from_pb(cfg, &(response->props), rpbresp->props);
    if (err) {
        rpb_get_bucket_resp__free_unpacked(rpbresp, cfg->pb_allocator);
        riak_free(cfg, &response);
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_get_bucketprops_response_free(riak_config                    *cfg,
                                   riak_get_bucketprops_response **resp) {
    riak_get_bucketprops_response *response = *resp;
    if (response == NULL) return;
    riak_bucketprops_free(cfg, &(response->props));
    riak_free(cfg, resp);
}

void
riak_get_bucketprops_response_print(riak_get_bucketprops_response *response,
                                    char                          *target,
                                    riak_size_t                    len) {
    riak_bucketprops_print(response->props, target, len);
}
