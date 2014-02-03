/*********************************************************************
 *
 * riak_error.c: Riak C Error Handling
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

#define _RIAK_ERROR_DEFINE_MSGS
#include "riak.h"
#include "riak_error.h"
#include "riak_messages-internal.h"
#include "riak_config-internal.h"

struct _riak_server_error {
    riak_uint32_t errcode;
    riak_binary  *errmsg;
};

riak_error
riak_server_error_new(struct _riak_config  *cfg,
                      riak_server_error   **err,
                      riak_uint32_t         errcode,
                      struct _riak_binary  *errmsg) {
    riak_server_error *error = (riak_server_error*)(cfg->malloc_fn)(sizeof(riak_server_error));
    if (error == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    *err = error;
    error->errcode = errcode;
    error->errmsg = riak_binary_copy(cfg, errmsg);

    if (error->errmsg == NULL) {
        riak_free(cfg, err);
        return ERIAK_OUT_OF_MEMORY;
    }

    return ERIAK_OK;
}

void
riak_server_error_free(struct _riak_config  *cfg,
                       riak_server_error   **err) {
    if (err == NULL || *err == NULL) return;
    riak_binary_free(cfg, &((*err)->errmsg));
    riak_free(cfg, err);
}


riak_uint32_t
riak_server_error_get_errcode(riak_server_error *err) {
    return err->errcode;
}

riak_binary*
riak_server_error_get_errmsg(riak_server_error *err) {
    return err->errmsg;
}

const char* riak_strerror(riak_error err) {
    if (err >= ERIAK_OK && err < ERIAK_LAST_ERRORNUM) {
        return errmsgs[err];
    }
    return "<Unknown Error>";
}
