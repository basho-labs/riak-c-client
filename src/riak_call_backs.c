/*********************************************************************
 *
 * call_backs.c: Riak C Message Test Callbacks
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
#include "riak_connection-internal.h"
#include "riak_call_backs.h"
#include "riak_config-internal.h"

// TEST CALLBACKS
//
void
ping_cb(riak_ping_response *response,
       void                *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "ping_cb");
    riak_log_debug(cxn, "%s", "PONG");
    riak_free_ping_response(cxn->config, &response);
}

void
serverinfo_cb(riak_serverinfo_response *response,
              void                     *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "serverinfo_cb");
    char output[10240];
    riak_print_serverinfo_response(response, output, sizeof(output));
    riak_log_debug(cxn, "%s", output);
    riak_free_serverinfo_response(cxn->config, &response);
}

void
listbucket_cb(riak_listbuckets_response *response,
              void                      *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "listbucket_cb");
    char output[10240];
    riak_print_listbuckets_response(response, output, sizeof(output));
    riak_log_debug(cxn, "%s", output);
    fflush(stdout);
    riak_free_listbuckets_response(cxn->config, &response);
}

void
listkey_cb(riak_listkeys_response *response,
           void                   *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "listkey_cb");
    char output[10240];
    riak_print_listkeys_response(response, output, sizeof(output));
    riak_log_debug(cxn, "%s", output);
    fflush(stdout);
    riak_free_listkeys_response(cxn->config, (riak_listkeys_response**)&(response));
}

void
get_cb(riak_get_response *response,
       void              *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "get_cb");
    char output[10240];
    riak_print_get_response(response, output, sizeof(output));
    riak_log_debug(cxn, "%s", output);
    riak_free_get_response(cxn->config, &response);
}

void
put_cb(riak_put_response *response,
       void              *ptr) {
    riak_connection   *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "put_cb");
    char output[10240];
    riak_print_put_response(response, output, sizeof(output));
    riak_log_debug(cxn, "%s", output);
    riak_free_put_response(cxn->config, &response);
}

void
delete_cb(riak_delete_response *response,
          void                 *ptr) {
    riak_connection   *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "delete_cb");
    riak_free_delete_response(cxn->config, &response);
}

void
getclientid_cb(riak_get_clientid_response *response,
               void                       *ptr) {
    riak_connection   *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "getclientid_cb");
    char output[10240];
    riak_print_get_clientid_response(response, output, sizeof(output));
    riak_log_debug(cxn, "%s", output);
    riak_free_get_clientid_response(cxn->config, &response);
}

void
setclientid_cb(riak_set_clientid_response *response,
               void                       *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "setclientid_cb");
    riak_free_set_clientid_response(cxn->config, &response);
}

void
getbucketprops_cb(riak_get_bucketprops_response *response,
                  void                          *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "getbucketprops_cb");
    char output[10240];
    riak_print_get_bucketprops_response(response, output, sizeof(output));
    riak_log_debug(cxn, "%s", output);
    fflush(stdout);
    riak_free_get_bucketprops_response(cxn->config, (riak_get_bucketprops_response**)&(response));
}

void
resetbucketprops_cb(riak_reset_bucketprops_response *response,
                    void                            *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "resetbucketprops_cb");
    riak_free_reset_bucketprops_response(cxn->config, (riak_reset_bucketprops_response**)&(response));
}

void
setbucketprops_cb(riak_set_bucketprops_response *response,
                  void                          *ptr) {
    riak_connection *cxn = (riak_connection*)ptr;
    riak_log_debug(cxn, "%s", "setbucketprops_cb");
    riak_free_set_bucketprops_response(cxn->config, (riak_set_bucketprops_response**)&(response));
}

//
// SYNCHRONOUS CALLBACKS
//

void
riak_sync_cb(void *response,
             void *ptr) {
    riak_sync_wrapper *wrapper = (riak_sync_wrapper*)ptr;
#ifdef _RIAK_DEBUG
    riak_connection       **cxn     = wrapper->cxn;
    riak_log_debug(*cxn, "%s", "riak_sync_cb");
#endif
    wrapper->response = response;
}


/* LIBEVENT CALLBACKS */

void
riak_connection_callback(riak_bufferevent *bev,
                         short             events,
                         void             *ptr)
{
    riak_connection   **cxn_target = (riak_connection**)ptr;
    riak_connection    *cxn = *cxn_target;
    if (events & BEV_EVENT_CONNECTED) {
         riak_log_debug(cxn, "%s","Connect okay.");
    } else if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) {
         char *reason = "BEV_EVENT_ERROR";
         if (events & BEV_EVENT_ERROR) {
            reason = "BEV_EVENT_EOF";
            int err = bufferevent_socket_get_dns_error(bev);
            if (err)
                riak_log_error(cxn, "DNS error: %s", evutil_gai_strerror(err));
         }
#ifdef _RIAK_DEBUG
         struct evbuffer *ev_read  = buffecxnent_get_input(bev);
         struct evbuffer *ev_write = buffecxnent_get_output(bev);
#endif
         riak_log_debug(cxn, "Closing because of %s [read event=%p, write event=%p]",
                 reason, (void*)ev_read, (void*)ev_write);
         bufferevent_free(bev);
         event_base_loopexit(cxn->base, NULL);
    } else if (events & BEV_EVENT_TIMEOUT) {
        riak_log_debug(cxn, "%s","Timeout Event");
        bufferevent_free(bev);
        event_base_loopexit(cxn->base, NULL);
    } else {
        riak_log_debug(cxn, "Event %d", events);
    }
}


void
riak_write_callback(riak_bufferevent *bev,
                    void             *ptr)
{
#ifdef _RIAK_DEBUG
    riak_connection   **cxn_target = (riak_connection**)ptr;
    riak_connection    *cxn = *cxn_target;
    struct evbuffer *buf = buffecxnent_get_output(bev);
    riak_log_debug(cxn, "Ready for write with event %p.\n", (void*)buf);
#endif
}


// MAIN RESULT CALLBACK
void
riak_read_result_callback(riak_bufferevent *bev,
                          void             *ptr) {
    riak_connection   **cxn_target = (riak_connection**)ptr;
    riak_connection    *cxn = *cxn_target;
    riak_config  *cfg = (riak_config*)(cxn->config);
    riak_boolean_t done_streaming = RIAK_FALSE;
    riak_size_t    buflen;
    while(RIAK_TRUE) {
        // Are we in the middle of a message already?
        if (cxn->msglen_complete == RIAK_FALSE) {
            // Read the first 32-bits which are the message size
            // However, if a few size bytes were included during the last read, add them in
            riak_uint32_t  inmsglen = cxn->msglen;
            riak_size_t remaining_msg_len = sizeof(inmsglen) - cxn->position;
            riak_uint8_t *target = (riak_uint8_t*)(&inmsglen);
            target += cxn->position;
            buflen = bufferevent_read(bev, target, remaining_msg_len);
            target = (riak_uint8_t*)(&inmsglen);
            // If we can't ready any more bytes, stop trying
            if (buflen != remaining_msg_len) {
                riak_log_debug(cxn, "Expected %d bytes but received bytes = %d", remaining_msg_len, buflen);
                if (buflen == 0) break;
                // A few message size bytes of next message were in this buffer
                // Stuff the partial size into cxn->msglen and note the position
                if (buflen < sizeof(inmsglen)) {
                    cxn->position = buflen;
                    cxn->msglen = inmsglen;
                    return;
                }
                abort();  // Something is hosed here
            }

            cxn->msglen_complete = RIAK_TRUE;
            cxn->msglen = ntohl(inmsglen);
            riak_log_debug(cxn, "Read msglen = %d", cxn->msglen);

            // TODO: Need to malloc new buffer each time?
            cxn->msgbuf = (riak_uint8_t*)(cfg->malloc_fn)(cxn->msglen);
            if (cxn->msgbuf == NULL) {
                riak_log_debug(cxn, "%s", "Could not allocate buffer in riak_read_result_callback");
                abort();
            }
        } else {
            riak_log_debug(cxn, "%s", "Continuation of partial message");
        }

        riak_uint8_t *current_position = cxn->msgbuf;
        current_position += cxn->position;
        buflen = bufferevent_read(bev, (void*)current_position, cxn->msglen - cxn->position);
        riak_log_debug(cxn, "read %d bytes at position %d, msglen = %d", buflen, cxn->position, cxn->msglen);
        cxn->position += buflen;
        // Are we done yet? If not, break out and wait for the next callback
        if (cxn->position < cxn->msglen) {
            riak_log_error(cxn, "%s","Partial message received");
            return;
        }
        assert(cxn->position == cxn->msglen);

        riak_uint8_t msgid = (cxn->msgbuf)[0];
        riak_pb_message *pbresp = riak_pb_message_new(cfg, msgid, cxn->msglen, cxn->msgbuf);
        riak_error result;
        cxn->position = 0;  // Reset on success
        cxn->msglen = 0;
        cxn->msglen_complete = RIAK_FALSE;

        // Response varies by data type
        riak_error_response *err_response = NULL;
        // Assume we are doing a single loop, unless told otherwise
        done_streaming = RIAK_TRUE;
        if (cxn->decoder == NULL) {
            riak_log_debug(cxn, "%d NOT IMPLEMENTED", msgid);
            abort();
        }
        if (msgid == MSG_RPBERRORRESP) {
            result = riak_decode_error_response(cxn, pbresp, &err_response, &done_streaming);
            // Convert error response to a null-terminated string
            char errmsg[2048];
            riak_binary_print(err_response->errmsg, errmsg, sizeof(errmsg));
            riak_log_error(cxn, "ERR #%d - %s\n", err_response->errcode, errmsg);
            if (cxn->error_cb) {
                (cxn->error_cb)(err_response, cxn->cb_data);
            }
            cxn->response = NULL;
            riak_connection_free(cxn_target);
            return;
        }
        // Decode the message from Protocol Buffers
        result = (cxn->decoder)(cxn, pbresp, &(cxn->response), &done_streaming);
        riak_free(cfg, &pbresp);
        riak_free(cfg, &cxn->msgbuf);

        // Call the user-defined callback for this message, when finished
        if (done_streaming && cxn->response_cb) {
            (cxn->response_cb)(cxn->response, cxn->cb_data);
        }

        // Something is amiss
        if (result)
            abort();
    }

    // What has been queued up
    fflush(stdout);
#ifdef _RIAK_DEBUG
    event_base_dump_events(cxn->base, stdout);
#endif

    if (done_streaming) {
        riak_connection_free(cxn_target);
    }
}
