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


riak_error
riak_ssl_handshake(riak_config *cfg,
                   riak_connection *cxn,
                   riak_security_credentials *creds,
                   BIO *bio) {
    SSL* ssl;
    SSL_library_init();

    struct _riak_security_credentials *sc = creds;
    SSL_CTX * ctx = SSL_CTX_new(SSLv23_client_method());

    if (NULL == ctx) {
      riak_log_critical_config(cfg, "%s", "SSL ctx is null");
        return ERIAK_TLS_ERROR;
    }

    // TODO: check cacert file pointer first
    if(!SSL_CTX_load_verify_locations(ctx, sc->cacertfile, NULL)) {
        riak_log_critical_config(cfg, "%s", "Can't load cacert file");
        return ERIAK_TLS_ERROR;
    }

    ssl = SSL_new(ctx);
    SSL_set_bio(ssl,bio,bio);
    SSL_set_connect_state(ssl);
    int handshake_response = SSL_do_handshake(ssl);
    if(handshake_response <= 0) {
      int ec = SSL_get_error(ssl, handshake_response);
      //TODO
      printf("Error: %s\n", ERR_reason_error_string(ec));
      //riak_log_critical_config(cfg, "%s", "Can't load cacert file");
      return ERIAK_TLS_ERROR;
    }


    if(!(SSL_get_peer_certificate != NULL && SSL_get_verify_result(ssl) == X509_V_OK)) {
        long l = SSL_get_verify_result(ssl);
        // TODO
        printf("Certificate verification error: %s\n", X509_verify_cert_error_string(l));
        //riak_log_critical_config(cfg, "%s", "Can't load cacert file");
        return ERIAK_TLS_ERROR;
    }

    // store SSL info in the riak_connection
    // freed in riak_connection_free
    cxn->ssl_bio = bio;
    cxn->ssl = ssl;
    cxn->ssl_context = ctx;
    return ERIAK_OK;
}

riak_error
riak_ssl_auth(riak_config *cfg,
              riak_connection *cxn,
              riak_security_credentials *creds)  {

    riak_binary *user_bin = riak_binary_copy_from_string(cfg, creds->username);
    riak_binary *pass_bin = riak_binary_copy_from_string(cfg, creds->password);
    riak_auth_response *unused = NULL;
    printf("Calling Riak AUTH\n");
    riak_error result = riak_auth(cxn, user_bin, pass_bin, unused);
    printf("AUTH RESULT = %d\n", result);
    riak_binary_free(cfg, &user_bin);
    riak_binary_free(cfg, &pass_bin);
    if(result != ERIAK_OK) {
      printf("AUTH FAILED\n");
      return ERIAK_TLS_ERROR;
    }
    printf("Finished Riak AUTH\n");
    return ERIAK_OK;
}

riak_error
riak_secure_connection_new(riak_config       *cfg,
                           riak_connection  **cxn_target,
                           const char        *hostname,
                           const char        *portnum,
                           riak_addr_resolver resolver,
                           riak_security_credentials *creds) {
    if(creds == NULL) {
      riak_log_critical_config(cfg, "%s", "SSL Configuration not specified");
      return ERIAK_TLS_ERROR;
    }

    // TODO: resolver is unused..

    riak_connection *cxn = (riak_connection*)(cfg->malloc_fn)(sizeof(riak_connection));
    if (cxn == NULL) {
        riak_log_critical_config(cfg, "%s", "Could not allocate a riak_connection");
        return ERIAK_OUT_OF_MEMORY;
    }
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
      riak_log_critical_config(cfg, "%s", "StartTLS handshake failed");
      return starttls_err;
    }

    riak_error handshake_result = riak_ssl_handshake(cfg, cxn, creds, bio);
    if(handshake_result != ERIAK_OK) {
      riak_log_critical_config(cfg, "%s", "SSL handshake failed");
      return handshake_result;
    }

    printf("STARTTLS successful, trying auth\n");
    riak_error auth_err = riak_ssl_auth(cfg, cxn, creds);
    if(auth_err != ERIAK_OK) {
      return auth_err;
    }

    return ERIAK_OK;
}

SSL*
riak_connection_get_ssl(riak_connection *cxn) {
    return cxn->ssl;
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

    if(cxn->ssl) {
      SSL_free(cxn->ssl);
    }

    if(cxn->ssl_bio) {
      BIO_free_all(cxn->ssl_bio);
    }

    if(cxn->ssl_context) {
      SSL_CTX_free(cxn->ssl_context);
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
    // TODO: macro?
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

