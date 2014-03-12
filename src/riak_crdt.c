#include "riak_crdt.h"
#include "riak.h"

riak_boolean_t is_counter_update(DatatypeUpdate *d) {
	return d->riak_dt == DT_COUNTER;
}

riak_boolean_t is_flag_update(DatatypeUpdate *d) {
	return d->riak_dt == DT_FLAG;
}

riak_boolean_t is_map_update(DatatypeUpdate *d) {
	return d->riak_dt == DT_MAP;
}

riak_boolean_t is_register_update(DatatypeUpdate *d) {
	return d->riak_dt == DT_REGISTER;
}

riak_boolean_t is_set_update(DatatypeUpdate *d) {
	return d->riak_dt == DT_SET;
}

riak_error counter_set_delta(DatatypeUpdate *d, long l) {
	if(is_counter_update(d)) {
		d->delta = l;
		return ERIAK_OK;
	} else {
		return ERIAK_INVALID_DT;
	}
}

riak_error counter_get_delta(DatatypeUpdate *d, long *l) {
	if(is_counter_update(d)) {
		*l = d->delta;
		return ERIAK_OK;
	} else {
		return ERIAK_INVALID_DT;
	}
}
