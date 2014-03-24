/*********************************************************************
 *
 * riak_async.c: Riak Asynchronous Operations
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

#include "riak.h"
#include "riak_connection.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_operation-internal.h"

riak_error
riak_async_register_ping(riak_operation        *rop,
                         riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_ping_request_encode(rop, &(rop->pb_request));
}

riak_error
riak_async_register_serverinfo(riak_operation        *rop,
                               riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_serverinfo_request_encode(rop, &(rop->pb_request));
}

riak_error
riak_async_register_get(riak_operation        *rop,
                        riak_binary           *bucket_type,
                        riak_binary           *bucket,
                        riak_binary           *key,
                        riak_get_options      *get_options,
                        riak_response_callback cb) {

    riak_operation_set_response_cb(rop, cb);
    return riak_get_request_encode(rop, bucket_type, bucket, key, get_options, &(rop->pb_request));
}

riak_error
riak_async_register_put(riak_operation        *rop,
                        riak_object           *riak_obj,
                        riak_put_options      *options,
                        riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_put_request_encode(rop, riak_obj, options, &(rop->pb_request));
}

riak_error
riak_async_register_delete(riak_operation        *rop,
                           riak_binary           *bucket_type,
                           riak_binary           *bucket,
                           riak_binary           *key,
                           riak_delete_options   *options,
                           riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_delete_request_encode(rop, bucket_type, bucket, key, NULL, &(rop->pb_request));
}

riak_error
riak_async_register_listbuckets(riak_operation        *rop,
                                riak_binary           *bucket_type,
                                riak_uint32_t               timeout,                                                               
                                riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_listbuckets_request_encode(rop, bucket_type, timeout, &(rop->pb_request));
}

riak_error
riak_async_register_listkeys(riak_operation        *rop,
                             riak_binary           *bucket_type,
                             riak_binary           *bucket,
                             riak_uint32_t          timeout,
                             riak_response_callback cb ) {
    riak_operation_set_response_cb(rop, cb);
    return riak_listkeys_request_encode(rop, bucket_type, bucket, timeout, &(rop->pb_request));
}

riak_error
riak_async_register_get_clientid(riak_operation       *rop,
                                 riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_get_clientid_request_encode(rop, &(rop->pb_request));
}

riak_error
riak_async_register_set_clientid(riak_operation  *rop,
                                 riak_binary     *clientid,
                                 riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_set_clientid_request_encode(rop, clientid, &(rop->pb_request));
}

riak_error
riak_async_register_get_bucketprops(riak_operation  *rop,
                                    riak_binary     *bucket_type,
                                    riak_binary     *bucket,
                                    riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_get_bucketprops_request_encode(rop, bucket_type, bucket, &(rop->pb_request));
}

riak_error
riak_async_register_reset_bucketprops(riak_operation  *rop,
                                      riak_binary     *bucket_type,
                                      riak_binary     *bucket,
                                      riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_reset_bucketprops_request_encode(rop, bucket_type, bucket, &(rop->pb_request));
}

riak_error
riak_async_register_set_bucketprops(riak_operation        *rop,
                                    riak_binary           *bucket_type,
                                    riak_binary           *bucket,
                                    riak_bucketprops      *props,
                                    riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_set_bucketprops_request_encode(rop, bucket_type, bucket, props, &(rop->pb_request));
}

riak_error
riak_async_register_mapreduce(riak_operation        *rop,
                              riak_binary           *content_type,
                              riak_binary           *map_request,
                              riak_boolean_t         streaming,
                              riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_mapreduce_request_encode(rop, content_type, map_request, &(rop->pb_request));
}

riak_error
riak_async_register_2index(riak_operation        *rop,
                           riak_binary           *bucket_type,
                           riak_binary           *bucket,
                           riak_binary           *index,
                           riak_2index_options   *index_options,
                           riak_response_callback cb) {
    riak_operation_set_response_cb(rop, cb);
    return riak_2index_request_encode(rop, bucket_type, bucket, index, index_options, &(rop->pb_request));
}

riak_error
riak_async_register_search(riak_operation      *rop,
                           riak_binary         *bucket,
                           riak_binary         *query,
                           riak_search_options *search_options,
                           riak_response_callback cb) {

    riak_operation_set_response_cb(rop, cb);
    return riak_search_request_encode(rop, bucket, query, search_options, &(rop->pb_request));
}

