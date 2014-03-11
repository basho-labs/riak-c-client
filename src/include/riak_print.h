/*********************************************************************
 *
 * riak_print.h: Riak C Client Printing Utilities
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

#ifndef _RIAK_PRINT_H
#define _RIAK_PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

struct _riak_print_state {
    char        *start;  // Beginning of output buffer
    char        *target; // Current write location in buffer
    riak_int32_t len;    // Maximum number of bytes left to write
    riak_int32_t total;  // Total number in original buffer
    riak_int32_t wrote;  // Total number of bytes written so far
};
typedef struct _riak_print_state riak_print_state;

/**
 * @brief Set up the Riak Printing State to allow safe string appending
 * @param state Returned initialized print state
 * @param location Underlying buffer
 * @param maxlen Size of underlying buffer
 */
void
riak_print_init(riak_print_state *state,
                char             *location,
                riak_int32_t      maxlen);

/**
 * @brief Safely print any old thing to a string
 * @param state Riak Print State
 * @param format printf-style format
 * @pararm ... Arguments for the format
 */
riak_int32_t
riak_print(riak_print_state *state,
           const char       *format,
           ...);

/**
 * @brief Access the number of bytes available in the current printing state
 * @param state Riak Print State
 * @returns Bytes available in current buffer
 */
riak_int32_t
riak_print_len(riak_print_state *state);

/**
 * @brief Print an integer value to a string
 * @param state Riak Printing State
 * @param name Printable name of field
 * @param value Value to print
 * @returns Number of bytes written in current call
 */
riak_int32_t
riak_print_label_int(riak_print_state *state,
                     char             *name,
                     riak_int32_t     value);

/**
 * @brief Print a floating point value to a string
 * @param state Riak Printing State
 * @param name Printable name of field
 * @param value Value to print
 * @returns Number of bytes written in current call
 */
riak_int32_t
riak_print_label_float(riak_print_state *state,
                       char             *name,
                       riak_float32_t    value);

/**
 * @brief Print a boolean value to a string
 * @param state Riak Printing State
 * @param name Printable name of field
 * @param value Value to print
 * @returns Number of bytes written in current call
 */
riak_int32_t
riak_print_label_bool(riak_print_state *state,
                      char           *name,
                      riak_boolean_t  value);

/**
 * @brief Print a Riak Binary value to a string
 * @param state Riak Printing State
 * @param name Printable name of field
 * @param value Value to print
 * @returns Number of bytes written in current call
 */
riak_int32_t
riak_print_label_binary(riak_print_state *state,
                        char             *name,
                        riak_binary      *value);

/**
 * @brief User-readable representation of a binary object
 * @param state Riak Print State
 * @param bin Riak Binary
 * @returns Number of bytes written
 */
riak_int32_t
riak_print_binary(riak_print_state *state,
                  riak_binary      *bin);

/**
 * @brief Print an Riak Binary as a hexadecimal to a string
 * @param state Riak Printing State
 * @param name Printable name of field
 * @param value Value to print
 * @returns Number of bytes written in current call
 */
riak_int32_t
riak_print_label_binary_hex(riak_print_state *state,
                            char             *name,
                            riak_binary      *value);

/**
 * @brief Hexadecimal representation of a binary object
 * @param state Riak Print State
 * @param bin Riak Binary
 * @returns Number of bytes written
 */
riak_int32_t
riak_print_binary_hex(riak_print_state *state,
                      riak_binary      *bin);

/**
 * @brief Print an chunk of raw bytes as hexadecimal to a string
 * @param state Riak Printing State
 * @param name Printable name of field
 * @param value Value to print
 * @returns Number of bytes written in current call
 */
riak_int32_t
riak_print_label_raw_hex_array(riak_print_state *state,
                               riak_uint8_t     *value,
                               riak_int32_t      value_len);
/**
 * @brief Print a timestamp to a string
 * @param state Riak Printing State
 * @param name Printable name of field
 * @param value Value to print
 * @returns Number of bytes written in current call
 */
riak_int32_t
riak_print_label_time(riak_print_state *state,
                      char             *name,
                      riak_int32_t      value);

/**
 * @brief Print a null-terminated C-string to a string
 * @param state Riak Printing State
 * @param name Printable name of field
 * @param value Value to print
 * @returns Number of bytes written in current call
 */
riak_int32_t
riak_print_label_string(riak_print_state *state,
                        char             *name,
                        char             *value);

#ifdef __cplusplus
}
#endif
#endif // _RIAK_PRINT_H
