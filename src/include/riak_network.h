/*********************************************************************
 *
 * riak_network.h: Riak C General Networking Utilities
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

#ifndef _RIAK_NETWORK_H
#define _RIAK_NETWORK_H

/**
 * @brief Turns ASCII host/port into addrinfo struct
 * @param cfg Riak Configuration
 * @param resolver Function to resolve IP addresses
 * @param host String name of host (or IP)
 * @param portnum Port number or service name
 * @param addrinfo Return value of populated addrinfo struct
 * @returns Error on DNS resolution issues
 */
riak_error
riak_resolve_address(riak_config      *cfg,
                     riak_addr_resolver resolver,
                     const char        *host,
                     const char        *portnum,
                     riak_addrinfo    **addrinfo);

/**
 * @brief Opens a socket and connects to a host
 * @param cfg Riak Configuration
 * @param addrinfo Address of machine to connect to
 * @returns A connect socket file descriptor
 */
riak_socket_t
riak_just_open_a_socket(riak_config  *cfg,
                        riak_addrinfo *addrinfo);

/**
 * @brief Prints a human-readable version of addrinfo
 * @param addrinfo Address to be printed
 * @param target Where to put the printed string
 * @param len Length of `target`
 * @param port Return value of port number (0 if undefined)
 */
void
riak_print_host(riak_addrinfo *addrinfo,
                char          *target,
                int            len,
                riak_uint16_t *port);

#endif // _RIAK_NETWORK_H
