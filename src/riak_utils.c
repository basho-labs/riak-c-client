/*********************************************************************
 *
 * utils.c: Riak C Client Utilities
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
#include "riak_binary-internal.h"
#include "riak_messages-internal.h"
#include "riak_utils-internal.h"
#include "riak_config-internal.h"
#include "riak_connection-internal.h"

size_t
riak_strlcpy(char       *dst,
             const char *src,
             size_t      size) {
    if (size <= 0) return 0;

    size_t len;
    for(len = 0; (len < size) && (*src != '\0'); len++) {
        *dst++ = *src++;
    }
    *dst = '\0';

    return len;
}

size_t
riak_strlcat(char       *dst,
             const char *src,
             size_t      size) {
    if (size <= 0) return 0;

    size_t len;
    // Walk to the end of the first string
    for(len = 0; (len < size) && (*dst != '\0'); len++, dst++) {}
    for(; (len < size) && (*src != '\0'); len++) {
        *dst++ = *src++;
    }
    *dst = '\0';

    return len;
}

void**
riak_array_realloc(riak_config *cfg,
                   void       ***from,
                   riak_size_t   size,
                   riak_uint32_t oldnum,
                   riak_uint32_t newnum) {
    void** new_array = (void**)riak_config_clean_allocate(cfg, newnum*size);
    if (new_array == NULL) {
        return NULL;
    }
    memcpy((void*)new_array, (void*)(*from), oldnum*size);
    riak_free(cfg, from);
    *from = new_array;
    return (*from);
}


void
riak_free_internal(riak_config *cfg,
                   void       **pp) {
    if(pp != NULL && *pp != NULL) {
        (cfg->free_fn)(*pp);
        *pp = NULL;
    }
}

