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
#include "riak_connection.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_connection-internal.h"

static riak_error
riak_synchronous_request(riak_connection **cxn_target,
                         void            **response) {
    riak_connection *cxn = *cxn_target;
    riak_connection_set_response_cb(cxn, (riak_response_callback)riak_sync_cb);
    riak_sync_wrapper wrapper = RIAK_INIT_SYNC_WRAPPER;
    wrapper.cxn = cxn_target;
    riak_connection_set_cb_data(cxn, &wrapper);

    riak_error err = riak_send_req(cxn, cxn->pb_request);
    if (err) {
        riak_log_fatal(cxn, "Could not send request", NULL);
        return err;
    }

    // Terminates only on error or timeout
    riak_connection_loop(cxn->config);
    *response = wrapper.response;

    if (cxn->error) {
        return ERIAK_SERVER_ERROR;
    }

    return ERIAK_OK;
}

riak_error
riak_ping(riak_config  *cfg) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    riak_ping_response *response = NULL;
    err = riak_encode_ping_request(cxn, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)&response);
    if (err) {
        return err;
    }
    if (response->success != RIAK_TRUE) {
        return ERIAK_NO_PING;
    }
    return ERIAK_OK;
}

riak_error
riak_serverinfo(riak_config              *cfg,
                riak_serverinfo_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_serverinfo_request(cxn, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}


riak_error
riak_get(riak_config        *cfg,
         riak_binary        *bucket,
         riak_binary        *key,
         riak_get_options   *opts,
         riak_get_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_get_request(cxn, bucket, key, opts, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_put(riak_config        *cfg,
         riak_object        *obj,
         riak_put_options   *opts,
         riak_put_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_put_request(cxn, obj, opts, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_delete(riak_config         *cfg,
           riak_binary          *bucket,
           riak_binary          *key,
           riak_delete_options  *opts) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_delete_request(cxn, bucket, key, opts, &(cxn->pb_request));
    if (err) {
        return err;
    }
    riak_get_response *response = NULL;
    err = riak_synchronous_request(&cxn, (void**)&response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_listbuckets(riak_config                *cfg,
                 riak_listbuckets_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_listbuckets_request(cxn, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }
    return ERIAK_OK;
}

riak_error
riak_listkeys(riak_config             *cfg,
              riak_binary             *bucket,
              riak_uint32_t            timeout,
              riak_listkeys_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_listkeys_request(cxn, bucket, timeout, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }
    return ERIAK_OK;
}

riak_error
riak_get_clientid(riak_config                 *cfg,
                  riak_get_clientid_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_get_clientid_request(cxn, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_set_clientid(riak_config                 *cfg,
                  riak_binary                 *clientid,
                  riak_set_clientid_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_set_clientid_request(cxn, clientid, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_get_bucketprops(riak_config                    *cfg,
                     riak_binary                    *bucket,
                     riak_get_bucketprops_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_get_bucketprops_request(cxn, bucket, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_reset_bucketprops(riak_config                      *cfg,
                       riak_binary                      *bucket,
                       riak_reset_bucketprops_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_reset_bucketprops_request(cxn, bucket, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}

riak_error
riak_set_bucketprops(riak_config                    *cfg,
                     riak_binary                    *bucket,
                     riak_bucket_props              *props,
                     riak_set_bucketprops_response **response) {
    riak_connection *cxn = NULL;
    riak_error err = riak_connection_new(cfg, &cxn, NULL, NULL, NULL);
    if (err) {
        return err;
    }
    err = riak_encode_set_bucketprops_request(cxn, bucket, props, &(cxn->pb_request));
    if (err) {
        return err;
    }
    err = riak_synchronous_request(&cxn, (void**)response);
    if (err) {
        return err;
    }

    return ERIAK_OK;
}
