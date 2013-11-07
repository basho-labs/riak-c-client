/*********************************************************************
 *
 * riak_event.c: Management of the Riak event
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
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_event.h"
#include "riak_context-internal.h"
#include "riak_event-internal.h"
#include "riak_network.h"

riak_error
riak_event_new(riak_context          *ctx,
               riak_event           **rev_target,
               riak_response_callback response_cb,
               riak_response_callback error_cb,
               void                  *cb_data) {

    riak_event *rev = (riak_event*)(ctx->malloc_fn)(sizeof(riak_event));
    if (rev == NULL) {
        riak_log_fatal_context(ctx, "%s", "Could not allocate a riak_event");
        return ERIAK_OUT_OF_MEMORY;
    }
    *rev_target = rev;
    rev->base = riak_context_get_base(ctx); // Keep a copy until interface has settled
    rev->context = ctx;
    rev->decoder = NULL;
    rev->response_cb = response_cb;
    rev->error_cb = error_cb;
    rev->cb_data = cb_data;
    rev->position = 0;
    rev->msglen = 0;
    rev->msgbuf = NULL;
    rev->msglen_complete = RIAK_FALSE;
    rev->pb_request = NULL;
    rev->pb_response = NULL;
    rev->response = NULL;
    rev->error = NULL;

    // TODO: Implement retry logic
    rev->fd = riak_just_open_a_socket(ctx, ctx->addrinfo);
    if (rev->fd < 0) {
        riak_log_fatal_context(ctx, "%s", "Could not just open a socket");
        return ERIAK_LOGGING;
    }

    // rev->bevent = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS|BEV_OPT_THREADSAFE);
    rev->bevent = bufferevent_socket_new(rev->base, rev->fd, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
    if (rev->bevent == NULL) {
        riak_log_fatal_context(ctx, "Could not create bufferevent [fd %d]", rev->fd);
        return ERIAK_OUT_OF_MEMORY;
    }
    int enabled = bufferevent_enable(rev->bevent, EV_READ|EV_WRITE);
    if (enabled != 0) {
        riak_log_fatal_context(ctx, "Could not enable bufferevent [fd %d]", rev->fd);
        return ERIAK_EVENT;
    }

    // Set the internal read and write callbacks
    bufferevent_setcb(rev->bevent, riak_read_result_callback, riak_write_callback, riak_event_callback, rev_target);

    return ERIAK_OK;
}

void
riak_event_set_cb_data(riak_event *rev,
                       void       *cb_data) {
    rev->cb_data = cb_data;
}
void
riak_event_set_response_cb(riak_event             *rev,
                           riak_response_callback  cb) {
    rev->response_cb = cb;
}

void
riak_event_set_error_cb(riak_event             *rev,
                        riak_response_callback  cb) {
    rev->error_cb = cb;
}

void
riak_event_set_response_decoder(riak_event           *rev,
                                riak_response_decoder decoder) {
    rev->decoder = decoder;
}

void riak_event_free(riak_event** re) {
    riak_event *rev = *re;
    if (re == NULL) return;
    riak_free_fn freer = rev->context->free_fn;
    if (rev->bevent) {
        //bufferevent_disable(rev->bevent, EV_READ|EV_WRITE);
        bufferevent_free(rev->bevent);
    }
    if (rev->pb_request) {
        riak_pb_message_free(rev->context, &(rev->pb_request));
    }
    if (rev->fd) close(rev->fd);
    (freer)(*re);
    *re = NULL;
}

void
riak_event_loop(riak_context *ctx) {
    event_base_dispatch(riak_context_get_base(ctx));
}

riak_socket_t
riak_event_get_fd(riak_event *rev) {
    return rev->fd;
}
riak_context*
riak_event_get_context(riak_event *rev) {
    return rev->context;
}

riak_server_error*
riak_event_get_server_error(riak_event *rev) {
    return rev->error;
}

