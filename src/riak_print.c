/*********************************************************************
 *
 * riak_print.c: Riak C Client Printing Utilities
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

#include <time.h>
#include "riak.h"
#include "riak_binary-internal.h"
#include "riak_print.h"


void
riak_print_init(riak_print_state  *state,
                char              *location,
                riak_int32_t       maxlen) {
    memset(state, '\0', sizeof(riak_print_state));
    state->start  = location;
    state->target = location;
    state->len    = maxlen-1;  // Leave room for NULL
    state->total  = maxlen;
    state->wrote  = 0;
}

riak_int32_t
riak_print(riak_print_state *state,
           const char       *format,
           ...) {
    riak_int32_t wrote = 0;
    if (state->len > 0) {
        va_list va;
        va_start(va, format);
        wrote = vsnprintf(state->target, state->len, format, va);
        if (wrote > 0) {
            state->len    -= wrote;
            state->target += wrote;
            state->wrote  += wrote;
        }
        va_end(va);
    }

    return wrote;
}

riak_int32_t
riak_print_len(riak_print_state *state) {
    return state->len;
}

riak_int32_t
riak_print_label_int(riak_print_state *state,
                     char             *name,
                     riak_int32_t      value) {

    return riak_print(state, "%s: %d\n", name, value);
}

riak_int32_t
riak_print_label_float(riak_print_state *state,
                       char             *name,
                       riak_float32_t    value) {
    return riak_print(state, "%s: %f\n", name, value);
}

riak_int32_t
riak_print_label_bool(riak_print_state *state,
                      char             *name,
                      riak_boolean_t    value) {
    return riak_print(state, "%s: %s\n", name, (value ? "true" : "false"));
}


riak_int32_t
riak_print_label_binary(riak_print_state *state,
                        char             *name,
                        riak_binary      *value) {
    riak_int32_t wrote = riak_print(state, "%s: ", name);
    if (state->len > 0) {
        wrote += riak_print_binary(state, value);
    }
    wrote += riak_print(state, "%s", "\n");

    return wrote;
}

riak_int32_t
riak_print_binary(riak_print_state *state,
                  riak_binary      *bin) {
    riak_size_t wrote = riak_binary_print(bin, state->target, state->len);
    state->len    -= wrote;
    state->target += wrote;
    state->wrote  += wrote;

    return wrote;
}

riak_int32_t
riak_print_label_binary_hex(riak_print_state *state,
                            char             *name,
                            riak_binary      *value) {
    riak_int32_t wrote = riak_print(state, "%s: ", name);
    if (state->len > 0) {
        wrote += riak_print_binary_hex(state, value);
    }
    wrote += riak_print(state, "%s", "\n");

    return wrote;
}

riak_int32_t
riak_print_binary_hex(riak_print_state *state,
                      riak_binary      *bin) {
    riak_size_t wrote = riak_binary_hex_print(bin, state->target, state->len);
    state->len    -= wrote;
    state->target += wrote;
    state->wrote  += wrote;

    return wrote;
}

riak_int32_t
riak_print_label_raw_hex_array(riak_print_state *state,
                               riak_uint8_t     *value,
                               riak_int32_t      value_len) {
    riak_int32_t wrote = 0;
    wrote += riak_print(state, "%s", "0x");
    for (riak_int32_t bytes = 0; state->len > 0 && bytes < value_len; bytes++) {
        wrote += riak_print(state, "%02x", value[bytes]);
    }
    return wrote;
}

riak_int32_t
riak_print_label_string(riak_print_state *state,
                        char             *name,
                        char             *value) {
    return riak_print(state, "%s: %s\n", name, value);
}

riak_int32_t
riak_print_label_time(riak_print_state *state,
                      char             *name,
                      riak_int32_t      value) {
    char buffer[256];
    riak_int32_t wrote = 0;
    if (state->len > 0) {
        time_t mod = (time_t)value;
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&mod));
        wrote = riak_print_label_string(state, name, buffer);
    }

    return wrote;
}

