/*********************************************************************
 *
 * test_get.h:  Riak C Unit testing for Get Message
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
test_get_options_r();
void
test_get_options_pr();
void
test_get_options_basic_quorum();
void
test_get_options_notfound_ok();
void
test_get_options_if_modified();
void
test_get_options_head();
void
test_get_options_deletedvclock();
void
test_get_options_timeout();
void
test_get_options_sloppy_quorum();
void
test_get_options_n_val();
void
test_get_decode_response();
