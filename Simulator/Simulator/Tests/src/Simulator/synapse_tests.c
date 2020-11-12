#include "synapse_tests.h"
#include "Synapse.h"

#include <math.h>


TestStatus synapse_class_create_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass* synapse_class = NULL;
	float rev_potential = 0.0f;
	float tau_ms = -1.0f / (float)log(0.5);
	uint32_t delay = 1;
	SynapseType s_type = CONDUCTANCE_SYNAPCE;
	float simuation_time_ms = 1.0f;
	float tau_exp = 0.5f;

	// call normal case
	synapse_class = synapse_class_create(rev_potential, tau_ms, delay, s_type, simuation_time_ms);
	check(synapse_class != NULL, "@synapse_class is NULL");
	check(synapse_class->E == rev_potential, "@synapse_class->E should be %f not %f", rev_potential, synapse_class->E);
	check(synapse_class->delay == delay, "@synapse_class->delay should be %u not %u", delay, synapse_class->delay);
	check(synapse_class->type == s_type, "@synapse_class->type should be %d not %d", s_type, synapse_class->type);
	check(float_test(synapse_class->tau_exp, tau_exp), "@synapse_class->tau_exp %f not close enough to %f", synapse_class->tau_exp, tau_exp);
	synapse_class_destroy(synapse_class);

	// call with @tau_ms <= 0.0
	synapse_class = synapse_class_create(rev_potential, 0.0f, delay, s_type, simuation_time_ms);
	check(synapse_class == NULL, "@synapse_class should be NULL for @tau_ms = 0");

	// call with @simulation_step_ms <= 0
	synapse_class = synapse_class_create(rev_potential, tau_ms, delay, s_type, 0.0f);
	check(synapse_class == NULL, "@synapse_class should be NULL for @simulation_step_ms == 0");

	// call with @type = 2
	synapse_class = synapse_class_create(rev_potential, tau_ms, delay, 2, simuation_time_ms);
	check(synapse_class == NULL, "@synapse_class should be NULL for @type = 2");

	status = TEST_SUCCESS;

	// cleanup
error:
	return status;
}


TestStatus synapse_class_destroy_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass* synapse_class = synapse_class_create(0.0f, 15.0f, 1, CONDUCTANCE_SYNAPCE, 1.0f);

	// call normal
	synapse_class_destroy(synapse_class);

	// call with NULL
	synapse_class_destroy(NULL);

	status = TEST_SUCCESS;

	// cleaup
	return status;
}


void synapse_class_set_default_values(SynapseType type, SynapseClass* s_class) {
	s_class->E = 0.0f;
	s_class->tau_exp = 0.5f;
	s_class->delay = 1;
	s_class->type = type;
}


TestStatus synapse_create_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	synapse_class_set_default_values(CONDUCTANCE_SYNAPCE, &s_class);
	float w = 1.0f;
	Synapse* synapse = NULL;

	// calls
	synapse = synapse_create(&s_class, w);
	check(synapse != NULL, "@synapse should not be NULL");
	check(synapse->spike_times != NULL, "@synapse->spike_times should not be NULL");
	check(synapse->spike_times->array.length == SYNAPSE_INITIAL_SPIKE_CAPACITY, "@synapse->spike_times->array.length should be %u not %u", SYNAPSE_INITIAL_SPIKE_CAPACITY, synapse->spike_times->array.length);
	check(queue_is_empty(synapse->spike_times), "@synapse->spike_times should be empty");
	check(synapse->s_class == &s_class, "@synapse->s_class should be %p not %p", &s_class, synapse->s_class);
	check(synapse->w == w, "@synapse->w should be %f not %f", w, synapse->w);
	check(synapse->g == 0.0f, "@synapse->g should be 0");
	synapse_destroy(synapse);

	// call with class NULL
	synapse = synapse_create(NULL, w);
	check(synapse == NULL, "@synapse should be NULL for @s_class = NULL");

	status = TEST_SUCCESS;

	// cleaup
error:
	return status;
}


TestStatus synapse_destroy_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	float w = 1.0f;
	Synapse* synapse = synapse_create(&s_class, w);
	Queue* spike_times = NULL;

	// call with @synapse = NULL
	synapse_destroy(NULL);
	
	// call with @synapse->s_class = NULL
	synapse->s_class = NULL;
	synapse_destroy(synapse);
	synapse->s_class = &s_class;

	// call with @synapse->spike_times = NULL
	spike_times = synapse->spike_times;
	synapse->spike_times = NULL;
	synapse_destroy(synapse);
	synapse->spike_times = spike_times;

	// calls
	synapse_destroy(synapse);

	status = TEST_SUCCESS;

	// cleanup
	return status;
}


TestStatus synapse_add_spike_time_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus synapse_compute_PSC_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus synapse_step_test() {
	return TEST_UNIMPLEMENTED;
}