/*********************************************************************
 *
 * riak_connection.c: Management of the Riak event
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
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_connection.h"
#include "riak_config-internal.h"
#include "riak_connection-internal.h"
#include "riak_network.h"

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

riak_error
riak_connection_new(riak_config       *cfg,
                    riak_connection  **cxn_target,
                    const char        *hostname,
                    const char        *portnum,
                    riak_addr_resolver resolver) {

    riak_connection *cxn = (riak_connection*)(cfg->malloc_fn)(sizeof(riak_connection));
    if (cxn == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not allocate a riak_connection");
        return ERIAK_OUT_OF_MEMORY;
    }
    cxn->is_secure = RIAK_FALSE;
    memset((void*)cxn, '\0', sizeof(riak_connection));
    *cxn_target = cxn;
    cxn->config = cfg;

    if (resolver == NULL) {
        resolver = getaddrinfo;
    }

    riak_strlcpy(cxn->hostname, hostname, sizeof(cxn->hostname));
    riak_strlcpy(cxn->portnum, portnum, sizeof(cxn->portnum));

    riak_error err = riak_resolve_address(cfg, resolver, hostname, portnum, &(cxn->addrinfo));
    if (err) {
        return ERIAK_DNS_RESOLUTION;
    }

    // TODO: Implement retry logic
    cxn->fd = riak_just_open_a_socket(cfg, cxn->addrinfo);
    if (cxn->fd < 0) {
        riak_log_critical_config(cfg, "%s", "Could not just open a socket");
        return ERIAK_CONNECT;
    }

    return ERIAK_OK;
}


// send a RpbStartTLS message to Riak without using protobuf-c
riak_error starttls(riak_config *cfg,
                    BIO *bio) {
    unsigned char b[1024];
    unsigned char messageCode = 255;
    int length = htonl(1);

    BIO_write(bio, &length, 4);
    BIO_write(bio, &messageCode, 1);

    int total = 0;
    for (;;) {
        int p = BIO_read(bio, b+total, 5);
        total += p;
        if (p <= 0) {
            riak_log_critical_config(cfg, "%s", "StartTLS data error");
            return ERIAK_TLS_ERROR;
        }
        if (total >= 5) {
            break;
        }
    }

    if (b[4] != 255) {
        riak_log_critical_config(cfg, "%s", "Invalid StartTLS response");
        return ERIAK_TLS_ERROR;
    }
  return ERIAK_OK;
}

// after starttls has been called,
// perform an SSL handshake and verify cacert
riak_error
riak_ssl_handshake(riak_config *cfg,
                   riak_connection *cxn,
                   riak_security_credentials *creds,
                   BIO *bio) {
    SSL* ssl;
    SSL_library_init();

    struct _riak_security_credentials *sc = creds;
    SSL_CTX* ctx = SSL_CTX_new(sc->ssl_method);

    if (NULL == ctx) {
        riak_log_critical_config(cfg, "%s", "SSL ctx is null");
        SSL_CTX_free(ctx);
        return ERIAK_TLS_ERROR;
    }

    if(sc->cacertfile == NULL) {
        riak_log_critical_config(cfg, "%s", "cacert file isn't specified");
        SSL_CTX_free(ctx);
        return ERIAK_TLS_ERROR;
    }
    if(!SSL_CTX_load_verify_locations(ctx, sc->cacertfile, NULL)) {
        riak_log_critical_config(cfg, "%s", "Can't load cacert file");
        SSL_CTX_free(ctx);
        return ERIAK_TLS_ERROR;
    }

    ssl = SSL_new(ctx);
    SSL_set_bio(ssl,bio,bio);
    SSL_set_connect_state(ssl);

    int handshake_response = SSL_do_handshake(ssl);
    if(handshake_response <= 0) {
        riak_log_critical_config(cfg, "%s:%s", "SSL handshake error", SSL_get_error(ssl, handshake_response));
        SSL_CTX_free(ctx);
        SSL_free(ssl);
        return ERIAK_TLS_ERROR;
    }

    // TODO
    if(!(SSL_get_peer_certificate != NULL && SSL_get_verify_result(ssl) == X509_V_OK)) {
        long l = SSL_get_verify_result(ssl);
        riak_log_critical_config(cfg,
                                 "%s:%s",
                                 "Certificate verification error",
                                 X509_verify_cert_error_string(l));
        SSL_CTX_free(ctx);
        SSL_free(ssl);
        return ERIAK_TLS_ERROR;
    }

    // store SSL info in the riak_connection
    // freed in riak_connection_free
    cxn->ssl_bio = bio;
    cxn->ssl = ssl;
    cxn->ssl_context = ctx;
    return ERIAK_OK;
}


// send a Riak auth request over an SSL connection
riak_error
riak_ssl_auth(riak_config *cfg,
              riak_connection *cxn,
              riak_security_credentials *creds)  {

    riak_binary *user_bin = riak_binary_copy_from_string(cfg, creds->username);
    riak_binary *pass_bin = riak_binary_copy_from_string(cfg, creds->password);
    riak_error result = riak_auth(cxn, user_bin, pass_bin);
    riak_binary_free(cfg, &user_bin);
    riak_binary_free(cfg, &pass_bin);
    if(result != ERIAK_OK) {
      return result;
    }
    return ERIAK_OK;
}

riak_error
riak_secure_connection_new(riak_config       *cfg,
                           riak_connection  **cxn_target,
                           const char        *hostname,
                           const char        *portnum,
                           riak_security_credentials *creds) {
    if(creds == NULL) {
      riak_log_critical_config(cfg, "%s", "SSL Configuration not specified");
      return ERIAK_TLS_ERROR;
    }

    riak_connection *cxn = (riak_connection*)(cfg->malloc_fn)(sizeof(riak_connection));
    if (cxn == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not allocate a riak_connection");
        return ERIAK_OUT_OF_MEMORY;
    }
    cxn->is_secure = RIAK_FALSE;
    memset((void*)cxn, '\0', sizeof(riak_connection));
    *cxn_target = cxn;
    cxn->config = cfg;

    riak_strlcpy(cxn->hostname, hostname, sizeof(cxn->hostname));
    riak_strlcpy(cxn->portnum, portnum, sizeof(cxn->portnum));

    int hostportlen = strlen(hostname) + strlen(portnum) + 1;
    char hp[hostportlen];
    if(sprintf(hp,"%s:%s", hostname, portnum) != hostportlen) {
      return ERIAK_OUT_OF_MEMORY;
    }

    BIO *bio;

    // TODO: have SSL use an existing fd as in riak_connection_new
    bio = BIO_new_connect(hp);

    if(NULL == bio) {
        riak_log_critical_config(cfg, "%s", "Failed to initialize OpenSSL BIO");
        return ERIAK_TLS_ERROR;
    }

    if (BIO_do_connect(bio) <= 0) {
        riak_log_critical_config(cfg, "%s", "OpenSSL BIO can't connect");
        ERR_print_errors_fp(stderr);
        BIO_free_all(bio);
        return ERIAK_TLS_ERROR;
    }

    riak_error starttls_err = starttls(cfg, bio);
    if(starttls_err != ERIAK_OK) {
      BIO_free_all(bio);
      riak_log_critical_config(cfg, "%s", "StartTLS handshake failed");
      return starttls_err;
    }

    riak_error handshake_result = riak_ssl_handshake(cfg, cxn, creds, bio);
    if(handshake_result != ERIAK_OK) {
      // ssl + ssl_ctx have already been freed as a result of riak_ssl_handshake
      BIO_free_all(bio);
      riak_log_critical_config(cfg, "%s", "SSL handshake failed");
      return handshake_result;
    }

    // set is_secure to true to let the auth request use the SSL bio
    cxn->is_secure = RIAK_TRUE;

    // handshake has succeeded, now try to auth against Riak
    riak_error auth_err = riak_ssl_auth(cfg, cxn, creds);
    if(auth_err != ERIAK_OK) {
      cxn->is_secure = RIAK_FALSE;
      BIO_free_all(bio);
      return auth_err;
    }
    return ERIAK_OK;
}

SSL*
riak_connection_get_ssl(riak_connection *cxn) {
    return cxn->ssl;
}

riak_boolean_t
riak_connection_is_secure(riak_connection *cxn) {
    return cxn->is_secure;
}

riak_socket_t
riak_connection_get_fd(riak_connection *cxn) {
    return cxn->fd;
}

riak_config*
riak_connection_get_config(riak_connection *cxn) {
    return cxn->config;
}

void riak_connection_free(riak_connection** cxn_target) {
    if (cxn_target == NULL || *cxn_target == NULL) return;
    riak_connection *cxn = *cxn_target;
    riak_config *cfg = riak_connection_get_config(cxn);

    if (cxn->fd) {
        close(cxn->fd);
    }

    if(cxn->ssl_context) {
      SSL_CTX_free(cxn->ssl_context);
    }

    if(cxn->ssl_bio) {
      // TODO
      // segfault freeing the BIO.. wtf?
      //BIO_free(cxn->ssl_bio);
    }

    if(cxn->ssl) {
      SSL_free(cxn->ssl);
    }


    if (cxn->addrinfo != NULL) freeaddrinfo(cxn->addrinfo);
    riak_free(cfg, cxn_target);
}


riak_error
riak_security_credentials_new(riak_config *cfg,
                              riak_security_credentials **creds_target,
                              char *username,
                              char *password,
                              char *cacertfile) {
    riak_security_credentials *creds= (riak_security_credentials*)(cfg->malloc_fn)(sizeof(riak_security_credentials));
    if (creds == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not allocate riak_security_credentials");
        return ERIAK_OUT_OF_MEMORY;
    }
    memset((void*)creds, '\0', sizeof(riak_security_credentials));
    *creds_target = creds;
    // TODO: should we allow this to be passed in with riak_security_creds?
    creds->ssl_method = SSLv23_client_method();
    riak_strlcpy(creds->username, username, sizeof(creds->username));
    riak_strlcpy(creds->password, password, sizeof(creds->password));
    riak_strlcpy(creds->cacertfile, cacertfile, sizeof(creds->cacertfile));
    return ERIAK_OK;
}

void riak_security_credentials_free(riak_config *cfg,
                                    riak_security_credentials **creds) {

    if(creds== NULL || *creds== NULL) return;
    riak_free(cfg, creds);
}

riak_binary*
riak_security_credentials_get_username(riak_config *cfg,
                                       riak_security_credentials *creds) {
    riak_binary *user_bin = riak_binary_copy_from_string(cfg, creds->username);
    return user_bin;
}

riak_binary*
riak_security_credentials_get_password(riak_config *cfg,
                                       riak_security_credentials *creds) {
    riak_binary *password_bin = riak_binary_copy_from_string(cfg, creds->password);
    return password_bin;
}

riak_binary*
riak_security_credentials_get_cacertfile(riak_config *cfg,
                                         riak_security_credentials *creds) {
    riak_binary *cacertfile_bin = riak_binary_copy_from_string(cfg, creds->cacertfile);
    return cacertfile_bin;
}

