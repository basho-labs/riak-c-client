/*********************************************************************
 *
 * riak_libevent.h: Management of the Riak Libevent Framework
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

#ifdef __cplusplus
extern "C" {
#endif

struct _riak_libevent {
    struct event_base  *base;
    struct bufferevent *bevent;
    riak_operation     *rop;
};

/**
 * @brief Called by Libevent each time a connection is established
 * @param bev Libevent buffer event
 * @param events Bitvector of events
 * @param ptr User-defined pointer (here riak_connection)
 */
void
riak_libevent_connection_cb(struct bufferevent *bev,
                            short               events,
                            void               *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    if (events & BEV_EVENT_CONNECTED) {
         riak_log_debug(cxn, "%s","Connect okay.");
    } else if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) {
         char *reason = (char*)"BEV_EVENT_ERROR";
         if (events & BEV_EVENT_ERROR) {
            reason = (char*)"BEV_EVENT_EOF";
            int err = bufferevent_socket_get_dns_error(bev);
            if (err)
                riak_log_error(cxn, "DNS error: %s", evutil_gai_strerror(err));
         }
#ifdef _RIAK_DEBUG
         struct evbuffer *ev_read  = bufferevent_get_input(bev);
         struct evbuffer *ev_write = bufferevent_get_output(bev);
         riak_log_debug(cxn, "Closing because of %s [read event=%p, write event=%p]",
                 reason, (void*)ev_read, (void*)ev_write);
#endif
         bufferevent_free(bev);
         event_base_loopexit(bufferevent_get_base(bev), NULL);
    } else if (events & BEV_EVENT_TIMEOUT) {
        riak_log_debug(cxn, "%s","Timeout Event");
        bufferevent_free(bev);
        event_base_loopexit(bufferevent_get_base(bev), NULL);
    } else {
        riak_log_debug(cxn, "Event %d", events);
    }
}

/**
 * @brief Called by libevent on a read event
 * @param bev Libevent Bufferevent
 * @param ptr User-supplied pointer (Libevent Operation)
 */

void
riak_libevent_write_event_cb(struct bufferevent *bev,
                             void               *ptr) {
#ifdef _RIAK_DEBUG
    riak_libevent   *event = (riak_libevent*)ptr;
    riak_connection *cxn = riak_operation_get_connection(event->rop);
    struct evbuffer *buf = bufferevent_get_output(bev);
    riak_log_debug(cxn, "Ready for write with event %p.\n", (void*)buf);
#endif
}

/**
 * @brief Callback
 */
riak_ssize_t
riak_libevent_read_cb(void       *ptr,
                      void       *data,
                      riak_size_t size) {
    riak_libevent *event = (riak_libevent*)ptr;
    struct bufferevent *bev   = event->bevent;
    return bufferevent_read(bev, data, size);
}

riak_ssize_t
riak_libevent_write_cb(void       *ptr,
                       void       *data,
                       riak_size_t size) {
    riak_libevent *event = (riak_libevent*)ptr;
    struct bufferevent *bev   = event->bevent;
    int result = bufferevent_write(bev, data, size);
    if (result == 0) {
        return size;
    } else {
        return 0;
    }
}

void
riak_libevent_result_cb(struct bufferevent *bev,
                        void               *ptr) {
    riak_libevent   *event = (riak_libevent*)ptr;
    riak_operation  *rop   = event->rop;
    riak_boolean_t   done_streaming = RIAK_FALSE;

    riak_error err = riak_read(rop,
                               &done_streaming,
                               riak_libevent_read_cb,
                               (void*)event);
    // TODO: How do we communicate back a read error?
    (void)err;

#ifdef _RIAK_DEBUG
    // What has been queued up
    fflush(stdout);
    event_base_dump_events(event->base, stdout);
#endif

    if (done_streaming) {
        bufferevent_free(bev);
    }
}

/**
 * @brief Construct a Riak Event framework
 * @param rev Riak Event (out)
 * @param cxn Riak Connection
 * @returns Error code
 */
riak_error
riak_libevent_new(riak_libevent    **rev_target,
                  riak_operation    *rop,
                  struct event_base *base) {
    riak_config     *cfg = riak_operation_get_config(rop);
    riak_connection *cxn = riak_operation_get_connection(rop);

    riak_libevent *rev = (riak_libevent*)riak_config_clean_allocate(cfg, sizeof(riak_libevent));
    if (rev == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not allocate a riak_libevent");
        return ERIAK_OUT_OF_MEMORY;
    }
    *rev_target = rev;
    rev->rop    = rop;
    rev->base   = base;

    rev->bevent = bufferevent_socket_new(rev->base,
                                         riak_connection_get_fd(cxn),
                                         BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS|BEV_OPT_THREADSAFE);
    if (rev->bevent == NULL) {
        riak_log_critical_config(cfg,
                                 "Could not create bufferevent [fd %d]",
                                 riak_connection_get_fd(cxn));
        return ERIAK_OUT_OF_MEMORY;
    }
    int enabled = bufferevent_enable(rev->bevent, EV_READ|EV_WRITE);
    if (enabled != 0) {
        riak_log_critical_config(cfg,
                                 "Could not enable bufferevent [fd %d]",
                                 riak_connection_get_fd(cxn));
        return ERIAK_EVENT;
    }

    // Set the internal read and write callbacks
    bufferevent_setcb(rev->bevent,
                      riak_libevent_result_cb,
                      riak_libevent_write_event_cb,
                      riak_libevent_connection_cb,
                      rev);

    return ERIAK_OK;
}

void
riak_libevent_free(riak_config    *cfg,
                   riak_libevent **rev_target) {
    if (rev_target == NULL || *rev_target == NULL) {
        return;
    }
    riak_libevent *rev = *rev_target;
    if (rev->bevent) {
        bufferevent_free(rev->bevent);
    }
    riak_free(cfg, rev_target);
    //if (cfg->base != NULL) event_base_free(cfg->base);
}

riak_error
riak_libevent_send(riak_operation *rop,
                   riak_libevent  *rev) {

    return riak_write(rop,
                      riak_libevent_write_cb,
                      (void*)rev);
}
#ifdef __cplusplus
}

#endif

