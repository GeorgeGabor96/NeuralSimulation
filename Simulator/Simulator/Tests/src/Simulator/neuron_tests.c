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

	// call normal
	n_class = neuron_class_create(n_type);
	assert(n_class != NULL, "Should be able to create a @NeuronClass");
	assert(n_class->type == n_type, "@n_class->type should be %d not %d", n_type, n_class->type);

	// check LIF values
	assert(n_class->u_th == LIF_U_TH, "@n_class->u_th should be %f not %f", LIF_U_TH, n_class->u_th);
	assert(n_class->u_rest == LIF_U_REST, "@n_class->u_rest should be %f not %f", LIF_U_REST, n_class->u_rest);
	assert(float_test(n_class->tau, tau), "@n_class->tau should be %f not %f", tau, n_class->tau);
	assert(float_test(n_class->u_factor, u_factor), "@n_class->u_factor should be %f not %f", u_factor, n_class->u_factor);
	assert(float_test(n_class->i_factor, i_factor), "@n_class->i_factor should be %f not %f", i_factor, n_class->i_factor);
	assert(float_test(n_class->free_factor, free_factor), "@n_class->free_factor should be %f not %f", free_factor, n_class->free_factor);

	// call with invalid @type
	assert(neuron_class_create(5) == NULL, "Should not work for invalid @type");
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
	for (i = 0; i < 1000; ++i) neuron_classes[i] = neuron_class_create(LIF_NEURON);
	for (i = 0; i < 1000; ++i) neuron_class_destroy(neuron_classes[i]);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus neuron_general_use_case_test() {
	// setup
	TestStatus status = TEST_FAILED;
	NeuronClass n_class;
	n_class.type = LIF_NEURON;
	neuron_class_set_LIF_parameters(&n_class, LIF_U_TH, LIF_U_REST, LIF_R, LIF_C);
	Neuron* neuron = NULL;
	
	/*--------create neuron-------- */
	neuron = neuron_create(&n_class);
	assert(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
	assert(neuron->n_class == &n_class, "@neuron->n_class should be %p not %p", &n_class, neuron->n_class);
	assert(neuron->u == n_class.u_rest, "@neuron->u should be %f not %f", n_class.u_rest, neuron->u);
	
	/*--------add input synapses--------*/
	SynapseClass* s_class = synapse_class_create_default();
	Synapse* sy_in_1 = synapse_create(s_class, 1.1f);
	Synapse* sy_in_2 = synapse_create(s_class, 3.4f);
	Synapse* sy = NULL;
	Synapse sy1_copy;
	Synapse sy2_copy;
	memcpy(&sy1_copy, sy_in_1, sizeof(Synapse));
	memcpy(&sy2_copy, sy_in_2, sizeof(Synapse));
	
	assert(neuron_add_in_synapse(neuron, sy_in_1, TRUE) == SUCCESS, "Cannot add input synapse @sy_in_1");
	assert(neuron_add_in_synapse(neuron, sy_in_2, TRUE) == SUCCESS, "Cannot add input synapse @sy_in_2");
	assert(neuron->in_synapses.length == 2, "@neuron->in_synapses.lenght should be 2 not %u", neuron->in_synapses.length);
	sy = (Synapse*)array_get(&(neuron->in_synapses), 0);
	assert(memcmp(sy, &sy1_copy, sizeof(Synapse)) == 0, "@sy and @sy1_copy should have the same values");
	sy = (Synapse*)array_get(&(neuron->in_synapses), 1);
	assert(memcmp(sy, &sy2_copy, sizeof(Synapse)) == 0, "@sy and @sy2_copy should have the same values");

	/*--------add output synapse--------*/
	Synapse* sy_out_1 = synapse_create(s_class, 1.1f);
	Synapse* sy_out_2 = synapse_create(s_class, 3.4f);
	assert(neuron_add_out_synapse(neuron, sy_out_1) == SUCCESS, "Cannot add output synapse @sy_out_1");
	assert(neuron_add_out_synapse(neuron, sy_out_2) == SUCCESS, "Cannot add output synapse @sy_out_2");
	assert(neuron->out_synapses_refs.length == 2, "@neuron->in_synapses->lenght should be 2 not %u", neuron->in_synapses.length);
	sy = *(Synapse**)array_get(&(neuron->out_synapses_refs), 0);
	assert(sy == sy_out_1, "@sy should be %p not %p", sy_out_1, sy);
	sy = *(Synapse**)array_get(&(neuron->out_synapses_refs), 1);
	assert(sy == sy_out_2, "@sy should be %p not %p", sy_out_2, sy);

	/*--------neuron step--------*/
	sy_in_1 = (Synapse*)array_get(&(neuron->in_synapses), 0);
	sy_in_2 = (Synapse*)array_get(&(neuron->in_synapses), 1);

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
	neuron_force_spike(neuron, 2u);
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
	neuron_inject_current(neuron, 3.0f, 3u);
	assert(neuron->spike == FALSE, "Should not spyke");
	assert(sy_out_1->spike_times.array.length == 0, "Shouldn't have added spyke on the output synapse 1");
	assert(sy_out_2->spike_times.array.length == 0, "Shouldn't have added spyke on tht output synapse 2");

	// case 5: inject current -> spyke
	neuron_inject_current(neuron, 100.0f, 4u);
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

	assert(neuron_add_in_synapse(NULL, sy_in_1, TRUE) == FAIL, "Should fail for invalid @neuron");
	assert(neuron_add_in_synapse(neuron, NULL, TRUE) == FAIL, "Should fail for invalid @synapse");

	assert(neuron_add_out_synapse(NULL, sy_out_1) == FAIL, "Should fail for invalid @neuron");
	assert(neuron_add_out_synapse(neuron, NULL) == FAIL, "Should fail for invalid @synapse");

	assert(neuron_step(NULL, 0u), "Should fail for invalid @neuron");
	assert(neuron_force_spike(NULL, 10u) == FAIL, "Should fail for invalid @neuron");
	assert(neuron_inject_current(NULL, 1.0f, 10u) == FAIL, "Should fail for invalid @neuron");

	neuron_destroy(neuron);
	synapse_class_destroy(s_class);
	synapse_destroy(sy_out_1);
	synapse_destroy(sy_out_2);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus neuron_memory_test() {
	TestStatus status = TEST_FAILED;

	uint32_t i = 0;
	uint32_t j = 0;
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	Neuron* neurons[100] = { NULL };
	SynapseClass* s_class = synapse_class_create_default();
	Synapse* synapse = NULL;

	for (i = 0; i < 100; ++i) {
		neurons[i] = neuron_create(n_class);
		for (j = 0; j < 100; ++j) {
			synapse = synapse_create(s_class, 1.0f);
			neuron_add_in_synapse(neurons[i], synapse, TRUE);
			neuron_step(neurons[i], j);
			check(neuron_is_valid(neurons[i]) == TRUE, invalid_argument("neurons[i]"));
		}
	}
	for (i = 0; i < 100; ++i) neuron_destroy(neurons[i]);

	neuron_class_destroy(n_class);
	synapse_class_destroy(s_class);
	
	assert(memory_leak() == FALSE, "Memory leak");
	status = TEST_SUCCESS;
error:
	return status;
}
