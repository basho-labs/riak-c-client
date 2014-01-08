/*********************************************************************
 *
 * test_put.h:  Riak C Unit testing for Put Message
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
test_put_options_vclock();

void
test_put_options_w();

void
test_put_options_dw();

void
test_put_options_return_body();

void
test_put_options_pw();

void
test_put_options_if_not_modified();

void
test_put_options_if_none_match();

void
test_put_options_return_head();

void
test_put_options_timeout();

void
test_put_options_asis();

void
test_put_options_sloppy_quorum();

void
test_put_options_n_val();

void
test_put_decode_response();

void
test_put_decode_response();
