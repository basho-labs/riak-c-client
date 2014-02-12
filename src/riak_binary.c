/*********************************************************************
 *
 * riak_binary.c: Riak C Client Binary Object Utilities
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
#include "riak_binary.h"
#include "riak_binary-internal.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"

riak_binary*
riak_binary_new(riak_config  *cfg,
                riak_size_t   len,
                riak_uint8_t *data) {
    riak_binary *b = riak_config_allocate(cfg, sizeof(riak_binary));
    // In the degenerate case, force the length to be zero
    if (data == NULL) {
        len = 0;
    }
    if (b) {
        b->len     = len;
        b->data    = riak_config_allocate(cfg, len);
        b->managed = RIAK_TRUE;
        if (b->data) {
            memcpy((void*)b->data, (void*)data, len);
        } else {
            riak_free(cfg, &b);
        }
    }
    return b;
}

riak_binary*
riak_binary_new_shallow(riak_config  *cfg,
                        riak_size_t   len,
                        riak_uint8_t *data) {
    riak_binary *b = riak_config_allocate(cfg, sizeof(riak_binary));
    // In the degenerate case, force the length to be zero
    if (data == NULL) {
        len = 0;
    }
    if (b) {
        b->len     = len;
        b->data    = data;
        b->managed = RIAK_FALSE;
    }
    return b;
}

riak_binary*
riak_binary_copy(riak_config *cfg,
                 riak_binary *bin) {
    if (bin == NULL) {
        return NULL;
    }
    return riak_binary_new(cfg, bin->len, bin->data);
}

riak_binary*
riak_binary_copy_shallow(riak_config *cfg,
                         riak_binary *bin) {
    riak_size_t  len = bin->len;
    riak_binary *b   = riak_config_allocate(cfg, sizeof(riak_binary));
    // In the degenerate case, force the length to be zero
    if (bin->data == NULL) {
        len = 0;
    }
    if (b) {
        b->len     = len;
        b->data    = bin->data;
        b->managed = RIAK_FALSE;
    }
    return b;
}

riak_binary*
riak_binary_copy_from_string(riak_config *cfg,
                             const char  *from) {
    if (from == NULL) {
        return NULL;
    }
    return riak_binary_new(cfg, strlen(from), (riak_uint8_t*)from);
}

riak_binary*
riak_binary_copy_from_pb(riak_config         *cfg,
                         ProtobufCBinaryData *bin) {
    riak_size_t  len = bin->len;
    riak_binary *b   = riak_config_allocate(cfg, sizeof(riak_binary));
    // In the degenerate case, force the length to be zero
    if (bin->data == NULL) {
        len = 0;
    }
    if (b) {
        b->len     = len;
        b->data    = bin->data;
        b->managed = RIAK_FALSE;
    }
    return b;
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
riak_binary_free(riak_config  *cfg,
                 riak_binary **b) {
      if (b == NULL || *b == NULL) {
          return;
      }
      if ((*b)->managed) {
          riak_free(cfg, &((*b)->data));
      }
      riak_free(cfg, b);
}

void
riak_binary_copy_to_pb(ProtobufCBinaryData *to,
                       riak_binary         *from) {
    to->len  = from->len;
    to->data = from->data;
}

//TODO: Figure out clean way to print UTF-8 encoding
riak_size_t
riak_binary_print(riak_binary *bin,
                  char         *target,
                  riak_uint32_t len) {
    riak_size_t i = 0;
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

riak_size_t
riak_binary_hex_print(riak_binary  *bin,
                      char         *target,
                      riak_uint32_t len) {
    riak_size_t count = 0;
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

