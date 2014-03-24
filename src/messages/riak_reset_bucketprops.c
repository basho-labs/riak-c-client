/*********************************************************************
 *
 * riak_get.c: Riak C Client Reset Bucket Properties Message
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
riak_reset_bucketprops_request_encode(riak_operation      *rop,
                                      riak_binary         *bucket,
                                      riak_binary         *bucket_type,                                      
                                      riak_pb_message    **req) {

    riak_config *cfg = riak_operation_get_config(rop);
    RpbResetBucketReq resetmsg;
    rpb_reset_bucket_req__init(&resetmsg);
    if(bucket_type != NULL) {
        riak_binary_copy_to_pb(&resetmsg.type, bucket_type);
        resetmsg.has_type = RIAK_TRUE;
    }
    riak_binary_copy_to_pb(&resetmsg.bucket, bucket);

    riak_uint32_t msglen = rpb_reset_bucket_req__get_packed_size(&resetmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);
    if (msgbuf == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    rpb_reset_bucket_req__pack(&resetmsg, msgbuf);

    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBSETBUCKETREQ, msglen, msgbuf);
    if (request == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_reset_bucketprops_response_decode);

    return ERIAK_OK;
}

riak_error
riak_reset_bucketprops_response_decode(riak_operation                   *rop,
                                       riak_pb_message                  *pbresp,
                                       riak_reset_bucketprops_response **resp,
                                       riak_boolean_t                   *done) {
    riak_config *cfg = riak_operation_get_config(rop);
    riak_reset_bucketprops_response *response = (riak_reset_bucketprops_response*)(cfg->malloc_fn)(sizeof(riak_reset_bucketprops_response));
    *done = RIAK_TRUE;
    if (response == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *resp = response;

    return ERIAK_OK;
}

void
riak_reset_bucketprops_response_free(riak_config                      *cfg,
                                     riak_reset_bucketprops_response **resp) {
    riak_free(cfg, resp);
}
