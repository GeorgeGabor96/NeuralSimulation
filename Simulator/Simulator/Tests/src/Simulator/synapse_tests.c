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
	s_class.type = CONDUCTANCE_SYNAPCE;
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
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	s_class.type = CONDUCTANCE_SYNAPCE;
	s_class.delay = 0u;
	float w = 1.0f;
	Synapse* synapse = synapse_create(&s_class, w);
	Queue* spike_times = NULL;
	uint32_t spike_time1 = 1;
	uint32_t spike_time2 = 5;
	uint32_t spike_time3 = 1;
	uint32_t spike_time;

	// call with @synapse == NULL
	check(synapse_add_spike_time(NULL, 0) == FAIL, "Should fail for @synapse = NULL");

	// call with @s_calss == NULL
	synapse->s_class = NULL;
	check(synapse_add_spike_time(synapse, 0) == FAIL, "Should fail for @synapse->s_class = NULL");
	synapse->s_class = &s_class;

	// call with @synapse->spike_times == NULL
	spike_times = synapse->spike_times;
	synapse->spike_times = NULL;
	check(synapse_add_spike_time(synapse, 0) == FAIL, "Should fail for @synapse->spike_times = NULL");
	synapse->spike_times = spike_times;

	// normal calls
	check(synapse_add_spike_time(synapse, spike_time1) == SUCCESS, "Should work");
	check(synapse->spike_times->length == 1, "@synapse->spike->times should be 1");
	spike_time = *(uint32_t*)queue_head(synapse->spike_times);
	check(spike_time == spike_time1, "Head of @synapse->spike_times should be %u not %u", spike_time1, spike_time);

	check(synapse_add_spike_time(synapse, spike_time2) == SUCCESS, "Should work");
	check(synapse->spike_times->length == 2, "@synapse->spike->times should be 2");
	check(*(uint32_t*)array_get_fast(&(synapse->spike_times->array), synapse->spike_times->tail - 1) == spike_time2, "Tail of @synapse->spike_times should be %u", spike_time2);
	
	// add spike with older time stamp
	check(synapse_add_spike_time(synapse, spike_time3) == FAIL, "Should faile for older spike");
	check(synapse->spike_times->length == 2, "@synapse->spike->times should be 2");

	status = TEST_SUCCESS;

	// cleaup
error:
	if (synapse != NULL) {
		synapse_destroy(synapse);
	}
	return status;
}


TestStatus synapse_compute_PSC_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	s_class.type = CONDUCTANCE_SYNAPCE;
	s_class.E = 0.0f;
	float w = 1.5f;
	float u = 2.3f;
	float I = 0.0f;
	float g = 1.3f;
	float synapse_I = 0.0f;
	Synapse* synapse = synapse_create(&s_class, w);
	Queue* spike_times = synapse->spike_times;

	synapse->s_class = NULL;
	synapse->spike_times = NULL;
	synapse->g = 1.3f;

	// call for @synapse = NULL
	check(synapse_compute_PSC(NULL, u) == 0.0f, "Should return 0.0 for @synapse = NULL");

	// call for @synapse->s_class = NULL
	check(synapse_compute_PSC(synapse, u) == 0.0f, "Should return 0.0 for @synapse->s_class = NULL");
	synapse->s_class = &s_class;

	// call for @synapse->spike_times = NULL
	check(synapse_compute_PSC(synapse, u) == 0.0f, "Should return 0.0 for @synapse->spike_times = NULL");
	synapse->spike_times = spike_times;

	// call for undifined synapse type
	s_class.type = 5;
	check(synapse_compute_PSC(synapse, u) == 0.0f, "Should return 0.0 for undifined @synapse->s_class->type");

	// TEST CONDUCTANCE_SYNAPSE
	s_class.type = CONDUCTANCE_SYNAPCE;
	I = w * g;
	synapse_I = synapse_compute_PSC(synapse, u);
	check(float_test(I, synapse_I), "Synapse PSC should be %f not %f", I, synapse_I);

	// TEST VOLTAGE_DEPENDENT_SYNAPSE
	s_class.type = VOLTAGE_DEPENDENT_SYNAPSE;
	I = w * g * (u - s_class.E);
	synapse_I = synapse_compute_PSC(synapse, u);
	check(float_test(I, synapse_I), "Synapse PSC should be %f not %f", I, synapse_I);

	status = TEST_SUCCESS;

	// cleaup
error:
	if (synapse != NULL) {
		synapse_destroy(synapse);
	}
	return status;
}


TestStatus synapse_step_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	s_class.type = CONDUCTANCE_SYNAPCE;
	s_class.delay = 1u;
	s_class.tau_exp = 0.5f;

	Synapse* synapse = synapse_create(&s_class, 0.0f);
	Queue* spike_times = synapse->spike_times;
	uint32_t spike_time1 = 1;
	uint32_t spike_time2 = 3;
	uint32_t simulation_time = 0;
	float g = 0.0f;

	synapse->s_class = NULL;
	synapse->spike_times = NULL;

	// call with @synapse = NULL
	check(synapse_step(NULL, simulation_time) == FAIL, "Should fail for @synapse = NULL");
	
	// call with @synapse->s_class = NULL
	check(synapse_step(synapse, simulation_time) == FAIL, "Should fail for @synapse->s_class = NULL");
	synapse->s_class = &s_class;

	// call with @synapse->spike_times = NULL
	check(synapse_step(synapse, simulation_time) == FAIL, "Should fail for @synapse->spike_times = NULL");
	synapse->spike_times = spike_times;

	// add spikes in synapse
	synapse_add_spike_time(synapse, spike_time1);
	synapse_add_spike_time(synapse, spike_time2);

	// start computing g for each time
	// time = 0, g = 0
	synapse_step(synapse, simulation_time++);
	g = 0.0f;
	check(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 1, g = 0
	synapse_step(synapse, simulation_time++);
	check(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 2, g = 1
	synapse_step(synapse, simulation_time++);
	g = 1.0f;
	check(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 3, g = 0.5
	synapse_step(synapse, simulation_time++);
	g = 0.5f;
	check(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 4, g = 1.5
	synapse_step(synapse, simulation_time++);
	g = 1.5f;
	check(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 5, g = 0.75
	synapse_step(synapse, simulation_time++);
	g = 0.75f;
	check(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 6, g = 0.375
	synapse_step(synapse, simulation_time++);
	g = 0.375;
	check(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 7, g = 0.1875
	synapse_step(synapse, simulation_time++);
	g = 0.1875;
	check(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	status = TEST_SUCCESS;

	// cleanup
error:
	if (synapse != NULL) {
		synapse_destroy(synapse);
	}
	return status;
}