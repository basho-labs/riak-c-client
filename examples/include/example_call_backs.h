/*********************************************************************
 *
 * example_call_backs.h: Riak C Example Async Callbacks
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

#ifndef _RIAK_CALL_BACKS_H
#define _RIAK_CALL_BACKS_H

void
example_error_cb(void *resp,
                 void *ptr);

void
example_ping_cb(riak_ping_response *response,
                void               *ptr);
void
example_serverinfo_cb(riak_serverinfo_response *response,
                      void                     *ptr);
void
example_get_cb(riak_get_response *response,
               void              *ptr);
void
example_put_cb(riak_put_response *response,
               void              *ptr);
void
example_delete_cb(riak_delete_response *response,
                  void                 *ptr);
void
example_listbucket_cb(riak_listbuckets_response *response,
                      void                      *ptr);
void
example_listkey_cb(riak_listkeys_response *response,
                   void                   *ptr);

void
example_getclientid_cb(riak_get_clientid_response *response,
                      void                        *ptr);

void
example_setclientid_cb(riak_set_clientid_response *response,
                       void *ptr);

void
example_getbucketprops_cb(riak_get_bucketprops_response *response,
                          void                          *ptr);

void
example_resetbucketprops_cb(riak_reset_bucketprops_response *response,
                            void                            *ptr);

void
example_setbucketprops_cb(riak_set_bucketprops_response *response,
                          void                          *ptr);

void
example_mapreduce_cb(riak_mapreduce_response *response,
                     void                    *ptr);

void
example_2i_cb(riak_2i_response *response,
                  void                 *ptr);

void
example_search_cb(riak_search_response *response,
                  void                 *ptr);

#endif // _RIAK_CALL_BACKS_H
