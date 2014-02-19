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

#include <stdarg.h>

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
    void** new_array = (void**)(cfg->malloc_fn)(newnum*size);
    if (new_array == NULL) {
        return NULL;
    }
    // Just for good measure, clear out memory
    memset((void*)new_array, '\0', newnum*size);
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

/* append formatted string to `target' buffer to the limit of available space, but still
 *  return the number of characters which would have been printed if enough space was
 *  available.
 * This function ignore an output error and returns 0.
 *
 * Basically, if the sum of calls to riak_snprintf_cat() is >= than the initial buffer
 *  size, then it means that not enough space was available.
 *
 * This functions:
 * - move the `target' pointer forwards
 * - decrement the `left_to_write' integer
 */
size_t
riak_snprintf_cat(char **target,
		  riak_uint32_t *left_to_write,
		  const char *format,
		  ...)
{
  va_list ap;
  size_t nb_bytes;

  va_start(ap, format);
  nb_bytes = vsnprintf(*target, *left_to_write, format, ap);
  va_end(ap);

  if (nb_bytes < 0)
    return 0; /* bug: ignore error... */

  int nb_really_wrote = nb_bytes;
  if (nb_really_wrote >= *left_to_write)
    nb_really_wrote = *left_to_write - 1; /* it will always be 1 byte left for the final '\0' */

  *target += nb_really_wrote;
  *left_to_write -= nb_really_wrote;

  return nb_bytes;
}
