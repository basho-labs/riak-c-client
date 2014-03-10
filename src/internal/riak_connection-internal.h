/*********************************************************************
 *
 * riak_connection-internal.h: Management of the Riak Connection
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

#ifndef _RIAK_CONNECTION_INTERNAL_H
#define _RIAK_CONNECTION_INTERNAL_H

#define RIAK_HOST_MAX_LEN   256
#define RIAK_CRED_MAX_LEN   256

#include <openssl/ssl.h>

struct _riak_connection {
    riak_config      *config;
    char              hostname[RIAK_HOST_MAX_LEN];
    char              portnum[RIAK_HOST_MAX_LEN]; // Keep as a string for debugging
    riak_addrinfo    *addrinfo;
    riak_socket_t     fd;
    riak_boolean_t    is_secure;
    SSL              *ssl;
    BIO              *ssl_bio;
    SSL_CTX          *ssl_context;
};

struct _riak_security_credentials {
    char          username[RIAK_CRED_MAX_LEN];
    char          password[RIAK_CRED_MAX_LEN];
    char          cacertfile[RIAK_CRED_MAX_LEN];
    const SSL_METHOD    *ssl_method;
};

#endif // _RIAK_CONNECTION_INTERNAL_H
