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
		d->dt_data.counter_data->delta = l;
		return ERIAK_OK;
	} else {
		return ERIAK_INVALID_DT;
	}
}

riak_error counter_get_delta(DatatypeUpdate *d, long *l) {
	if(is_counter_update(d)) {
		*l = d->dt_data.counter_data->delta;
		return ERIAK_OK;
	} else {
		return ERIAK_INVALID_DT;
	}
}

DatatypeUpdate* new_counter_update() {
	DatatypeUpdate *dt = malloc(sizeof(DatatypeUpdate));
	dt->riak_dt = DT_COUNTER;
	dt->dt_data.counter_data = malloc(sizeof(DTCounterData));
	return dt;
}

void free_counter_update(DatatypeUpdate *d) {
	free(d->dt_data.counter_data);
	free(d);
}
