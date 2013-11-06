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
#include "riak_event-internal.h"
#include "riak_call_backs.h"
#include "riak_context-internal.h"

// TEST CALLBACKS
//
void ping_cb(riak_ping_response *response, void *ptr) {
    riak_event *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "ping_cb");
    riak_log_debug(rev, "%s", "PONG");
    riak_free_ping_response(rev->context, &response);
}

void serverinfo_cb(riak_serverinfo_response *response, void *ptr) {
    riak_event *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "serverinfo_cb");
    char output[10240];
    riak_print_serverinfo_response(response, output, sizeof(output));
    riak_log_debug(rev, "%s", output);
    riak_free_serverinfo_response(rev->context, &response);
}

void listbucket_cb(riak_listbuckets_response *response, void *ptr) {
    riak_event *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "listbucket_cb");
    char output[10240];
    riak_print_listbuckets_response(response, output, sizeof(output));
    riak_log_debug(rev, "%s", output);
    fflush(stdout);
    riak_free_listbuckets_response(rev->context, &response);
}

void listkey_cb(riak_listkeys_response *response, void *ptr) {
    riak_event *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "listkey_cb");
    char output[10240];
    riak_print_listkeys_response(response, output, sizeof(output));
    riak_log_debug(rev, "%s", output);
    fflush(stdout);
    riak_free_listkeys_response(rev->context, (riak_listkeys_response**)&(response));
}

void get_cb(riak_get_response *response, void *ptr) {
    riak_event *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "get_cb");
    char output[10240];
    riak_print_get_response(response, output, sizeof(output));
    riak_log_debug(rev, "%s", output);
    riak_free_get_response(rev->context, &response);
}

void put_cb(riak_put_response *response, void *ptr) {
    riak_event   *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "put_cb");
    char output[10240];
    riak_print_put_response(response, output, sizeof(output));
    riak_log_debug(rev, "%s", output);
    riak_free_put_response(rev->context, &response);
}

void delete_cb(riak_delete_response *response, void *ptr) {
    riak_event   *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "delete_cb");
    riak_free_delete_response(rev->context, &response);
}

void getclientid_cb(riak_get_clientid_response *response, void *ptr) {
    riak_event   *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "getclientid_cb");
    char output[10240];
    riak_print_get_clientid_response(response, output, sizeof(output));
    riak_log_debug(rev, "%s", output);
    riak_free_get_clientid_response(rev->context, &response);
}

void setclientid_cb(riak_set_clientid_response *response, void *ptr) {
    riak_event *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "setclientid_cb");
    riak_free_set_clientid_response(rev->context, &response);
}

void getbucketprops_cb(riak_get_bucketprops_response *response, void *ptr) {
    riak_event *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "getbucketprops_cb");
    char output[10240];
    riak_print_get_bucketprops_response(response, output, sizeof(output));
    riak_log_debug(rev, "%s", output);
    fflush(stdout);
    riak_free_get_bucketprops_response(rev->context, (riak_get_bucketprops_response**)&(response));
}

void resetbucketprops_cb(riak_reset_bucketprops_response *response, void *ptr) {
    riak_event *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "resetbucketprops_cb");
    riak_free_reset_bucketprops_response(rev->context, (riak_reset_bucketprops_response**)&(response));
}

void setbucketprops_cb(riak_set_bucketprops_response *response, void *ptr) {
    riak_event *rev = (riak_event*)ptr;
    riak_log_debug(rev, "%s", "setbucketprops_cb");
    riak_free_set_bucketprops_response(rev->context, (riak_set_bucketprops_response**)&(response));
}

//
// SYNCHRONOUS CALLBACKS
//

void
riak_sync_cb(void *response,
             void *ptr) {
    riak_sync_wrapper *wrapper = (riak_sync_wrapper*)ptr;
#ifdef _RIAK_DEBUG
    riak_event       **rev     = wrapper->rev;
    riak_log_debug(*rev, "%s", "riak_sync_cb");
#endif
    wrapper->response = response;
}


/* LIBEVENT CALLBACKS */

void
riak_event_callback(riak_bufferevent *bev,
                    short             events,
                    void             *ptr)
{
    riak_event   **rev_target = (riak_event**)ptr;
    riak_event    *rev = *rev_target;
    if (events & BEV_EVENT_CONNECTED) {
         riak_log_debug(rev, "%s","Connect okay.");
    } else if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) {
         char *reason = "BEV_EVENT_ERROR";
         if (events & BEV_EVENT_ERROR) {
            reason = "BEV_EVENT_EOF";
            int err = bufferevent_socket_get_dns_error(bev);
            if (err)
                riak_log_error(rev, "DNS error: %s", evutil_gai_strerror(err));
         }
#ifdef _RIAK_DEBUG
         struct evbuffer *ev_read  = bufferevent_get_input(bev);
         struct evbuffer *ev_write = bufferevent_get_output(bev);
#endif
         riak_log_debug(rev, "Closing because of %s [read event=%p, write event=%p]",
                 reason, (void*)ev_read, (void*)ev_write);
         bufferevent_free(bev);
         event_base_loopexit(rev->base, NULL);
    } else if (events & BEV_EVENT_TIMEOUT) {
        riak_log_debug(rev, "%s","Timeout Event");
        bufferevent_free(bev);
        event_base_loopexit(rev->base, NULL);
    } else {
        riak_log_debug(rev, "Event %d", events);
    }
}


void
riak_write_callback(riak_bufferevent *bev,
                    void             *ptr)
{
#ifdef _RIAK_DEBUG
    riak_event   **rev_target = (riak_event**)ptr;
    riak_event    *rev = *rev_target;
    struct evbuffer *buf = bufferevent_get_output(bev);
    riak_log_debug(rev, "Ready for write with event %p.\n", (void*)buf);
#endif
}


// MAIN RESULT CALLBACK
void
riak_read_result_callback(riak_bufferevent *bev,
                          void             *ptr) {
    riak_event   **rev_target = (riak_event**)ptr;
    riak_event    *rev = *rev_target;
    riak_context  *ctx = (riak_context*)(rev->context);
    riak_boolean_t done_streaming = RIAK_FALSE;
    riak_size_t    buflen;
    while(RIAK_TRUE) {
        // Are we in the middle of a message already?
        if (rev->msglen_complete == RIAK_FALSE) {
            // Read the first 32-bits which are the message size
            // However, if a few size bytes were included during the last read, add them in
            riak_uint32_t  inmsglen = rev->msglen;
            riak_size_t remaining_msg_len = sizeof(inmsglen) - rev->position;
            riak_uint8_t *target = (riak_uint8_t*)(&inmsglen);
            target += rev->position;
            buflen = bufferevent_read(bev, target, remaining_msg_len);
            target = (riak_uint8_t*)(&inmsglen);
            // If we can't ready any more bytes, stop trying
            if (buflen != remaining_msg_len) {
                riak_log_debug(rev, "Expected %d bytes but received bytes = %d", remaining_msg_len, buflen);
                if (buflen == 0) break;
                // A few message size bytes of next message were in this buffer
                // Stuff the partial size into rev->msglen and note the position
                if (buflen < sizeof(inmsglen)) {
                    rev->position = buflen;
                    rev->msglen = inmsglen;
                    return;
                }
                abort();  // Something is hosed here
            }

            rev->msglen_complete = RIAK_TRUE;
            rev->msglen = ntohl(inmsglen);
            riak_log_debug(rev, "Read msglen = %d", rev->msglen);

            // TODO: Need to malloc new buffer each time?
            rev->msgbuf = (riak_uint8_t*)(ctx->malloc_fn)(rev->msglen);
            if (rev->msgbuf == NULL) {
                riak_log_debug(rev, "%s", "Could not allocate buffer in riak_read_result_callback");
                abort();
            }
        } else {
            riak_log_debug(rev, "%s", "Continuation of partial message");
        }

        riak_uint8_t *current_position = rev->msgbuf;
        current_position += rev->position;
        buflen = bufferevent_read(bev, (void*)current_position, rev->msglen - rev->position);
        riak_log_debug(rev, "read %d bytes at position %d, msglen = %d", buflen, rev->position, rev->msglen);
        rev->position += buflen;
        // Are we done yet? If not, break out and wait for the next callback
        if (rev->position < rev->msglen) {
            riak_log_error(rev, "%s","Partial message received");
            return;
        }
        assert(rev->position == rev->msglen);

        riak_uint8_t msgid = (rev->msgbuf)[0];
        riak_pb_message *pbresp = riak_pb_message_new(ctx, msgid, rev->msglen, rev->msgbuf);
        riak_error result;
        rev->position = 0;  // Reset on success
        rev->msglen = 0;
        rev->msglen_complete = RIAK_FALSE;

        // Response varies by data type
        riak_error_response *err_response = NULL;
        // Assume we are doing a single loop, unless told otherwise
        done_streaming = RIAK_TRUE;
        if (rev->decoder == NULL) {
            riak_log_debug(rev, "%d NOT IMPLEMENTED", msgid);
            abort();
        }
        if (msgid == MSG_RPBERRORRESP) {
            result = riak_decode_error_response(rev, pbresp, &err_response, &done_streaming);
            // Convert error response to a null-terminated string
            char errmsg[2048];
            riak_binary_print(err_response->errmsg, errmsg, sizeof(errmsg));
            riak_log_error(rev, "ERR #%d - %s\n", err_response->errcode, errmsg);
            if (rev->error_cb) {
                (rev->error_cb)(err_response, rev->cb_data);
            }
            rev->response = NULL;
            riak_event_free(rev_target);
            return;
        }
        // Decode the message from Protocol Buffers
        result = (rev->decoder)(rev, pbresp, &(rev->response), &done_streaming);
        riak_free(ctx, &pbresp);
        riak_free(ctx, &rev->msgbuf);

        // Call the user-defined callback for this message, when finished
        if (done_streaming && rev->response_cb) {
            (rev->response_cb)(rev->response, rev->cb_data);
        }

        // Something is amiss
        if (result)
            abort();
    }

    // What has been queued up
    fflush(stdout);
#ifdef _RIAK_DEBUG
    event_base_dump_events(rev->base, stdout);
#endif

    if (done_streaming) {
        riak_event_free(rev_target);
    }
}
