/*********************************************************************
 *
 * test_binary.h: Riak C Unit testing for riak_binary
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

void
test_build_binary();

void
test_build_binary_with_null();

void
test_build_binary_from_pb();

void
test_build_binary_to_pb();

void
test_binary_new_from_string();

void
test_binary_hex_print();

void
test_build_binary_from_existing();

void
test_riak_binary_from_stringl();

void
test_riak_binary_new_from_stringl();
