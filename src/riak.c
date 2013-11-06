/*********************************************************************
 *
 * riak.c: Riak Operations
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
#include "riak_call_backs.h"
#include "riak_event.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_event-internal.h"

static riak_error
riak_synchronous_request(riak_event **rev_target,
                         void       **response) {
    riak_event *rev = *rev_target;
    riak_event_set_response_cb(rev, (riak_response_callback)riak_sync_cb);
    riak_sync_wrapper wrapper = RIAK_INIT_SYNC_WRAPPER;
    wrapper.rev = rev_target;
    riak_event_set_cb_data(rev, &wrapper);

    riak_error err = riak_send_req(rev, rev->pb_request);
    if (err) {
        riak_log_fatal(rev, "Could not send request", NULL);
        return err;
    }

    // Terminates only on error or timeout
    riak_event_loop(rev->context);
    *response = wrapper.response;
    return ERIAK_OK;
}

riak_error
riak_ping(riak_context  *ctx) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    riak_ping_response *response = NULL;
    err = riak_encode_ping_request(rev, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)&response);
    if (err) {
        return err;
    }
    if (response->success != RIAK_TRUE) {
        return ERIAK_NO_PING;
    }
    return ERIAK_OK;
}

riak_error
riak_serverinfo(riak_context              *ctx,
                riak_serverinfo_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_serverinfo_request(rev, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}


riak_error
riak_get(riak_context      *ctx,
         riak_binary       *type,
         riak_binary       *bucket,
         riak_binary       *key,
         riak_get_options  *opts,
         riak_get_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_get_request(rev, type, bucket, key, opts, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_put(riak_context      *ctx,
         riak_object       *obj,
         riak_put_options  *opts,
         riak_put_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_put_request(rev, obj, opts, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_delete(riak_context      *ctx,
         riak_binary          *bucket,
         riak_binary          *key,
         riak_delete_options  *opts) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_delete_request(rev, bucket, key, opts, &(rev->pb_request));
    if (err) {
        return err;
    }
    riak_get_response *response = NULL;
    err = riak_synchronous_request(&rev, (void**)&response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_listbuckets(riak_context               *ctx,
                 riak_listbuckets_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_listbuckets_request(rev, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }
    return ERIAK_OK;
}

riak_error
riak_listkeys(riak_context            *ctx,
              riak_binary             *bucket_type,
              riak_binary             *bucket,
              riak_uint32_t            timeout,
              riak_listkeys_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_listkeys_request(rev, bucket_type, bucket, timeout, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }
    return ERIAK_OK;
}

riak_error
riak_get_clientid(riak_context                *ctx,
                  riak_get_clientid_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_get_clientid_request(rev, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_set_clientid(riak_context                *ctx,
                  riak_binary                 *clientid,
                  riak_set_clientid_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_set_clientid_request(rev, clientid, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_get_bucketprops(riak_context                   *ctx,
                     riak_binary                    *bucket,
                     riak_get_bucketprops_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_get_bucketprops_request(rev, bucket, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_reset_bucketprops(riak_context                     *ctx,
                       riak_binary                      *bucket,
                       riak_reset_bucketprops_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_reset_bucketprops_request(rev, bucket, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_set_bucketprops(riak_context                   *ctx,
                     riak_binary                    *bucket,
                     riak_bucket_props              *props,
                     riak_set_bucketprops_response **response) {
    riak_event *rev = NULL;
    riak_error err = riak_event_new(ctx, &rev, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_set_bucketprops_request(rev, bucket, props, &(rev->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&rev, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}
