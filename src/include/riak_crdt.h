/*********************************************************************
 *
 * riak_crdt.h: Riak C Client Data Types
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

#ifndef _RIAK_CRDT_H
#define _RIAK_CRDT_H

#include <inttypes.h>
#include <sys/types.h>
#include "riak_types.h"
#include "riak_error.h"
#include "riak_log_config.h"
#include "riak_config.h"
#include "riak_log.h"
#include "riak_binary.h"

typedef enum {DT_COUNTER, DT_FLAG, DT_MAP, DT_REGISTER, DT_SET} riak_datatype_t;

typedef struct _DatatypeUpdate {
	riak_datatype_t 			   riak_dt;
	// hash/set
	struct _DatatypeUpdate 		  *adds;
	struct _DatatypeUpdate 		  *removes;
	struct _DatatypeUpdate 		  *updates; // hash only
	long 		   				   delta;    // counter // TODO: unsigned?
	riak_binary                   *value;   // register
	riak_boolean_t                   flag;     // flag
} DatatypeUpdate;

riak_boolean_t is_counter_update(DatatypeUpdate *d);
riak_boolean_t is_flag_update(DatatypeUpdate *d);
riak_boolean_t is_map_update(DatatypeUpdate *d);
riak_boolean_t is_register_update(DatatypeUpdate *d);
riak_boolean_t is_set_update(DatatypeUpdate *d);

/*
  CRDT Counter Update
*/
riak_error counter_set_delta(DatatypeUpdate *d, long l);
riak_error counter_get_delta(DatatypeUpdate *d, long *l);

/*
  CRDT Register Update
*/
riak_error register_set_value(DatatypeUpdate *d, riak_binary *new_value);
riak_error register_get_value(DatatypeUpdate *d, riak_binary **current_value);

/*
  CRDT Flag Update
*/
riak_error flag_set_value(DatatypeUpdate *d, riak_boolean_t f);
riak_error flag_is_enabled(DatatypeUpdate *d, riak_boolean_t *f);

/*
  CRDT Set Updates
*/
riak_error set_add(DatatypeUpdate *d, riak_binary *v);
riak_error set_remove(DatatypeUpdate *d, riak_binary *v);
// returns HEAD
DatatypeUpdate* set_get_adds(DatatypeUpdate *d);
// returns HEAD
DatatypeUpdate* set_get_removes(DatatypeUpdate *d);


/*
  CRDT Map Updates
*/
// returns HEAD
DatatypeUpdate* map_get_adds(DatatypeUpdate *d);
// returns HEAD
DatatypeUpdate* map_get_removes(DatatypeUpdate *d);
// returns HEAD
DatatypeUpdate* map_get_updates(DatatypeUpdate *d);

riak_error map_add_counter(DatatypeUpdate *d, riak_binary *key, DatatypeUpdate *counter);
riak_error map_add_register(DatatypeUpdate *d, riak_binary *key, DatatypeUpdate *reg);
riak_error map_add_flag(DatatypeUpdate *d, riak_binary *key, DatatypeUpdate *flag);
riak_error map_add_set(DatatypeUpdate *d, riak_binary *key, DatatypeUpdate *set);
riak_error map_add_map(DatatypeUpdate *d, riak_binary *key, DatatypeUpdate *map);

#endif
