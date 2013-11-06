/*********************************************************************
 *
 * riak_async.c: Riak Asynchronous Operations
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

#include "riak.h"
#include "riak_call_backs.h"
#include "riak_event.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_event-internal.h"

riak_error
riak_async_create_event(riak_context *ctx,
                        riak_event  **rev) {
    riak_error err = riak_event_new(ctx, rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    // For convenience have user callback know about its riak_event
    riak_event_set_cb_data(*rev, *rev);

    return ERIAK_OK;
}

riak_error
riak_async_send_msg(riak_event *rev) {
    return riak_send_req(rev, rev->pb_request);
}

riak_error
riak_async_register_ping(riak_event            *rev,
                         riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_ping_request(rev, &(rev->pb_request));
}

riak_error
riak_async_register_serverinfo(riak_event            *rev,
                               riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_serverinfo_request(rev, &(rev->pb_request));
}

riak_error
riak_async_register_get(riak_event            *rev,
                        riak_binary           *bucket,
                        riak_binary           *key,
                        riak_get_options      *get_options,
                        riak_response_callback cb) {

    riak_event_set_response_cb(rev, cb);
    return riak_encode_get_request(rev, bucket, key, get_options, &(rev->pb_request));
}

riak_error
riak_async_register_put(riak_event            *rev,
                        riak_object           *riak_obj,
                        riak_put_options      *options,
                        riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_put_request(rev, riak_obj, options, &(rev->pb_request));
}

riak_error
riak_async_register_delete(riak_event            *rev,
                           riak_binary           *bucket,
                           riak_binary           *key,
                           riak_delete_options   *options,
                           riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_delete_request(rev, bucket, key, NULL, &(rev->pb_request));
}

riak_error
riak_async_register_listbuckets(riak_event            *rev,
                                riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_listbuckets_request(rev, &(rev->pb_request));
}

riak_error
riak_async_register_listkeys(riak_event            *rev,
                             riak_binary           *bucket,
                             riak_uint32_t          timeout,
                             riak_response_callback cb ) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_listkeys_request(rev, bucket, timeout, &(rev->pb_request));
}

riak_error
riak_async_register_get_clientid(riak_event            *rev,
                                 riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_get_clientid_request(rev, &(rev->pb_request));
}

riak_error
riak_async_register_set_clientid(riak_event            *rev,
                                 riak_binary           *clientid,
                                 riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_set_clientid_request(rev, clientid, &(rev->pb_request));
}

riak_error
riak_async_register_get_bucketprops(riak_event            *rev,
                                    riak_binary           *bucket,
                                    riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_get_bucketprops_request(rev, bucket, &(rev->pb_request));
}

riak_error
riak_async_register_reset_bucketprops(riak_event            *rev,
                                      riak_binary           *bucket,
                                      riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_reset_bucketprops_request(rev, bucket, &(rev->pb_request));
}

riak_error
riak_async_register_set_bucketprops(riak_event            *rev,
                                    riak_binary           *bucket,
                                    riak_bucket_props     *props,
                                    riak_response_callback cb) {
    riak_event_set_response_cb(rev, cb);
    return riak_encode_set_bucketprops_request(rev, bucket, props, &(rev->pb_request));
}

