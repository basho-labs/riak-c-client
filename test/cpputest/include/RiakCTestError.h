/*********************************************************************
 *
 * RiakCTestError.h: Dead Simple Error Message Container
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

#include <cstdlib>
#include <string>

#ifndef RIAKTESTINGERROR_H_
#define RIAKTESTINGERROR_H_

class RiakCTestError {
public:
    RiakCTestError() : _error_state(false), _lineno(0), _filename("") {}
    RiakCTestError(const char* message): _error_state(true),_message(message), _lineno(0), _filename("") {}
    virtual ~RiakCTestError() {}

    bool          hasError()   {return _error_state; }
    void          setMessage(const char* message) { _message = message; _error_state = true; }
    void          setMessage(const char* message, const char* file, riak_uint32_t lineno) { setMessage(message); _filename = file; _lineno = lineno; }
    const char*   getMessage() { return _message.c_str(); }
    const char*   getFile() { return _filename.c_str(); }
    riak_uint32_t getLine() { return _lineno; }

protected:
    bool          _error_state;
    std::string   _message;
    std::string   _filename;
    riak_uint32_t _lineno;
};

#endif /* RIAKTESTINGERROR_H_ */
