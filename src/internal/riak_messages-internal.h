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
#include "messages/riak_put-internal.h"
#include "messages/riak_listbuckets-internal.h"
#include "messages/riak_listkeys-internal.h"
#include "messages/riak_get_bucketprops-internal.h"
#include "messages/riak_set_bucketprops-internal.h"
#include "messages/riak_reset_bucketprops-internal.h"
#include "messages/riak_mapreduce-internal.h"

// Based on RpbErrorResp
struct _riak_error_response {
    riak_uint32_t errcode;
    riak_binary  *errmsg;

    RpbErrorResp *_internal;
};

struct _riak_ping_response {
    riak_boolean_t success;
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

#endif // _RIAK_EVENT_INTERNAL_H
