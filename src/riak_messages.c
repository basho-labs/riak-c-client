/*********************************************************************
 *
 * riak_pb_message.c: Riak C Client Protocol Buffer Message
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

riak_pb_message*
riak_pb_message_new(riak_config  *cfg,
                    riak_uint8_t  msgtype,
                    riak_size_t   msglen,
                    riak_uint8_t *buffer) {
    riak_pb_message *pb = (riak_pb_message*)riak_config_clean_allocate(cfg, sizeof(riak_pb_message));
    if (pb != NULL) {
        pb->msgid   = msgtype;
        pb->len     = msglen;
        pb->data    = buffer;
    }
    return pb;
}

void
riak_pb_message_free(riak_config      *cfg,
                     riak_pb_message **pb) {
    riak_free(cfg, &((*pb)->data));
    riak_free(cfg, pb);
}
