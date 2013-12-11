/*********************************************************************
 *
 * riak_messages_internal.h: Riak C Client Implementation-Specific Messages
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

#include "riak_binary-internal.h"

#ifndef _RIAK_INTERNAL_MESSAGES_H
#define _RIAK_INTERNAL_MESSAGES_H

#define MSG_RPBERRORRESP              0

// 0 length
#define MSG_RPBPINGREQ                1

// 0 length
#define MSG_RPBPINGRESP               2

#define MSG_RPBGETCLIENTIDREQ         3

// 0 length
#define MSG_RPBGETCLIENTIDRESP        4

// 0 length
#define MSG_RPBSETCLIENTIDREQ         5
#define MSG_RPBSETCLIENTIDRESP        6
#define MSG_RPBGETSERVERINFOREQ       7
#define MSG_RPBGETSERVERINFORESP      8
#define MSG_RPBGETREQ                 9
#define MSG_RPBGETRESP                10
#define MSG_RPBPUTREQ                 11
#define MSG_RPBPUTRESP                12
#define MSG_RPBDELREQ                 13
#define MSG_RPBDELRESP                14
#define MSG_RPBLISTBUCKETSREQ         15

// streaming
#define MSG_RPBLISTBUCKETSRESP        16
#define MSG_RPBLISTKEYSREQ            17

// streaming
#define MSG_RPBLISTKEYSRESP           18

#define MSG_RPBGETBUCKETREQ           19
#define MSG_RPBGETBUCKETRESP          20
#define MSG_RPBSETBUCKETREQ           21
#define MSG_RPBSETBUCKETRESP          22
#define MSG_RPBMAPREDREQ              23

// streaming
#define MSG_RPBMAPREDRESP             24

#define MSG_RPBINDEXREQ               25
#define MSG_RPBINDEXRESP              26
#define MSG_RPBSEARCHQUERYREQ         27
#define MSG_RBPSEARCHQUERYRESP        28
#define MSG_RPBRESETBUCKETREQ         29
#define MSG_RPBRESETBUCKETRESP        30
#define MSG_RPBGETBUCKETTYPEREQ       31
#define MSG_RPBSETBUCKETTYPEREQ       32
#define MSG_RPBRESETBUCKETTYPEREQ     33
#define MSG_RPBCSBUCKETREQ            40
#define MSG_RPBCSBUCKETRESP           41
#define MSG_RPBCOUNTERUPDATEREQ       50
#define MSG_RPBCOUNTERUPDATERESP      51
#define MSG_RPBCOUNTERGETREQ          52
#define MSG_RPBCOUNTERGETRESP         53
// Yokozuna Search
#define MSG_RPBYOKOZUNAINDEXGETREQ    54
#define MSG_RPBYOKOZUNAINDEXGETRESP   55
#define MSG_RPBYOKOZUNAINDEXPUTREQ    56
#define MSG_RPBYOKOZUNAINDEXDELETEREQ 57
#define MSG_RPBYOKOZUNASCHEMAGETREQ   58
#define MSG_RPBYOKOZUNASCHEMAGETRESP  59
#define MSG_RPBYOKOZUNASCHEMAPUTREQ   60
// CRDTs
#define MSG_DTFETCHREQ                80
#define MSG_DTFETCHRESP               81
#define MSG_DTUPDATEREQ               82
#define MSG_DTUPDATERESP              83
// internal message codes, grow downwards from 255
#define MSG_RPBAUTHREQ                253
#define MSG_RPBAUTHRESP               254
#define MSG_RPBSTARTTLS               255

typedef struct _riak_pb_message {
    riak_uint32_t len;
    riak_uint8_t  msgid;
    riak_uint8_t *data;
} riak_pb_message;

riak_pb_message*
riak_pb_message_new(riak_config *cfg,
                    riak_uint8_t  msgtype,
                    riak_size_t   msglen,
                    riak_uint8_t *buffer);
void
riak_pb_message_free(riak_config     *cfg,
                     riak_pb_message **pb);

#include "messages/riak_serverinfo-internal.h"
#include "messages/riak_get_clientid-internal.h"
#include "messages/riak_set_clientid-internal.h"
#include "messages/riak_delete-internal.h"
#include "messages/riak_get-internal.h"

// Based on RpbPutResp
struct _riak_put_response {
    riak_uint32_t  n_content;
    riak_object  **content; // Array of pointers to match Get
    riak_boolean_t has_vclock;
    riak_binary   *vclock;
    riak_boolean_t has_key;
    riak_binary   *key;

    RpbPutResp   *_internal;
};

// Based on RpbPutReq
struct _riak_put_options {
    riak_boolean_t has_vclock;
    riak_binary   *vclock;
    //RpbContent *content;
    riak_boolean_t has_w;
    riak_uint32_t  w;
    riak_boolean_t has_dw;
    riak_uint32_t  dw;
    riak_boolean_t has_return_body;
    riak_boolean_t return_body;
    riak_boolean_t has_pw;
    riak_uint32_t  pw;
    riak_boolean_t has_if_not_modified;
    riak_boolean_t if_not_modified;

    riak_boolean_t has_if_none_match;
    riak_boolean_t if_none_match;
    riak_boolean_t has_return_head;
    riak_boolean_t return_head;
    riak_boolean_t has_timeout;
    riak_uint32_t  timeout;
    riak_boolean_t has_asis;
    riak_boolean_t asis;
    riak_boolean_t has_sloppy_quorum;
    riak_boolean_t sloppy_quorum;
    riak_boolean_t has_n_val;
    riak_uint32_t  n_val;
};

// Based on RpbListBucketsResp
struct _riak_listbuckets_response {
    riak_uint32_t        n_buckets;
    riak_binary        **buckets; // Array of pointers to allow growth
    riak_boolean_t       done;

    riak_uint32_t        n_responses;
    RpbListBucketsResp **_internal; // Array for many responses
};

// Based on RpbListKeysResp
struct _riak_listkeys_response {
    riak_uint32_t     n_keys;
    riak_binary     **keys; // Array of pointers to allow growth
    riak_boolean_t    done;

    riak_uint32_t     n_responses;
    RpbListKeysResp **_internal; // Array for many responses
};

// Based on RpbErrorResp
struct _riak_error_response {
    riak_uint32_t errcode;
    riak_binary  *errmsg;

    RpbErrorResp *_internal;
};

struct _riak_ping_response {
    riak_boolean_t success;
};

// Based on RpbGetBucketReq
struct _riak_get_bucketprops_request
{
    riak_binary   *bucket;
    riak_boolean_t has_type;
    riak_binary   *type;
};

// Based on RpbGetBucketResp
struct _riak_get_bucketprops_response
{
    riak_bucket_props *props;

    RpbGetBucketResp *_internal;
};

// Based on RpbSetBucketReq
struct _riak_set_bucketprops_request
{
    riak_binary       *bucket;
    riak_bucket_props *props;
    riak_boolean_t     has_type;
    riak_binary       *type;
};

// Placeholder
struct _riak_set_bucketprops_response
{
// Empty
};

// Based on RpbResetBucketReq
struct _riak_reset_bucketprops_request
{
    riak_binary   *bucket;
    riak_boolean_t has_type;
    riak_binary   *type;
};

// Placeholder
struct _riak_reset_bucketprops_response
{
// Empty
};



/**
 * @brief Convert PBC error response into user-readable data type
 * @param rop Riak Operation
 * @param pbresp Protocol Buffer response from Riak
 * @param resp Returned error structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_decode_error_response(riak_operation       *rop,
                           riak_pb_message      *pbresp,
                           riak_error_response **resp,
                           riak_boolean_t       *done);

/**
 * @brief Build a ping request
 * @param rop Riak Operation
 * @param req Created PB message
 * @return Error if out of memory
 */
riak_error
riak_encode_ping_request(riak_operation   *rop,
                         riak_pb_message **req);

/**
 * @brief Convert PBC error response into user-readable data type
 * @param rop Riak Operation
 * @param pbresp Protocol Buffer response from Riak
 * @param resp Returned error structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_decode_ping_response(riak_operation      *rop,
                          riak_pb_message     *pbresp,
                          riak_ping_response **resp,
                          riak_boolean_t      *done);

/**
 * @brief Create Put Request
 * @param rop Riak Operation
 * @param riak_obj Riak object to be put
 * @param options Options to the put request
 * @param req Returned request message
 * @return Error if out of memory
 */
riak_error
riak_encode_put_request(riak_operation   *rop,
                        riak_object      *riak_obj,
                        riak_put_options *options,
                        riak_pb_message **req);

/**
 * @brief Translate PBC put message to a Riak response
 * @param rop Riak Operation
 * @param pbresp Protocol Buffer message
 * @param resp Returned Put message
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_decode_put_response(riak_operation     *rop,
                         riak_pb_message    *pbresp,
                         riak_put_response **resp,
                         riak_boolean_t     *done);

/**
 * @brief Create a request to find all buckets
 * @param rop Riak Operation
 * @param req Returned listbuckets request
 * @return Error if out of memory
 */
riak_error
riak_encode_listbuckets_request(riak_operation   *rop,
                                riak_pb_message **req);

/**
 * @brief Translate PBC listbuckets response into Riak strucuture
 * @param rop Riak Operation
 * @param pbresp PBC response message
 * @param resp Returned Riak response structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_decode_listbuckets_response(riak_operation             *rop,
                                 riak_pb_message            *pbresp,
                                 riak_listbuckets_response **resp,
                                 riak_boolean_t             *done);

/**
 * @brief Create a request to find all keys in a bucket
 * @param rop Riak Operation
 * @param bucket Name of Riak bucket
 * @param timeout How long to wait for a response
 * @param req Returned listbuckets request
 * @return Error if out of memory
 */
riak_error
riak_encode_listkeys_request(riak_operation   *rop,
                                 riak_binary  *bucket,
                                 riak_uint32_t timeout,
                                 riak_pb_message **req);

/**
 * @brief Translate PBC listkeys response into Riak structure
 * @param rop Riak Operation
 * @param pbresp PBC response message
 * @param resp Returned Riak response structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_decode_listkeys_response(riak_operation          *rop,
                              riak_pb_message         *pbresp,
                              riak_listkeys_response **resp,
                              riak_boolean_t          *done);

/**
 * @brief Create a request to fetch bucket properies
 * @param rop Riak Operation
 * @param bucket Name of Riak bucket
 * @param req Returned bucket properies request
 * @return Error if out of memory
 */
riak_error
riak_encode_get_bucketprops_request(riak_operation   *rop,
                                    riak_binary      *bucket,
                                    riak_pb_message **req);

/**
 * @brief Translate PBC get_bucketprops response into Riak structure
 * @param rop Riak Operation
 * @param pbresp PBC response message
 * @param resp Returned Riak response structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_decode_get_bucketprops_response(riak_operation                 *rop,
                                     riak_pb_message                *pbresp,
                                     riak_get_bucketprops_response **resp,
                                     riak_boolean_t                 *done);

/**
 * @brief Create a request to reset bucket properties
 * @param rop Riak Operation
 * @param bucket Name of Riak bucket
 * @param req Returned bucket properties request
 * @return Error if out of memory
 */
riak_error
riak_encode_reset_bucketprops_request(riak_operation   *rop,
                                      riak_binary      *bucket,
                                      riak_pb_message **req);

/**
 * @brief Translate PBC reset_bucketprops response into Riak structure
 * @param rop Riak Operation
 * @param pbresp PBC response message
 * @param resp Returned Riak response structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_decode_reset_bucketprops_response(riak_operation                   *rop,
                                       riak_pb_message                  *pbresp,
                                       riak_reset_bucketprops_response **resp,
                                       riak_boolean_t                   *done);


/**
 * @brief Create a request to set bucket properties
 * @param rop Riak Operation
 * @param bucket Name of Riak bucket
 * @param props Bucket properties
 * @param req Returned bucket properties request
 * @return Error if out of memory
 */
riak_error
riak_encode_set_bucketprops_request(riak_operation    *rop,
                                    riak_binary       *bucket,
                                    riak_bucket_props *props,
                                    riak_pb_message  **req);

/**
 * @brief Translate PBC set_bucketprops response into Riak structure
 * @param rop Riak Operation
 * @param pbresp PBC response message
 * @param resp Returned Riak response structure
 * @param done Returned flag set to true if finished streaming
 * @return Error if out of memory
 */
riak_error
riak_decode_set_bucketprops_response(riak_operation                 *rop,
                                     riak_pb_message                *pbresp,
                                     riak_set_bucketprops_response **resp,
                                     riak_boolean_t                 *done);

#endif // _RIAK_EVENT_INTERNAL_H
