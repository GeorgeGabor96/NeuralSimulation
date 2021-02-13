#include "synapse_tests.h"
#include "Synapse.h"

#include <math.h>


TestStatus synapse_class_create_destroy_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass* synapse_class = NULL;
	float rev_potential = 0.0f;
	float tau_ms = -1.0f / (float)log(0.5);
	uint32_t delay = 1;
	SynapseType s_type = CONDUCTANCE_SYNAPSE;
	float simuation_time_ms = 1.0f;
	float tau_exp = 0.5f;

	// call normal case
	synapse_class = synapse_class_create(rev_potential, tau_ms, delay, s_type, simuation_time_ms);
	assert(synapse_class != NULL, "@synapse_class is NULL");
	assert(synapse_class->E == rev_potential, "@synapse_class->E should be %f not %f", rev_potential, synapse_class->E);
	assert(synapse_class->delay == delay, "@synapse_class->delay should be %u not %u", delay, synapse_class->delay);
	assert(synapse_class->type == s_type, "@synapse_class->type should be %d not %d", s_type, synapse_class->type);
	assert(float_test(synapse_class->tau_exp, tau_exp), "@synapse_class->tau_exp %f not close enough to %f", synapse_class->tau_exp, tau_exp);
	synapse_class_destroy(synapse_class);

	// call with @tau_ms <= 0.0
	synapse_class = synapse_class_create(rev_potential, 0.0f, delay, s_type, simuation_time_ms);
	assert(synapse_class == NULL, "@synapse_class should be NULL for @tau_ms = 0");

	// call with @simulation_step_ms <= 0
	synapse_class = synapse_class_create(rev_potential, tau_ms, delay, s_type, 0.0f);
	assert(synapse_class == NULL, "@synapse_class should be NULL for @simulation_step_ms == 0");

	// call with @type = 2
	synapse_class = synapse_class_create(rev_potential, tau_ms, delay, 2, simuation_time_ms);
	assert(synapse_class == NULL, "@synapse_class should be NULL for @type = 2");
	
	// destroy with NULL
	synapse_class_destroy(NULL);

	assert(memory_leak() == FALSE, "Memory leak");
	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus synapse_class_memory_test() {
	TestStatus status = TEST_FAILED;
	SynapseClass* synapse_classes[1000] = { NULL };
	uint32_t i = 0;

	for (i = 0; i < 1000; ++i) synapse_classes[i] = synapse_class_create_default();
	for (i = 0; i < 1000; ++i) synapse_class_destroy(synapse_classes[i]);

	assert(memory_leak() == FALSE, "Memory leak");
	status = TEST_SUCCESS;
error:
	return status;
}


void synapse_class_set_default_values(SynapseType type, SynapseClass* s_class) {
	s_class->E = 0.0f;
	s_class->tau_exp = 0.5f;
	s_class->delay = 1;
	s_class->type = type;
}


TestStatus synapse_create_destroy_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	synapse_class_set_default_values(CONDUCTANCE_SYNAPSE, &s_class);
	float w = 1.0f;
	Synapse* synapse = NULL;

	// calls
	synapse = synapse_create(&s_class, w);
	assert(synapse != NULL, "@synapse should not be NULL");
	assert(queue_is_valid(&(synapse->spike_times)) == TRUE, invalid_argument("synapse->spike_times"));
	assert(synapse->spike_times.array.max_length == SYNAPSE_INITIAL_SPIKE_CAPACITY, "@synapse->spike_times.array.max_length should be %u not %u", SYNAPSE_INITIAL_SPIKE_CAPACITY, synapse->spike_times.array.max_length);
	assert(synapse->spike_times.array.length == 0, "@synapse->spike_times.array.length should be %u not %u", 0, synapse->spike_times.array.length);
	assert(queue_is_empty(&(synapse->spike_times)), "@synapse->spike_times should be empty");
	assert(synapse->s_class == &s_class, "@synapse->s_class should be %p not %p", &s_class, synapse->s_class);
	assert(synapse->w == w, "@synapse->w should be %f not %f", w, synapse->w);
	assert(synapse->g == 0.0f, "@synapse->g should be 0");

	// corner cases
	assert(synapse_create(NULL, 1.0f) == NULL, "Should return NULL for invalid @s_class");
	synapse_destroy(NULL);

	synapse_destroy(synapse);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus synapse_memory_test() {
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	synapse_class_set_default_values(CONDUCTANCE_SYNAPSE, &s_class);
	Synapse* synapses[1000] = { NULL };
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t value = 0;

	for (i = 0; i < 1000; ++i) {
		synapses[i] = synapse_create(&s_class, 1.0f);
		for (j = 0; j < 10000; ++j) {
			synapse_add_spike_time(synapses[i], j);
		}
		assert(synapses[i]->spike_times.array.length == 10000, "length %u not %u for synapse %u", synapses[i]->spike_times.array.length, 10000, i);
	}
	for (i = 0; i < 1000; ++i) {
		for (j = 0; j < 10000; ++j) {
			value = *((uint32_t*)array_get(&(synapses[i]->spike_times.array), j)) - 1;
			assert(value == j, "value %u, not %u for synapse %u", value, j, i);
		}
		for (j = 1; j < 10001; ++j) {
			synapse_step(synapses[i], j);
		}
		assert(synapses[i]->spike_times.array.length == 0, "length %u not %u for synapse %u", synapses[i]->spike_times.array.length, 0, i);

		synapse_destroy(synapses[i]);
	}
	assert(memory_leak() == FALSE, "Memory leak");
	status = TEST_SUCCESS;

error:
	return status;
}



TestStatus synapse_add_spike_time_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	s_class.type = CONDUCTANCE_SYNAPSE;
	s_class.delay = 0u;
	float w = 1.0f;
	Synapse* synapse = synapse_create(&s_class, w);
	Queue* spike_times = NULL;
	uint32_t spike_time1 = 1;
	uint32_t spike_time2 = 5;
	uint32_t spike_time3 = 1;
	uint32_t spike_time;

	// normal calls
	assert(synapse_add_spike_time(synapse, spike_time1) == SUCCESS, "Should work");
	assert(synapse->spike_times.array.length == 1, "@synapse->spike_times.array.length should be 1");
	spike_time = *((uint32_t*)queue_head(&(synapse->spike_times)));
	assert(spike_time == spike_time1, "Head of @synapse->spike_times should be %u not %u", spike_time1, spike_time);

	assert(synapse_add_spike_time(synapse, spike_time2) == SUCCESS, "Should work");
	assert(synapse->spike_times.array.length == 2, "@synapse->spike_times.array.length should be 2");
	spike_time = *((uint32_t*)array_get_fast(&(synapse->spike_times.array), synapse->spike_times.tail - 1));
	assert(spike_time == spike_time2, "Tail of @synapse->spike_times should be %u", spike_time2);
	
	// add spike with older time stamp
	assert(synapse_add_spike_time(synapse, spike_time3) == FAIL, "Should fail for older spike");
	assert(synapse->spike_times.array.length == 2, "@synapse->spike_times.array.length should be 2");

	// call with @synapse == NULL
	assert(synapse_add_spike_time(NULL, 0) == FAIL, "Should fail for @synapse = NULL");

	// call with @s_calss == NULL
	synapse->s_class = NULL;
	assert(synapse_add_spike_time(synapse, 0) == FAIL, "Should fail for @synapse->s_class = NULL");
	synapse->s_class = &s_class;

	synapse_destroy(synapse);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus synapse_compute_PSC_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	s_class.type = CONDUCTANCE_SYNAPSE;
	s_class.E = 0.0f;
	float w = 1.5f;
	float u = 2.3f;
	float I = 0.0f;
	float g = 1.3f;
	float synapse_I = 0.0f;
	Synapse* synapse = synapse_create(&s_class, w);
	//Queue* spike_times = synapse->spike_times;
	// TEST CONDUCTANCE_SYNAPSE
	s_class.type = CONDUCTANCE_SYNAPSE;
	synapse->g = g;
	I = w * g;
	synapse_I = synapse_compute_PSC(synapse, u);
	assert(float_test(I, synapse_I), "Synapse PSC should be %f not %f", I, synapse_I);

	// TEST VOLTAGE_DEPENDENT_SYNAPSE
	s_class.type = VOLTAGE_DEPENDENT_SYNAPSE;
	I = w * g * (u - s_class.E);
	synapse_I = synapse_compute_PSC(synapse, u);
	assert(float_test(I, synapse_I), "Synapse PSC should be %f not %f", I, synapse_I);

	// corner cases
	synapse->s_class = NULL;

	// call for @synapse = NULL
	assert(synapse_compute_PSC(NULL, u) == 0.0f, "Should return 0.0 for @synapse = NULL");

	// call for @synapse->s_class = NULL
	assert(synapse_compute_PSC(synapse, u) == 0.0f, "Should return 0.0 for @synapse->s_class = NULL");
	synapse->s_class = &s_class;

	// call for undifined synapse type
	s_class.type = 5;
	assert(synapse_compute_PSC(synapse, u) == 0.0f, "Should return 0.0 for undifined @synapse->s_class->type");

	s_class.type = CONDUCTANCE_SYNAPSE;
	synapse_destroy(synapse);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus synapse_step_test() {
	// setup
	TestStatus status = TEST_FAILED;
	SynapseClass s_class;
	s_class.type = CONDUCTANCE_SYNAPSE;
	s_class.delay = 1u;
	s_class.tau_exp = 0.5f;

	Synapse* synapse = synapse_create(&s_class, 0.0f);
	//Queue* spike_times = synapse->spike_times;
	uint32_t spike_time1 = 1;
	uint32_t spike_time2 = 3;
	uint32_t simulation_time = 0;
	float g = 0.0f;

	// add spikes in synapse
	synapse_add_spike_time(synapse, spike_time1);
	synapse_add_spike_time(synapse, spike_time2);

	// start computing g for each time
	// time = 0, g = 0
	synapse_step(synapse, simulation_time++);
	g = 0.0f;
	assert(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 1, g = 0
	synapse_step(synapse, simulation_time++);
	assert(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 2, g = 1
	synapse_step(synapse, simulation_time++);
	g = 1.0f;
	assert(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 3, g = 0.5
	synapse_step(synapse, simulation_time++);
	g = 0.5f;
	assert(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 4, g = 1.5
	synapse_step(synapse, simulation_time++);
	g = 1.5f;
	assert(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 5, g = 0.75
	synapse_step(synapse, simulation_time++);
	g = 0.75f;
	assert(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 6, g = 0.375
	synapse_step(synapse, simulation_time++);
	g = 0.375;
	assert(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// time = 7, g = 0.1875
	synapse_step(synapse, simulation_time++);
	g = 0.1875;
	assert(float_test(synapse->g, g), "At time %u conductance should be %f not %f", simulation_time, g, synapse->g);

	// corner cases
	synapse->s_class = NULL;

	// call with @synapse = NULL
	assert(synapse_step(NULL, simulation_time) == FAIL, "Should fail for @synapse = NULL");

	// call with @synapse->s_class = NULL
	assert(synapse_step(synapse, simulation_time) == FAIL, "Should fail for @synapse->s_class = NULL");
	synapse->s_class = &s_class;

	synapse_destroy(synapse);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}