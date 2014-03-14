/*********************************************************************
 *
 * riak_array.c: Management of a Dynamic Array
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
#include "riak_array.h"
#include "riak_utils-internal.h"

riak_error
riak_array_new(riak_config *cfg,
               riak_array **ra_out,
               riak_size_t  size) {
    riak_array *ra = riak_config_clean_allocate(cfg, sizeof(riak_array));
    if (ra == NULL) {
        return ERIAK_OUT_OF_MEMORY;
    }
    ra->cfg      = cfg;
    ra->length   = 0;
    ra->capacity = size;
    ra->array    = riak_config_clean_allocate(cfg, sizeof(void*)*size);
    if (ra->array == NULL) {
        riak_free(cfg, &ra);
        return ERIAK_OUT_OF_MEMORY;
    }
    *ra_out = ra;
    return ERIAK_OK;
}

void
riak_array_free(riak_array **ra_in) {
    if (ra_in == NULL || *ra_in == NULL) {
        return;
    }
    riak_array  *ra  = *ra_in;
    riak_config *cfg = ra->cfg;
    riak_free(cfg, &(ra->array));
    riak_free(cfg, ra_in);
}

riak_size_t
riak_array_length(riak_array *ra) {
    return ra->length;
}

/**
 * @brief Ensure that the array is at least big enough for new additions
 * @param ra Riak Array
 * @param size New requested size of the array
 * @returns Error Code
 */
static riak_error
riak_array_resize(riak_array *ra,
                  riak_size_t size) {
    if (size > ra->capacity) {
        riak_size_t new_cap = ra->capacity;
        while (new_cap < size) {
            new_cap *= 2;
        }
        if (riak_array_realloc(ra->cfg, &(ra->array), sizeof(void*), ra->capacity, new_cap) == NULL) {
            riak_array_free(&ra);
            return ERIAK_OUT_OF_MEMORY;
        }
        ra->capacity = new_cap;
    }
    return ERIAK_OK;
}

riak_error
riak_array_insert(riak_array *ra,
                  riak_size_t pos,
                  void       *elem) {
    if ((riak_int64_t)pos > ra->length) {
        return ERIAK_OUT_OF_RANGE;
    }
    riak_error err = riak_array_resize(ra, ra->length + 1);
    if (err) {
        return err;
    }
    if (ra->length > 0 && (riak_int64_t)pos < ra->length) {
        for(int j = ra->length; j > pos; j--) {
            ra->array[j] = ra->array[j-1];
        }
    }
    ra->length++;
    ra->array[pos] = elem;

    return ERIAK_OK;
}

void*
riak_array_remove(riak_array *ra,
                  riak_size_t pos) {
    if ((riak_int64_t)pos > ra->length-1) {
        return NULL;
    }
    void *result = ra->array[pos];
    for(int j = pos; j < ra->length; j++) {
        ra->array[j] = ra->array[j+1];
    }
    ra->length--;
    return result;
}

riak_error
riak_array_push(riak_array *ra,
                void       *elem) {
    riak_error err = riak_array_resize(ra, ra->length + 1);
    if (err) {
        return err;
    }
    ra->array[ra->length] = elem;
    ra->length++;
    return ERIAK_OK;
}

void*
riak_array_pop(riak_array *ra) {
    if (ra->length > 0) {
        ra->length--;
        return ra->array[ra->length];
    } else {
        return NULL;
    }
}

void*
riak_array_get(riak_array *ra,
               riak_size_t pos) {
    if ((riak_int64_t)pos > ra->length-1) {
        return NULL;
    }
    return ra->array[pos];
}

riak_error
riak_array_set(riak_array *ra,
               riak_size_t pos,
               void       *elem) {
    if ((riak_int64_t)pos > (ra->length)-1) {
        return ERIAK_OUT_OF_RANGE;
    }
    ra->array[pos] = elem;

    return ERIAK_OK;
}
