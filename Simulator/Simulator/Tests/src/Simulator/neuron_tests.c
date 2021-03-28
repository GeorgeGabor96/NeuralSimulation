#include "neuron_tests.h"
#include "Neuron.h"


TestStatus neuron_class_general_use_case_test() {
	// setup
	TestStatus status = TEST_FAILED;
	NeuronType n_type = LIF_NEURON;
	float tau = LIF_C * LIF_R;
	float u_factor = 1.0f - 1.0f / (LIF_C * LIF_R);
	float i_factor = 1.0f / LIF_C;
	float free_factor = LIF_U_REST / (LIF_C * LIF_R);
	NeuronClass* n_class = NULL;
	const char* name = "TEST";

	// call normal
	n_class = neuron_class_create(name, n_type);
	assert(n_class != NULL, "Should be able to create a @NeuronClass");
	assert(n_class->type == n_type, "@n_class->type should be %d not %d", n_type, n_class->type);
	assert(strcmp(string_get_C_string(n_class->name), name) == 0, "@n_class->name should be %s not %s", name, string_get_C_string(n_class->name));

	// check LIF values
	assert(n_class->u_th == LIF_U_TH, "@n_class->u_th should be %f not %f", LIF_U_TH, n_class->u_th);
	assert(n_class->u_rest == LIF_U_REST, "@n_class->u_rest should be %f not %f", LIF_U_REST, n_class->u_rest);
	assert(float_test(n_class->tau, tau), "@n_class->tau should be %f not %f", tau, n_class->tau);
	assert(float_test(n_class->u_factor, u_factor), "@n_class->u_factor should be %f not %f", u_factor, n_class->u_factor);
	assert(float_test(n_class->i_factor, i_factor), "@n_class->i_factor should be %f not %f", i_factor, n_class->i_factor);
	assert(float_test(n_class->free_factor, free_factor), "@n_class->free_factor should be %f not %f", free_factor, n_class->free_factor);

	// corner cases
	assert(neuron_class_create(name, INVALID_NEURON) == NULL, "Should not work for invalid @type");
	assert(neuron_class_create(NULL, LIF_NEURON) == NULL, "Should not work for invalid @name");
	neuron_class_destroy(NULL);

	neuron_class_destroy(n_class);
	assert(memory_leak() == FALSE, "Memory leak");

	status = SUCCESS;

	// cleaup
error:
	if (n_class != NULL) {
		neuron_class_destroy(n_class);
	}
	return status;
}


TestStatus neuron_class_memory_test() {
	TestStatus status = TEST_FAILED;

	NeuronClass* neuron_classes[1000] = { NULL };
	uint32_t i = 0;
	for (i = 0; i < 1000; ++i) neuron_classes[i] = neuron_class_create("TEST", LIF_NEURON);
	for (i = 0; i < 1000; ++i) neuron_class_destroy(neuron_classes[i]);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus neuron_general_use_case_test() {
	// setup
	TestStatus status = TEST_FAILED;
	NeuronClass* n_class = neuron_class_create("TEST", LIF_NEURON);
	Neuron* neuron = NULL;
	
	/*--------create neuron-------- */
	neuron = neuron_create(n_class);
	assert(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
	assert(neuron->n_class == n_class, "@neuron->n_class should be %p not %p", n_class, neuron->n_class);
	assert(neuron->u == n_class->u_rest, "@neuron->u should be %f not %f", n_class->u_rest, neuron->u);
	
	/*--------add input synapses--------*/
	SynapseClass* s_class = synapse_class_create_default("TEST");
	Synapse* sy_in_1 = synapse_create(s_class, 1.1f);
	Synapse* sy_in_2 = synapse_create(s_class, 3.4f);
	Synapse* sy = NULL;
	Synapse sy1_copy;
	Synapse sy2_copy;
	memcpy(&sy1_copy, sy_in_1, sizeof(Synapse));
	memcpy(&sy2_copy, sy_in_2, sizeof(Synapse));
	
	assert(neuron_add_in_synapse(neuron, sy_in_1) == SUCCESS, "Cannot add input synapse @sy_in_1");
	assert(neuron_add_in_synapse(neuron, sy_in_2) == SUCCESS, "Cannot add input synapse @sy_in_2");
	assert(neuron->in_synapses_refs.length == 2, "@neuron->in_synapses.lenght should be 2 not %u", neuron->in_synapses_refs.length);
	sy = *((Synapse**)array_get(&(neuron->in_synapses_refs), 0));
	assert(memcmp(sy, &sy1_copy, sizeof(Synapse)) == 0, "@sy and @sy1_copy should have the same values");
	sy = *((Synapse**)array_get(&(neuron->in_synapses_refs), 1));
	assert(memcmp(sy, &sy2_copy, sizeof(Synapse)) == 0, "@sy and @sy2_copy should have the same values");

	/*--------add output synapse--------*/
	Synapse* sy_out_1 = synapse_create(s_class, 1.1f);
	Synapse* sy_out_2 = synapse_create(s_class, 3.4f);
	assert(neuron_add_out_synapse(neuron, sy_out_1) == SUCCESS, "Cannot add output synapse @sy_out_1");
	assert(neuron_add_out_synapse(neuron, sy_out_2) == SUCCESS, "Cannot add output synapse @sy_out_2");
	assert(neuron->out_synapses_refs.length == 2, "@neuron->in_synapses->lenght should be 2 not %u", neuron->in_synapses_refs.length);
	sy = *(Synapse**)array_get(&(neuron->out_synapses_refs), 0);
	assert(sy == sy_out_1, "@sy should be %p not %p", sy_out_1, sy);
	sy = *(Synapse**)array_get(&(neuron->out_synapses_refs), 1);
	assert(sy == sy_out_2, "@sy should be %p not %p", sy_out_2, sy);

	/*--------neuron step--------*/
	sy_in_1 = *((Synapse**)array_get(&(neuron->in_synapses_refs), 0));
	sy_in_2 = *((Synapse**)array_get(&(neuron->in_synapses_refs), 1));

	// case 1: no spyke due to low conductances
	sy_in_1->g = 1.0f;
	sy_in_2->g = 1.0f;
	neuron_step(neuron, 0u);
	assert(neuron->spike == FALSE, "Should not spyke");
	assert(neuron->u > neuron->n_class->u_rest, "Should have increased the voltage");
	assert(sy_in_1->g < 1.0f, "Should have lowered the input synapse 1 conductance");
	assert(sy_in_2->g < 1.0f, "Should have lowered the input synapse 2 conductance");
	assert(sy_out_1->spike_times.array.length == 0, "Should not have added spyke on the output synapse 1");
	assert(sy_out_2->spike_times.array.length == 0, "Should not have added spyke on the output synapse 2");

	// case 2: spyke because of the high conductances
	sy_in_1->g = 10.0f;
	sy_in_2->g = 10.0f;
	neuron_step(neuron, 1u);
	assert(neuron->spike == TRUE, "Should spyke");
	assert(neuron->u == neuron->n_class->u_rest, "Should have reseted the voltage");
	assert(sy_in_1->g < 10.0f, "Should have lowered the input synapse 1 conductance");
	assert(sy_in_2->g < 10.0f, "Should have lowered the input synapse 2 conductance");
	assert(sy_out_1->spike_times.array.length == 1, "Should have added spyke on the output synapse 1");
	assert(sy_out_2->spike_times.array.length == 1, "Should have added spyke on the output synapse 2");
	// remove and verify the spike times
	uint32_t time = *((uint32_t*)queue_dequeue(&(sy_out_1->spike_times))) - s_class->delay; // remove the delay, synapse keep the time they need to process the spyke
	assert(time == 1u, "The spyke time should be 1 not %u", time);
	time = *((uint32_t*)queue_dequeue(&(sy_out_2->spike_times))) - s_class->delay;
	assert(time == 1u, "The spyke time should be 1 not %u", time);

	// case 3: force a spike
	sy_in_1->g = 0.0f;
	sy_in_2->g = 0.0f;
	neuron_step_force_spike(neuron, 2u);
	assert(neuron->spike == TRUE, "Should spyke");
	assert(neuron->u == neuron->n_class->u_rest, "Should have reseted the voltage");
	assert(sy_out_1->spike_times.array.length == 1, "Should have added spyke on the output synapse 1");
	assert(sy_out_2->spike_times.array.length == 1, "Should have added spyke on the output synapse 2");

	// remove and verify the spike times
	time = *((uint32_t*)queue_dequeue(&(sy_out_1->spike_times))) - s_class->delay;
	assert(time == 2u, "The spyke time should be 2 not %u", time);
	time = *((uint32_t*)queue_dequeue(&(sy_out_2->spike_times))) - s_class->delay;
	assert(time == 2u, "The spyke time should be 2 not %u", time);

	// case 4: inject current -> no spyke
	neuron_step_inject_current(neuron, 3.0f, 3u);
	assert(neuron->spike == FALSE, "Should not spyke");
	assert(sy_out_1->spike_times.array.length == 0, "Shouldn't have added spyke on the output synapse 1");
	assert(sy_out_2->spike_times.array.length == 0, "Shouldn't have added spyke on tht output synapse 2");

	// case 5: inject current -> spyke
	neuron_step_inject_current(neuron, 100.0f, 4u);
	assert(neuron->spike == TRUE, "Should spyke");
	assert(neuron->u == neuron->n_class->u_rest, "Should have reseted the voltage");
	assert(sy_out_1->spike_times.array.length == 1, "Should have added spyke on the output synapse 1");
	assert(sy_out_2->spike_times.array.length == 1, "Should have added spyke on tht output synapse 2");
	// remove and verify the spike times
	time = *((uint32_t*)queue_dequeue(&(sy_out_1->spike_times))) - s_class->delay;
	assert(time == 4u, "The spyke time should be 4 not %u", time);
	time = *((uint32_t*)queue_dequeue(&(sy_out_2->spike_times))) - s_class->delay;
	assert(time == 4u, "The spyke time should be 4 not %u", time);

	// corner cases
	assert(neuron_create(NULL) == NULL, "Should fail for @neuron_class = NULL");

	assert(neuron_add_in_synapse(NULL, sy_in_1) == FAIL, "Should fail for invalid @neuron");
	assert(neuron_add_in_synapse(neuron, NULL) == FAIL, "Should fail for invalid @synapse");

	assert(neuron_add_out_synapse(NULL, sy_out_1) == FAIL, "Should fail for invalid @neuron");
	assert(neuron_add_out_synapse(neuron, NULL) == FAIL, "Should fail for invalid @synapse");

	assert(neuron_step(NULL, 0u), "Should fail for invalid @neuron");
	assert(neuron_step_force_spike(NULL, 10u) == FAIL, "Should fail for invalid @neuron");
	assert(neuron_step_inject_current(NULL, 1.0f, 10u) == FAIL, "Should fail for invalid @neuron");

	neuron_destroy(neuron);
	neuron_class_destroy(n_class);
	synapse_destroy(sy_out_1);
	synapse_destroy(sy_out_2);
	synapse_class_destroy(s_class);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus neuron_memory_test() {
	TestStatus status = TEST_FAILED;

	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	NeuronClass* n_class = neuron_class_create("TEST", LIF_NEURON);
	Neuron* neurons[100] = { NULL };
	SynapseClass* s_class = synapse_class_create_default("TEST");
	Synapse* synapse = NULL;

	clock_t start, end;
	double cpu_time_used;

	uint32_t neurons_cnt = sizeof(neurons) / sizeof(Neuron*);
	for (i = 0; i < neurons_cnt; ++i) neurons[i] = neuron_create(n_class);
	
	start = clock();

	for (i = 0; i < neurons_cnt; ++i) {
		uint32_t step_runs = 100;
		Synapse** out_synapses = (Synapse**)calloc(step_runs, sizeof(Synapse*), "test");

		for (j = 0; j < step_runs; ++j) {
			out_synapses[j] = synapse_create(s_class, 1.0f);
			assert(synapse_is_valid(out_synapses[j]), invalid_argument("out_synapses[j]"));
		}

		for (j = 0; j < step_runs; ++j) {
			synapse = synapse_create(s_class, 1.0f);
			neuron_add_in_synapse(neurons[i], synapse);
			neuron_add_out_synapse(neurons[i], out_synapses[j]);

			neuron_step_inject_current(neurons[i], 1000.0f, j);
			neuron_step(neurons[i], j);
			// check you have spykes on output synapses
			for (k = 0; k <= j; ++k) {
				assert(out_synapses[k]->spike_times.array.length == j + 1 - k, "Should have %u spikes in synapse out %u", j + 1 - k, k);
			}

			assert(neuron_is_valid(neurons[i]) == TRUE, invalid_argument("neurons[i]"));
		}
		
		for (j = 0; j < step_runs; ++j) {
			synapse_destroy(out_synapses[j]);
		}
		free(out_synapses);
		// the synapses are not longer valid, need avoid checking the valididty
		neurons[i]->out_synapses_refs.length = 0;
	}

	for (i = 0; i < neurons_cnt; ++i) {
		neuron_destroy(neurons[i]);
	}
	end = clock();
	cpu_time_used = ((double)((size_t)end - start)) / CLOCKS_PER_SEC;
	printf("%u %lf\n", i, cpu_time_used);
	neuron_class_destroy(n_class);
	synapse_class_destroy(s_class);
	
	assert(memory_leak() == FALSE, "Memory leak");
	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus neuron_LIF_refractor_general_test() {
	TestStatus status = TEST_FAILED;

	NeuronClass* n_class = neuron_class_create("LIF_REFRACTOR_TEST", LIF_REFRACTORY_NEURON);
	check(neuron_class_is_valid(n_class) == TRUE, invalid_argument("n_class"));
	check(n_class->type == LIF_REFRACTORY_NEURON, "@n_class->type should be %u, not %u", LIF_REFRACTORY_NEURON, n_class->type);
	Neuron* neuron = neuron_create(n_class);
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	// force high current in the neuron to get a spike
	uint32_t start_time = 10;
	neuron_step_inject_current(neuron, 100.0f, start_time);
	check(neuron->spike == TRUE, "@neuron->spike should be TRUE");
	check(neuron->last_spike_time == start_time, "@neuron->last_spike_time should be %u, not %u", start_time, neuron->last_spike_time);
	check(neuron->u == neuron->n_class->u_rest, invalid_argument("neuron->u"));

	// inject high current for time == refractory time and should not produce any spikes
	for (uint32_t i = 1; i <= neuron->n_class->refractory_time; ++i) {
		neuron_step_inject_current(neuron, 100.0f, start_time + i);
		check(neuron->spike == FALSE, "@neuron->spike should be FALSE");
		check(neuron->last_spike_time == start_time, "@neuron->last_spike_time should be %u, not %u", start_time, neuron->last_spike_time);
		check(neuron->u == neuron->n_class->u_rest, invalid_argument("neuron->u"));
	}

	// now we should get a spike
	uint32_t next_spike_time = neuron->n_class->refractory_time + 1;
	neuron_step_inject_current(neuron, 100.0f, next_spike_time);
	check(neuron->spike == TRUE, "@neuron->spike should be TRUE");
	check(neuron->last_spike_time == next_spike_time, "@neuron->last_spike_time should be %u, not %u", next_spike_time, neuron->last_spike_time);
	check(neuron->u == neuron->n_class->u_rest, invalid_argument("neuron->u"));

	neuron_destroy(neuron);
	neuron_class_destroy(n_class);

	assert(memory_leak() == FALSE, "Memory leak");
	status = TEST_SUCCESS;

ERROR
	return status;
}
