/*********************************************************************
 *
 * RiakCTestConnection.h: Riak Testing Connection
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

#include <event2/dns.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/thread.h>
#include "riak.h"
#include "RiakCTest.h"

#ifndef _RIAK_TEST_CONNECTION_H_
#define _RIAK_TEST_CONNECTION_H_

#define RIAK_TEST_HOST      "RIAK_TEST_HOST"
#define RIAK_TEST_PB_PORT   "RIAK_TEST_PB_PORT"

class RiakCTestConnection {
public:
    /**
     * @brief Create a DB connection for testing using defaults
     * @param state Testing state
     */
    RiakCTestConnection(RiakCTest& state);
    /**
     * @brief Create a DB connection for testing
     * @param state Testing state
     * @param hostname Name of the live host running Riak
     * @param portnum Port number for Protocol Buffers
     */
    RiakCTestConnection(RiakCTest& state,
                       const char*       hostname,
                       const char*       portnum);

    virtual ~RiakCTestConnection();

    /**
     * @brief Setup the a testing connection
     */
    RiakCTestConnection();

    /**
     * @brief Create a Riak Operation and Libevent Event
     * @returns Error Code
     */
    riak_error asyncCreateEvent();

    /**
     * @brief Send an asynchronous message
     * @returns Error Code
     */
    riak_error asyncSendMessage();

    /**
     * @brief Access the Riak Opration (for async)
     * @returns Riak Operation
     */
    riak_operation* getOperation() { return _rop; }

    /**
     * @brief Access the Riak Connection
     * @returns Riak Connection
     */
    riak_connection* getConnection() { return _cxn; }

    /**
     * @brief Access the Riak Configuration
     * @returns Riak Configuration
     */
    riak_config* getConfig() { return _state.getConfig(); }

protected:
    RiakCTest&   _state;
    riak_connection* _cxn;
    riak_operation*  _rop; // async-only
    riak_libevent*   _rev; // async-only
};

#endif /* _RIAK_TEST_CONNECTION_H_ */
