/*********************************************************************
 *
 * call_backs.h: Riak C Message Test Callbacks
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

#ifndef CALL_BACKS_H_
#define CALL_BACKS_H_

void
ping_cb(riak_ping_response *response,
        void *ptr);
void
serverinfo_cb(riak_serverinfo_response *response,
              void *ptr);
void
get_cb(riak_get_response *response,
       void *ptr);
void
put_cb(riak_put_response *response,
       void *ptr);
void
delete_cb(riak_delete_response *response,
          void *ptr);
void
listbucket_cb(riak_listbuckets_response *response,
             void *ptr);
void
listkey_cb(riak_listkeys_response *response,
           void *ptr);

void
getclientid_cb(riak_get_clientid_response *response,
               void *ptr);

void
setclientid_cb(riak_set_clientid_response *response,
               void *ptr);

void
getbucketprops_cb(riak_get_bucketprops_response *response,
                  void                          *ptr);

void
resetbucketprops_cb(riak_reset_bucketprops_response *response,
                    void                            *ptr);

void
setbucketprops_cb(riak_set_bucketprops_response *response,
                  void                          *ptr);

typedef struct _riak_sync_wrapper {
    riak_event   **rev;
    void          *response;
} riak_sync_wrapper;

void
riak_sync_cb(void *response,
             void *ptr);

#endif /* CALL_BACKS_H_ */
