/*********************************************************************
 *
 * test_connection.h: Riak C Unit testing for riak_connection
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

void
test_connection_with_good_resolver();

void
test_connection_with_bad_resolver();

void
test_connection_pool_empty();

void
test_connection_pool_add_host();

void
test_connection_pool_remove_host();

void
test_connection_pool_lazy_connect();

void
test_connection_pool_eager_connect();

void
test_connection_pool_get_connection();

void
test_connection_pool_no_connections();

void
test_connection_pool_exceed_max_connections();

void
test_connection_pool_fair_distribution();

void
test_integration_connection_pool_single_host();

void
test_integration_connection_pool_many_hosts();

void
test_integration_connection_pool_max_connections();
