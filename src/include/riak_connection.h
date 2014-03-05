/*********************************************************************
 *
 * riak_connection.h: Management of the Riak event
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

#ifndef _RIAK_CONNECTION_H
#define _RIAK_CONNECTION_H
#include <openssl/ssl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _riak_connection riak_connection;
typedef struct _riak_security_credentials riak_security_credentials;


/**
 * @brief Construct a Riak connection
 * @param cfg Riak config for memory allocation
 * @param cxn Riak Connection (out)
 * @param hostname Name of Riak server
 * @param portnum Riak PBC port number
 * @param resolver IP Address resolving function (NULL for default)
 * @returns Error code
 */
riak_error
riak_connection_new(riak_config       *cfg,
                    riak_connection  **cxn,
                    const char        *hostname,
                    const char        *portnum,
                    riak_addr_resolver resolver);


/**
 * @brief Construct a secure Riak connection
 * @param cfg Riak config for memory allocation
 * @param cxn Riak Connection (out)
 * @param hostname Name of Riak server
 * @param portnum Riak PBC port number
 * @param creds Riak Security Credentials
 * @returns Error code
 */
riak_error
riak_secure_connection_new(riak_config               *cfg,
                           riak_connection          **cxn,
                           const char                *hostname,
                           const char                *portnum,
                           riak_addr_resolver        resolver,
                           riak_security_credentials *creds
                           );


/**
 * @brief Cleanup memory used by a Riak Connection
 * @param re Riak Connection
 */
void
riak_connection_free(riak_connection** re);


/**
 * @brief return the file descriptor in use by the Riak Connection
 * @param cxn Riak Connection
 * @returns active socket
 */
riak_socket_t
riak_connection_get_fd(riak_connection *cxn);

/**
 * @brief return the riak_config in use by the Riak Connection
 * @param cxn Riak Connection
 * @returns active riak_config
 */
riak_config*
riak_connection_get_config(riak_connection *cxn);


/**
 * @brief return the SSL object in use by the Riak Connection
 * @param cxn Riak Connection
 * @returns active SSL object
 */
SSL*
riak_connection_get_ssl(riak_connection *cxn);

/**
 * @brief has a secure connection been established?
 * @param cxn Riak Connection
 * @returns set to RIAK_TRUE after a successful SSL/TLS handshake
 */
riak_boolean_t
riak_connection_is_secure(riak_connection *cxn);


/**
 * @brief Create new Riak security credentials. Once created, the struct is immutable.
 * @param cfg Riak Config
 * @param creds_target the riak_security_credentials struct to allocate
 * @param username Riak username
 * @param password Riak password
 * @param cacertfile full path to a certificate authority file
 * @returns a riak_error upon failure
 */
riak_error
riak_security_credentials_new(riak_config *cfg,
                              riak_security_credentials **creds_target,
                              char *username,
                              char *password,
                              char *cacertfile);

/**
 * @brief Free an allocated riak_security_credentials struct
 * @param cfg Riak Config
 * @param creds the riak_security_credentials to free
 */
void riak_security_credentials_free(riak_config *cfg,
                                    riak_security_credentials **creds);



/**
 * @brief returns a copy of the username specified in the supplied credentials
 * @param creds Riak Security Credentials
 * @returns a riak_binary with specified username
 */
riak_binary*
riak_security_credentials_get_username(riak_config *cfg,
                                       riak_security_credentials *creds);

/**
 * @brief returns a copy of the password specified in the supplied credentials
 * @param cxn Riak Security Credentials
 * @returns set to RIAK_TRUE after a successful SSL/TLS handshake
 */
riak_binary*
riak_security_credentials_get_password(riak_config *cfg,
                                       riak_security_credentials *creds);

/**
 * @brief returns a copy of the cacertfile path in the supplied credentials
 * @param cxn Riak Security Credentials
 * @returns set to RIAK_TRUE after a successful SSL/TLS handshake
 */
riak_binary*
riak_security_credentials_get_cacertfile(riak_config *cfg,
                                         riak_security_credentials *creds);


#ifdef __cplusplus
}
#endif



#endif // _RIAK_CONNECTION_H
