/*********************************************************************
 *
 * riak.c: Riak Operations
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

#include <errno.h>
#include "riak.h"
#include "riak_connection.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_operation-internal.h"

//
// SYNCHRONOUS CALLBACKS
//

riak_ssize_t
riak_sync_read_cb(void       *ptr,
                  void       *data,
                  riak_size_t size) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_socket_t     fd = riak_connection_get_fd(cxn);
    riak_ssize_t  result = read(fd, data, size);
    if (result < 0) {
        char message[256];
        strerror_r(errno, message, sizeof(message));
        fprintf(stderr, "%s\n", message);
    }
    return result;
}

riak_ssize_t
riak_sync_write_cb(void       *ptr,
                   void       *data,
                   riak_size_t size) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_socket_t    fd  = riak_connection_get_fd(cxn);
    return write(fd, data, size);
}

static riak_error
riak_sync_request(riak_operation **rop_target,
                  void           **response) {
    riak_operation  *rop = *rop_target;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_operation_set_cb_data(rop, rop);

    riak_error err = riak_write(rop, riak_sync_write_cb, rop);
    if (err) {
        riak_log_critical(cxn, "%s", "Could not send request");
        riak_operation_free(rop_target);
        return err;
    }

    riak_boolean_t done_streaming;
    err = riak_read(rop, &done_streaming, riak_sync_read_cb, rop);

    *response = rop->response;
    riak_operation_free(rop_target);
    return err;
}

riak_error
riak_ping(riak_connection *cxn) {
    riak_operation *rop = NULL;
    riak_error err   = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    riak_config *cfg = riak_connection_get_config(cxn);
    if (err) {
        return err;
    }
    riak_ping_response *response = NULL;
    err = riak_ping_request_encode(rop, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)&response);
    if (err) {
        return err;
    }
    err = (response->success) ? ERIAK_OK : ERIAK_NO_PING;
    riak_free(cfg, &response);

    return err;
}

riak_error
riak_serverinfo(riak_connection           *cxn,
                riak_serverinfo_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_serverinfo_request_encode(rop, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}


riak_error
riak_get(riak_connection    *cxn,
         riak_binary        *bucket,
         riak_binary        *key,
         riak_get_options   *opts,
         riak_get_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_get_request_encode(rop, bucket, key, opts, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_put(riak_connection    *cxn,
         riak_object        *obj,
         riak_put_options   *opts,
         riak_put_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_put_request_encode(rop, obj, opts, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_delete(riak_connection    *cxn,
           riak_binary         *bucket,
           riak_binary         *key,
           riak_delete_options *opts) {
    riak_operation *rop = NULL;
    riak_config    *cfg = riak_connection_get_config(cxn);
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_delete_request_encode(rop, bucket, key, opts, &(rop->pb_request));
    if (err) {
        return err;
    }
    riak_delete_response *response = NULL;
    err = riak_sync_request(&rop, (void**)&response);
    riak_delete_response_free(cfg, &response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_listbuckets(riak_connection            *cxn,
                 riak_listbuckets_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_listbuckets_request_encode(rop, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }
    return ERIAK_OK;
}

riak_error
riak_listkeys(riak_connection         *cxn,
              riak_binary             *bucket,
              riak_uint32_t            timeout,
              riak_listkeys_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_listkeys_request_encode(rop, bucket, timeout, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }
    return ERIAK_OK;
}

riak_error
riak_get_clientid(riak_connection             *cxn,
                  riak_get_clientid_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_get_clientid_request_encode(rop, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_set_clientid(riak_connection *cxn,
                  riak_binary     *clientid) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_set_clientid_request_encode(rop, clientid, &(rop->pb_request));
    if (err) {
        return err;
    }
    riak_set_clientid_response *response = NULL;
    err = riak_sync_request(&rop, (void**)&response);
    riak_set_clientid_response_free(riak_connection_get_config(cxn), &response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_get_bucketprops(riak_connection                *cxn,
                     riak_binary                    *bucket,
                     riak_get_bucketprops_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_get_bucketprops_request_encode(rop, bucket, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_reset_bucketprops(riak_connection                  *cxn,
                       riak_binary                      *bucket,
                       riak_reset_bucketprops_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_reset_bucketprops_request_encode(rop, bucket, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_set_bucketprops(riak_connection                *cxn,
                     riak_binary                    *bucket,
                     riak_bucketprops               *props,
                     riak_set_bucketprops_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_set_bucketprops_request_encode(rop, bucket, props, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_mapreduce(riak_connection          *cxn,
               riak_binary              *content_type,
               riak_binary              *map_request,
               riak_mapreduce_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_mapreduce_request_encode(rop, content_type, map_request, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_2index(riak_connection       *cxn,
            riak_binary           *bucket,
            riak_binary           *index,
            riak_2index_options   *opts,
            riak_2index_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_2index_request_encode(rop, bucket, index, opts, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_search(riak_connection       *cxn,
            riak_binary           *bucket,
            riak_binary           *query,
            riak_search_options   *opts,
            riak_search_response **response) {
    riak_operation *rop = NULL;
    riak_error err = riak_operation_new(cxn, &rop, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_search_request_encode(rop, bucket, query, opts, &(rop->pb_request));
    if (err) {
        return err;
    }
    err = riak_sync_request(&rop, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_read(riak_operation *rop,
          riak_boolean_t *done_streaming,
          riak_io_cb      read_cb,
          void           *read_cb_data) {
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_size_t      buflen;
    *done_streaming = RIAK_FALSE;

    while(RIAK_TRUE) {
        // Are we in the middle of a message already?
        if (rop->msglen_complete == RIAK_FALSE) {
            // Read the first 32-bits which are the message size
            // However, if a few size bytes were included during the last read, add them in
            riak_uint32_t inmsglen          = rop->msglen;
            riak_size_t   remaining_msg_len = sizeof(inmsglen) - rop->position;
            riak_uint8_t *target            = (riak_uint8_t*)(&inmsglen);
            target += rop->position;
            buflen = (read_cb)(read_cb_data, target, remaining_msg_len);
            target = (riak_uint8_t*)(&inmsglen);
            // If we can't ready any more bytes, stop trying
            if (buflen != remaining_msg_len) {
                riak_log_debug(cxn, "Expected %d bytes but received bytes = %d", remaining_msg_len, buflen);
                if (buflen == 0) break;
                // A few message size bytes of next message were in this buffer
                // Stuff the partial size into rop->msglen and note the position
                if (buflen < sizeof(inmsglen)) {
                    rop->position = buflen;
                    rop->msglen   = inmsglen;
                    return ERIAK_OK;
                }
                return ERIAK_READ;  // Something is hosed here
            }

            rop->msglen_complete = RIAK_TRUE;
            rop->msglen = ntohl(inmsglen);
            riak_log_debug(cxn, "Read msglen = %d", rop->msglen);

            // TODO: Need to malloc new buffer each time?
            rop->msgbuf = (riak_uint8_t*)riak_config_allocate(cfg, rop->msglen);
            if (rop->msgbuf == NULL) {
                riak_log_debug(cxn, "%s", "Could not allocate read buffer");
                return ERIAK_READ;
            }
        } else {
            riak_log_debug(cxn, "%s", "Continuation of partial message");
        }

        riak_uint8_t *current_position = rop->msgbuf;
        current_position += rop->position;
        buflen = (read_cb)(read_cb_data, (void*)current_position, rop->msglen - rop->position);
        riak_log_debug(cxn, "read %d bytes at position %d, msglen = %d", buflen, rop->position, rop->msglen);
        rop->position += buflen;
        // Are we done yet? If not, break out and wait for the next callback
        if (rop->position < rop->msglen) {
            riak_log_error(cxn, "%s","Partial message received");
            return ERIAK_OK;
        }
        assert(rop->position == rop->msglen);

        riak_uint8_t msgid = (rop->msgbuf)[0];
        riak_pb_message *pbresp = riak_pb_message_new(cfg, msgid, rop->msglen, rop->msgbuf);
        riak_error result;
        rop->position = 0;  // Reset on success
        rop->msglen = 0;
        rop->msglen_complete = RIAK_FALSE;

        // Response varies by data type
        riak_server_error_response *err_response = NULL;
        // Assume we are doing a single loop, unless told otherwise
        *done_streaming = RIAK_TRUE;
        if (rop->decoder == NULL) {
            riak_log_debug(cxn, "%d NOT IMPLEMENTED", msgid);
            return ERIAK_READ;
        }
        if (msgid == MSG_RPBERRORRESP) {
            result = riak_server_error_response_decode(rop, pbresp, &err_response, done_streaming);
            // Convert error response to a null-terminated string
            char errmsg[2048];
            riak_binary_print(err_response->errmsg, errmsg, sizeof(errmsg));
            riak_log_error(cxn, "ERR #%d - %s\n", err_response->errcode, errmsg);
            if (rop->error_cb) {
                (rop->error_cb)(err_response, rop->cb_data);
            }
            riak_server_serror_response_free(cfg, &err_response);
            rop->response = NULL;
            return ERIAK_SERVER_ERROR;
        }
        // Decode the message from Protocol Buffers
        result = (rop->decoder)(rop, pbresp, &(rop->response), done_streaming);

        riak_free(cfg, &pbresp);
        riak_free(cfg, &rop->msgbuf);

        // Something is amiss
        if (result)
            return ERIAK_READ;

        // Call the user-defined callback for this message, when finished
        if (*done_streaming) {
            if (rop->response_cb) {
                (rop->response_cb)(rop->response, rop->cb_data);
            }
            break;  // Done with current message
        }
    }

    return ERIAK_OK;
}


// TODO: NOT CHARSET SAFE, need iconv
riak_error
riak_write(riak_operation *rop,
           riak_io_cb      write_cb,
           void           *write_cb_data) {
    riak_pb_message *msg = rop->pb_request;
    riak_uint8_t  reqid  = msg->msgid;
    riak_uint8_t *msgbuf = msg->data;
    riak_size_t   len    = msg->len;

    // Convert len to network byte order
    riak_uint32_t msglen = htonl(len+1);
    riak_int32_t wrote = (write_cb)(write_cb_data, (void*)&msglen, sizeof(msglen));
    if (wrote == 0) return ERIAK_WRITE;
    wrote = (write_cb)(write_cb_data, (void*)&reqid, sizeof(reqid));
    if (wrote == 0) return ERIAK_WRITE;
    if (len > 0) {
        wrote = (write_cb)(write_cb_data, (void*)msgbuf, len);
        if (wrote == 0) return ERIAK_WRITE;
    }
#ifdef _RIAK_DEBUG
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_log_debug(cxn, "Wrote %d bytes", (int)len);
    char buffer[10240];
    riak_size_t buflen = sizeof(buffer);
    char *pos = buffer;
    wrote = 0;
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
    riak_log_debug(cxn, "%s", buffer);
#endif
    return ERIAK_OK;
}
