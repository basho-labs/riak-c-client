/*********************************************************************
 *
 * riak_binary.c: Riak C Client Binary Object Utilities
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
#include "riak_binary.h"
#include "riak_binary-internal.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_context-internal.h"

riak_binary*
riak_binary_new(riak_context *ctx,
                riak_size_t   len,
                riak_uint8_t *data) {
    riak_binary *b = (riak_binary*)(ctx->malloc_fn)(sizeof(riak_binary));
    if (b == NULL) return NULL;
    b->len  = len;
    b->data = data;

    return b;
}

riak_binary*
riak_binary_deep_new(riak_context *ctx,
                    riak_size_t    len,
                    riak_uint8_t  *data) {

    riak_binary *b = riak_binary_new(ctx, len, data);
    if (b) {
        b->data = (riak_uint8_t*)(ctx->malloc_fn)(len);
        memcpy((void*)b->data, (void*)data, len);
    }
    return b;
}


riak_binary*
riak_binary_populate(riak_context *ctx,
                     riak_binary  *b) {
    return riak_binary_new(ctx, b->len, b->data);
}

riak_binary*
riak_binary_populate_from_pb(riak_context        *ctx,
                             ProtobufCBinaryData *b) {
    return riak_binary_new(ctx, b->len, b->data);
}


riak_size_t
riak_binary_len(riak_binary *bin) {
    return bin->len;
}

riak_uint8_t*
riak_binary_data(riak_binary *bin) {
    return bin->data;
}

void
riak_binary_free(riak_context  *ctx,
                 riak_binary  **b) {
      if (b == NULL) {
          return;
      }
      riak_free(ctx, b);
}

void
riak_binary_deep_free(riak_context  *ctx,
                      riak_binary  **b) {
      if (b == NULL) {
          return;
      }
      riak_free(ctx, &((*b)->data));
      riak_free(ctx, b);
}

void
riak_binary_deep_free_pb(riak_context        *ctx,
                         ProtobufCBinaryData *b) {
      if (b == NULL) {
          return;
      }
      riak_free(ctx, &(b->data));
}

void
riak_binary_copy(riak_binary* to,
                 riak_binary* from) {
    to->len  = from->len;
    to->data = from->data;
}

riak_error
riak_binary_deep_copy(riak_context *ctx,
                      riak_binary  *to,
                      riak_binary  *from) {
    to->len  = from->len;
    to->data = (riak_uint8_t*)(ctx->malloc_fn)(from->len);
    if (to->data == NULL) return ERIAK_OUT_OF_MEMORY;
    memcpy((void*)to->data, (void*)from->data, from->len);
    return ERIAK_OK;
}

void
riak_binary_to_pb_copy(ProtobufCBinaryData *to,
                       riak_binary         *from) {
    to->len  = from->len;
    to->data = from->data;
}

riak_error
riak_binary_to_pb_deep_copy(riak_context        *ctx,
                            ProtobufCBinaryData *to,
                            riak_binary         *from) {
    to->len  = from->len;
    to->data = (riak_uint8_t*)(ctx->malloc_fn)(from->len);
    if (to->data == NULL) return ERIAK_OUT_OF_MEMORY;
    memcpy((void*)to->data, (void*)from->data, from->len);
    return ERIAK_OK;
}

void
riak_binary_from_pb_copy(riak_binary         *to,
                         ProtobufCBinaryData *from) {
    to->len  = from->len;
    to->data = from->data;
}

riak_error
riak_binary_from_pb_deep_copy(riak_context        *ctx,
                              riak_binary         *to,
                              ProtobufCBinaryData *from) {
    to->len  = from->len;
    to->data = (riak_uint8_t*)(ctx->malloc_fn)(from->len);
    if (to->data == NULL) return ERIAK_OUT_OF_MEMORY;
    memcpy((void*)to->data, (void*)from->data, from->len);
    return ERIAK_OK;
}

//TODO: Figure out clean way to print UTF-8 encoding
int
riak_binary_print(riak_binary *bin,
                  char         *target,
                  riak_uint32_t len) {
    int i = 0;
    for( ; (bin) && (i < (bin->len)) && i < (len-1); i++) {
        char c = '.';
        // Non-printable characters are replaced by a dot
        if (bin->data[i] >= 32) c = bin->data[i];
        target[i] = c;
    }
    if (len > 0) {
        target[i] = '\0';
    }
    return i;
}

int
riak_binary_hex_print(riak_binary  *bin,
                      char         *target,
                      riak_uint32_t len) {
    int count = 0;
    static char hex[] = "0123456789abcdef";
    if (bin != NULL) {
        for( ; count < bin->len && (count*2) < len-1; count++) {

            int nibble = (bin->data[count] & 0xf0) >> 4;
            target[count*2] = hex[nibble];
            nibble = (bin->data[count] & 0x0f);
            target[count*2+1] = hex[nibble];
        }
    }
    if (len > 0) target[count*2] = '\0';
    return count*2;
}

void
riak_binary_from_string(riak_binary *to,
                        const char  *from) {
    to->data = (riak_uint8_t*)from;
    to->len = strlen(from);
}

riak_error
riak_binary_from_string_deep_copy(riak_context *ctx,
                                  riak_binary  *to,
                                  const char   *from) {

    to->len = strlen(from);
    to->data = (riak_uint8_t*)(ctx->malloc_fn)(to->len);
    if (to->data == NULL) return ERIAK_OUT_OF_MEMORY;
    memcpy((void*)to->data, (void*)from, to->len);
    return ERIAK_OK;
}

riak_binary*
riak_binary_new_from_string(riak_context *ctx,
                            const char   *from) {
    riak_binary *b = riak_binary_new(ctx, 0, NULL);
    if (b != NULL) {
        riak_binary_from_string(b, from);
    }
    return b;
}

