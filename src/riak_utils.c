/*********************************************************************
 *
 * utils.c: Riak C Client Utilities
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
#include "riak_binary-internal.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_config-internal.h"
#include "riak_connection-internal.h"

size_t
riak_strlcpy(char       *dst,
             const char *src,
             size_t      size) {
    if (size <= 0) return 0;

    size_t len;
    for(len = 0; (len < size) && (*src != '\0'); len++) {
        *dst++ = *src++;
    }
    *dst = '\0';

    return len;
}

size_t
riak_strlcat(char       *dst,
             const char *src,
             size_t      size) {
    if (size <= 0) return 0;

    size_t len;
    // Walk to the end of the first string
    for(len = 0; (len < size) && (*dst != '\0'); len++, dst++) {}
    for(; (len < size) && (*src != '\0'); len++) {
        *dst++ = *src++;
    }
    *dst = '\0';

    return len;
}

void**
riak_array_realloc(riak_config *cfg,
                   void       ***from,
                   riak_size_t   size,
                   riak_uint32_t oldnum,
                   riak_uint32_t newnum) {
    void** new_array = (void**)(cfg->malloc_fn)(newnum*size);
    if (new_array == NULL) {
        return NULL;
    }
    // Just for good measure, clear out memory
    memset((void*)new_array, '\0', newnum*size);
    memcpy((void*)new_array, (void*)(*from), oldnum*size);
    riak_free(cfg, from);
    *from = new_array;
    return (*from);
}


void
riak_free_internal(riak_config *cfg,
                   void       **pp) {
    if(pp != NULL && *pp != NULL) {
        (cfg->free_fn)(*pp);
        *pp = NULL;
    }
}

riak_error
riak_send_req(riak_connection *cxn,
              riak_pb_message *req) {
    riak_bufferevent *bev    = cxn->bevent;
    riak_uint8_t      reqid  = req->msgid;
    riak_uint8_t     *msgbuf = req->data;
    riak_size_t       len    = req->len;
    // Convert len to network byte order
    ev_uint32_t msglen = htonl(len+1);
    int result = bufferevent_write(bev, (void*)&msglen, sizeof(msglen));
    if (result != 0) return ERIAK_WRITE;
    result = bufferevent_write(bev, (void*)&reqid, sizeof(reqid));
    if (result != 0) return ERIAK_WRITE;
    if (msglen > 0) {
        result = bufferevent_write(bev, (void*)msgbuf, len);
        if (result != 0) return ERIAK_WRITE;
    }
    riak_log_debug(cxn, "Wrote %d bytes\n", (int)len);
#ifdef _RIAK_DEBUG
    char buffer[10240];
    riak_size_t buflen = sizeof(buffer);
    char *pos = buffer;
    riak_int32_t wrote = 0;
    int i;
    for(i = 0; buflen > 0 && i < len; i++) {
        wrote = snprintf(pos, buflen, "%02x", msgbuf[i]);
        pos += wrote;
        buflen -= wrote;
    }
    fprintf(stdout, "\n");
    for(i = 0; buflen > 0 && i < len; i++) {
        char c = '.';
        if (msgbuf[i] > 31 && msgbuf[i] < 128) {
            c = msgbuf[i];
        }
        wrote = snprintf(pos, buflen, "%c", c);
        pos += wrote;
        buflen -= wrote;
    }
#endif
    return ERIAK_OK;
}
