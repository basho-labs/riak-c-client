/*********************************************************************
 *
 * riak_connection.c: Management of the Riak event
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
#include "riak_connection.h"
#include "riak_config-internal.h"
#include "riak_connection-internal.h"
#include "riak_network.h"

riak_error
riak_connection_new(riak_config      *cfg,
               riak_connection      **cxn_target,
               riak_response_callback response_cb,
               riak_response_callback error_cb,
               void                  *cb_data) {

    riak_connection *cxn = (riak_connection*)(cfg->malloc_fn)(sizeof(riak_connection));
    if (cxn == NULL) {
        riak_log_fatal_config(cfg, "%s", "Could not allocate a riak_connection");
        return ERIAK_OUT_OF_MEMORY;
    }
    *cxn_target = cxn;
    cxn->base = riak_config_get_base(cfg); // Keep a copy until interface has settled
    cxn->config = cfg;
    cxn->decoder = NULL;
    cxn->response_cb = response_cb;
    cxn->error_cb = error_cb;
    cxn->cb_data = cb_data;
    cxn->position = 0;
    cxn->msglen = 0;
    cxn->msgbuf = NULL;
    cxn->msglen_complete = RIAK_FALSE;
    cxn->pb_request = NULL;
    cxn->pb_response = NULL;
    cxn->response = NULL;
    cxn->error = NULL;

    // TODO: Implement retry logic
    cxn->fd = riak_just_open_a_socket(cfg, cfg->addrinfo);
    if (cxn->fd < 0) {
        riak_log_fatal_config(cfg, "%s", "Could not just open a socket");
        return ERIAK_LOGGING;
    }

    // cxn->bevent = buffecxnent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS|BEV_OPT_THREADSAFE);
    cxn->bevent = bufferevent_socket_new(cxn->base, cxn->fd, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);
    if (cxn->bevent == NULL) {
        riak_log_fatal_config(cfg, "Could not create bufferevent [fd %d]", cxn->fd);
        return ERIAK_OUT_OF_MEMORY;
    }
    int enabled = bufferevent_enable(cxn->bevent, EV_READ|EV_WRITE);
    if (enabled != 0) {
        riak_log_fatal_config(cfg, "Could not enable bufferevent [fd %d]", cxn->fd);
        return ERIAK_EVENT;
    }

    // Set the internal read and write callbacks
    bufferevent_setcb(cxn->bevent, riak_read_result_callback, riak_write_callback, riak_connection_callback, cxn_target);

    return ERIAK_OK;
}

void
riak_connection_set_cb_data(riak_connection *cxn,
                            void            *cb_data) {
    cxn->cb_data = cb_data;
}
void
riak_connection_set_response_cb(riak_connection       *cxn,
                                riak_response_callback cb) {
    cxn->response_cb = cb;
}

void
riak_connection_set_error_cb(riak_connection       *cxn,
                             riak_response_callback cb) {
    cxn->error_cb = cb;
}

void
riak_connection_set_response_decoder(riak_connection      *cxn,
                                     riak_response_decoder decoder) {
    cxn->decoder = decoder;
}

void riak_connection_free(riak_connection** re) {
    riak_connection *cxn = *re;
    if (re == NULL) return;
    riak_free_fn freer = cxn->config->free_fn;
    if (cxn->bevent) {
        //buffecxnent_disable(cxn->bevent, EV_READ|EV_WRITE);
        bufferevent_free(cxn->bevent);
    }
    if (cxn->pb_request) {
        riak_pb_message_free(cxn->config, &(cxn->pb_request));
    }
    if (cxn->fd) close(cxn->fd);
    (freer)(*re);
    *re = NULL;
}

void
riak_connection_loop(riak_config *cfg) {
    event_base_dispatch(riak_config_get_base(cfg));
}

riak_socket_t
riak_connection_get_fd(riak_connection *cxn) {
    return cxn->fd;
}
riak_config*
riak_connection_get_config(riak_connection *cxn) {
    return cxn->config;
}

riak_server_error*
riak_connection_get_server_error(riak_connection *cxn) {
    return cxn->error;
}

