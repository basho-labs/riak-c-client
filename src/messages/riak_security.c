/*********************************************************************
 *
 * riak_security.c: Riak C Client STARTTLS
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
riak_auth_request_encode(riak_operation   *rop,
                        riak_binary       *user,
                        riak_binary       *password,
                        riak_pb_message  **req) {

    riak_config *cfg = riak_operation_get_config(rop);
    RpbAuthReq authmsg = RPB_AUTH_REQ__INIT;

    riak_binary_copy_to_pb(&authmsg.user, user);
    riak_binary_copy_to_pb(&authmsg.password, password);

    printf("Auth w/ user %s\n", riak_binary_data(user));
    printf("Auth w/ password %s\n", riak_binary_data(password));

    riak_uint32_t msglen = rpb_auth_req__get_packed_size (&authmsg);
    riak_uint8_t* msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(msglen);

    if (msgbuf == NULL) {
        //response->success = RIAK_FALSE;
        return ERIAK_OUT_OF_MEMORY;
    }

    rpb_auth_req__pack (&authmsg, msgbuf);
    riak_pb_message* request = riak_pb_message_new(cfg, MSG_RPBAUTHREQ, msglen, msgbuf);
    if (request == NULL) {
        //response->success = RIAK_FALSE;
        return ERIAK_OUT_OF_MEMORY;
    }
    *req = request;
    riak_operation_set_response_decoder(rop, (riak_response_decoder)riak_auth_response_decode);
    printf("ENCODE COMPLETE\n");
    return ERIAK_OK;

}



riak_error
riak_auth_response_decode(riak_operation      *rop,
                          riak_pb_message     *pbresp,
                          riak_auth_response **resp,
                          riak_boolean_t      *done) {
    printf("DECODING MESSAGE\n");
    riak_config *cfg = riak_operation_get_config(rop);
    riak_auth_response *response = (riak_auth_response*)(cfg->malloc_fn)(sizeof(riak_auth_response));
    *done = RIAK_TRUE;
    if (response == NULL) {
        //response->success = RIAK_FALSE;
        return ERIAK_OUT_OF_MEMORY;
    }
    response->success = RIAK_TRUE;
    *resp = response;

    return ERIAK_OK;
}

void
riak_free_auth_response(riak_config         *cfg,
                        riak_auth_response **resp) {
    riak_free(cfg, resp);
}



