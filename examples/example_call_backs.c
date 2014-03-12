/*********************************************************************
 *
 * example_call_backs.c: Riak C Example Async Callbacks
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
#include "riak_messages.h"
#include "riak_connection.h"
#include "example_call_backs.h"

void
example_error_cb(void *resp,
                 void *ptr) {
    riak_operation *rop = (riak_operation*)ptr;
    char message[1024];
    riak_server_error *error = riak_operation_get_server_error(rop);
    if (error) {
        riak_binary_print(riak_server_error_get_errmsg(error), message, sizeof(message));
        fprintf(stderr, "ERROR: %s\n", message);
    }
    exit(1);
}

void
example_ping_cb(riak_ping_response *response,
                void               *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_ping_cb");
    riak_log_debug(cxn, "%s", "PONG");
    riak_ping_response_free(cfg, &response);
}

void
example_serverinfo_cb(riak_serverinfo_response *response,
                      void                     *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_serverinfo_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_serverinfo_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_serverinfo_response_free(cfg, &response);
}

void
example_listbucket_cb(riak_listbuckets_response *response,
                      void                      *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_listbucket_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_listbuckets_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_listbuckets_response_free(cfg, &response);
}

void
example_listkey_cb(riak_listkeys_response *response,
                   void                   *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "listkey_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_listkeys_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_listkeys_response_free(cfg, (riak_listkeys_response**)&(response));
}

void
example_get_cb(riak_get_response *response,
               void              *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_get_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_get_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_get_response_free(cfg, &response);
}

void
example_put_cb(riak_put_response *response,
               void              *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_put_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_put_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_put_response_free(cfg, &response);
}

void
example_delete_cb(riak_delete_response *response,
                  void                 *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_delete_cb");
    riak_delete_response_free(cfg, &response);
}

void
example_getclientid_cb(riak_get_clientid_response *response,
                       void                       *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_getclientid_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_get_clientid_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_get_clientid_response_free(cfg, &response);
}

void
example_setclientid_cb(riak_set_clientid_response *response,
                       void                       *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_setclientid_cb");
    riak_set_clientid_response_free(cfg, &response);
}

void
example_getbucketprops_cb(riak_get_bucketprops_response *response,
                          void                          *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_getbucketprops_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_get_bucketprops_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_get_bucketprops_response_free(cfg, &response);
}

void
example_resetbucketprops_cb(riak_reset_bucketprops_response *response,
                            void                            *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_resetbucketprops_cb");
    riak_reset_bucketprops_response_free(cfg, &response);
}

void
example_setbucketprops_cb(riak_set_bucketprops_response *response,
                          void                          *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_setbucketprops_cb");
    riak_set_bucketprops_response_free(cfg, &response);
}

void
example_mapreduce_cb(riak_mapreduce_response *response,
                     void                    *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_mapreduce_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_mapreduce_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_mapreduce_response_free(cfg, &response);
}

void
example_2i_cb(riak_2i_response *response,
                  void                 *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_2i_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_2i_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_2i_response_free(cfg, &response);
}

void
example_search_cb(riak_search_response *response,
                  void                 *ptr) {
    riak_operation  *rop = (riak_operation*)ptr;
    riak_connection *cxn = riak_operation_get_connection(rop);
    riak_config     *cfg = riak_connection_get_config(cxn);
    riak_log_debug(cxn, "%s", "example_search_cb");
    char output[10240];
    riak_print_state print_state;
    riak_print_init(&print_state, output, sizeof(output));
    riak_search_response_print(&print_state, response);

    riak_log_debug(cxn, "%s", output);
    riak_search_response_free(cfg, &response);
}
