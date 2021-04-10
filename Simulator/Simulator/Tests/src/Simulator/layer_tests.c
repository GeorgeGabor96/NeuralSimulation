#include "layer_tests.h"

#include "Layer.h"


TestStatus layer_general_use_case_test() {
	TestStatus status = TEST_FAILED;
	Status s = FAIL;
	uint32_t i = 0;
	Neuron* neuron = NULL;
	
	const char* l_name = "Layer test";
	NeuronClass* n_class = neuron_class_create("LIF_NEURON", LIF_NEURON);
	Layer* layer = layer_create(LAYER_FULLY_CONNECTED, 100, n_class, l_name);

	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(layer->neurons.length == 100, invalid_argument("layer->neurons->length"));
	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		assert(neuron_is_valid(neuron) == TRUE, "neuron %u is invalid", i);
	}
	assert(layer->link == layer_link_fc, invalid_argument("layer->link"));
	assert(string_equal_C_string(layer->name, l_name) == TRUE, "Should be equal");

	// test case 1: force spike on every neuron, make a step, and verify that no neuron has spiked
	ArrayBool* spikes = array_create(layer->neurons.length, layer->neurons.length, sizeof(BOOL));
	BOOL spike = FALSE;
	for (i = 0; i < spikes->length; ++i) {
		spike = TRUE;
		array_set(spikes, i, &spike);
	}
	s = layer_step_force_spikes(layer, spikes, 0);
	assert(s == SUCCESS, "Couldn't force spikes");
	array_destroy(spikes, NULL);

	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		assert(neuron->spike == TRUE, "neuron %u didn't spike", i);
	}

	s = layer_step(layer, 1);
	assert(s == SUCCESS, "Couldn't step layer");
	// neurons didn't have any input current -> imposible to spike
	spikes = layer_get_spikes(layer);
	assert(array_is_valid(spikes) == TRUE, invalid_argument("spikes"));
	assert(spikes->length == layer->neurons.length, "spikes->length is %u - layer->neurons.length is %u", spikes->length, layer->neurons.length);
	for (i = 0; i < spikes->length; ++i) {
		spike = *((BOOL*)array_get(spikes, i));
		assert(spike == FALSE, "neuron %u spiked", i);
	}
	array_destroy(spikes, NULL);

	// test case 2: inject currents not enough for spike, check no spike, make a step, check no spike
	ArrayFloat* currents = array_create(layer->neurons.length, layer->neurons.length, sizeof(float));
	float current = 0.0f;
	for (i = 0; i < currents->length; ++i) {
		current = 1.0f;
		array_set(currents, i, &current);
	}
	s = layer_step_inject_currents(layer, currents, 2);
	assert(s == SUCCESS, "Couldn't inject currents");
	array_destroy(currents, NULL);
	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		assert(neuron->spike == FALSE, "neuron %u did spike", i);
	}

	s = layer_step(layer, 3);
	assert(s == SUCCESS, "Couldn't step layer");
	spikes = layer_get_spikes(layer);
	assert(array_is_valid(spikes) == TRUE, invalid_argument("spikes"));
	assert(spikes->length == layer->neurons.length, "spikes->length is %u - layer->neurons.length is %u", spikes->length, layer->neurons.length);
	for (i = 0; i < spikes->length; ++i) {
		spike = *((BOOL*)array_get(spikes, i));
		assert(spike == FALSE, "neuron %u spiked", i);
	}
	array_destroy(spikes, NULL);

	// test case 3: inject currects enough for spike, check spike, make a step, check no spike
	currents = array_create(layer->neurons.length, layer->neurons.length, sizeof(float));
	current = 0.0f;
	for (i = 0; i < currents->length; ++i) {
		current = 100.0f;
		array_set(currents, i, &current);
	}
	s = layer_step_inject_currents(layer, currents, 4);
	assert(s == SUCCESS, "Couldn't inject currents");
	array_destroy(currents, NULL);
	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		assert(neuron->spike == TRUE, "neuron %u didn't spike", i);
	}

	s = layer_step(layer, 5);
	assert(s == SUCCESS, "Couldn't step layer");
	spikes = layer_get_spikes(layer);
	assert(array_is_valid(spikes) == TRUE, invalid_argument("spikes"));
	assert(spikes->length == layer->neurons.length, "spikes->length is %u - layer->neurons.length is %u", spikes->length, layer->neurons.length);
	for (i = 0; i < spikes->length; ++i) {
		spike = *((BOOL*)array_get(spikes, i));
		assert(spike == FALSE, "neuron %u spiked", i);
	}
	array_destroy(spikes, NULL);

	// test case 4: get voltages, don't care about values
	ArrayFloat* voltages = layer_get_voltages(layer);
	check(array_is_valid(voltages) == TRUE, invalid_argument("voltages"));
	array_destroy(voltages, NULL);

	// corner cases
	assert(layer_create(INVALID_NEURON, 100, n_class, l_name) == NULL, "Should return NULL for invalid @type");
	assert(layer_create(LIF_NEURON, 0, n_class, l_name) == NULL, "Should return NULL for invalid @n_neurons");
	assert(layer_create(LIF_NEURON, 100, NULL, l_name) == NULL, "Should return NULL for invalid @neuron_class");
	assert(layer_create(LIF_NEURON, 100, n_class, NULL) == NULL, "Should return NULL for invalid @name");

	assert(layer_step_force_spikes(NULL, spikes, 0u) == FAIL, "Should fail for invalid @layer");
	assert(layer_step_force_spikes(layer, NULL, 0u) == FAIL, "Should fail for invalid @spikes");

	assert(layer_step(NULL, 0u) == FAIL, "Should fail for invalid @layer");

	assert(layer_get_spikes(NULL) == NULL, "Should return NULL for invalid @layer");

	layer_destroy(NULL);

	layer_destroy(layer);
	neuron_class_destroy(n_class);
	assert(memory_leak() == FALSE, "memory_leak");

	status = TEST_SUCCESS;
error:

	return status;
}


TestStatus layer_memory_test_test() {
	TestStatus status = TEST_FAILED;
	uint32_t i = 0;
	uint32_t j = 0;
	const char* l_name = "Layer test";
	NeuronClass* n_class = neuron_class_create("LIF NEURON", LIF_NEURON);
	ArrayBool* spikes = NULL;
	ArrayFloat* voltages = NULL;
	ArrayFloat* currents = NULL;
	BOOL spike = FALSE;


	Layer* layers[100] = { NULL };
	for (i = 0; i < 100; ++i) {
		layers[i] = layer_create(LAYER_FULLY_CONNECTED, 100, n_class, l_name);
	}

	spikes = array_create(layers[0]->neurons.length, layers[0]->neurons.length, sizeof(BOOL));
	spike = TRUE;
	for (i = 0; i < 100; ++i) array_set(spikes, i, &spike);
	for (i = 0; i < 100; ++i) {
		for (j = 0; j < 100; ++j) {
			layer_step_force_spikes(layers[i], spikes, j);
			layer_step(layers[i], j);
			voltages = layer_get_voltages(layers[i]);
			array_destroy(voltages, NULL);
		}
	}
	array_destroy(spikes, NULL);

	currents = array_create(layers[0]->neurons.length, layers[0]->neurons.length, sizeof(float));
	for (i = 0; i < 100; ++i) {
		for (j = 0; j < 100; ++j) {
			layer_step_inject_currents(layers[i], currents, j);
			layer_step(layers[i], j);
			voltages = layer_get_voltages(layers[i]);
			array_destroy(voltages, NULL);
		}
	}
	array_destroy(currents, NULL);

	for (i = 0; i < 100; ++i) layer_destroy(layers[i]);
	neuron_class_destroy(n_class);
	assert(memory_leak() == FALSE, "memory_leak");

	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus layer_fully_connected_test() {
	TestStatus status = TEST_FAILED;
	// build 3 layers
	NeuronClass* n_class = neuron_class_create("LIF NEURON", LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default("SYN CLASS");

	const char* layer_input_name = "layer_input";
	uint32_t layer_input_length = 10;
	Layer* layer_input = layer_create_fully_connected(layer_input_length, n_class, layer_input_name);

	const char* layer_middle_name = "layer_middle";
	Layer* layer_middle = layer_create_fully_connected(100, n_class, layer_middle_name);

	const char* layer_output_name = "layer_output";
	Layer* layer_output = layer_create_fully_connected(100, n_class, layer_output_name);

	assert(layer_is_valid(layer_input) == TRUE, invalid_argument("layer_input"));
	assert(layer_is_valid(layer_middle) == TRUE, invalid_argument("layer_middle"));
	assert(layer_is_valid(layer_output) == TRUE, invalid_argument("layer_output"));

	LayerInputDataLink link_data = { 0 };
	link_data.input_layer = layer_input;
	link_data.s_class = s_class;
	link_data.connectivity = 1.0f;
	layer_middle->link(layer_middle, &link_data);
	
	link_data.input_layer = layer_middle;
	layer_output->link(layer_output, &link_data);

	uint32_t i = 0;
	uint32_t j = 0;

	assert(layer_is_valid(layer_input) == TRUE, invalid_argument("layer_input"));
	assert(layer_is_valid(layer_middle) == TRUE, invalid_argument("layer_middle"));
	assert(layer_is_valid(layer_output) == TRUE, invalid_argument("layer_output"));

	// every neuron of layer_input should have 100 output synapses
	for (i = 0; i < layer_input->neurons.length; ++i) {
		Neuron* neuron = (Neuron*)array_get(&(layer_input->neurons), i);
		assert(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
		assert(neuron->n_class == n_class, invalid_argument("neuron->n_class"));

		assert(neuron->out_synapses_refs.length == 100, "@neuron->out_synapses_refs.length is %u, not %u", neuron->out_synapses_refs.length, 100);
		for (j = 0; j < neuron->out_synapses_refs.length; ++j) {
			Synapse* synapse = *((Synapse**)array_get(&(neuron->out_synapses_refs), j));
			assert(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
			assert(synapse->s_class == s_class, invalid_argument("synapse->s_class"));
		}
	}

	// every neuron of layer_middle should have 100 input synapses and 100 output synapses
	for (i = 0; i < layer_middle->neurons.length; ++i) {
		Neuron* neuron = (Neuron*)array_get(&(layer_middle->neurons), i);
		assert(neuron_is_valid(neuron) == TRUE, invalid_argument("neurons"));
		assert(neuron->n_class == n_class, invalid_argument("neuron->n_class"));

		assert(neuron->in_synapses_refs.length == 10, "@neuron->in_synapses.length is %u, not %u", neuron->in_synapses_refs.length, 10);
		for (j = 0; j < neuron->in_synapses_refs.length; ++j) {
			Synapse* synapse = *((Synapse**)array_get(&(neuron->in_synapses_refs), j));
			assert(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
			assert(synapse->s_class == s_class, invalid_argument("synapse->s_class"));
		}

		assert(neuron->out_synapses_refs.length == 100, "@neuron->out_synapses_refs.length is %u, not %u", neuron->out_synapses_refs.length, 100);
		for (j = 0; j < neuron->out_synapses_refs.length; ++j) {
			Synapse* synapse = *((Synapse**)array_get(&(neuron->out_synapses_refs), j));
			assert(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
			assert(synapse->s_class == s_class, invalid_argument("synapse->s_class"));
		}
	}

	// every neuron of layer_output should have 100 input synapses
	for (i = 0; i < layer_output->neurons.length; ++i) {
		Neuron* neuron = (Neuron*)array_get(&(layer_output->neurons), i);
		assert(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
		assert(neuron->n_class == n_class, invalid_argument("neuron->n_class"));

		assert(neuron->in_synapses_refs.length == 100, "@neuron->in_synapses.length is %u, not %u", neuron->in_synapses_refs.length, 100);
		for (j = 0; j < neuron->in_synapses_refs.length; ++j) {
			Synapse* synapse = *((Synapse**)array_get(&(neuron->in_synapses_refs), j));
			assert(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
			assert(synapse->s_class == s_class, invalid_argument("synapse->s_class"));
		}
	}

	for (uint32_t i = 0; i < 1000; ++i) {
		ArrayFloat* currents = array_create(layer_input_length, layer_input_length, sizeof(float));
		float current = 1000.0f;
		for (j = 0; j < currents->length; ++j) array_set(currents, j, &current);

		layer_step(layer_input, i);
		layer_step(layer_middle, i);
		layer_step(layer_output, i);

		layer_step_inject_currents(layer_input, currents, i);

		array_destroy(currents, NULL);
	}

	layer_destroy(layer_input);
	layer_destroy(layer_middle);
	layer_destroy(layer_output);
	neuron_class_destroy(n_class);
	synapse_class_destroy(s_class);

	assert(memory_leak() == FALSE, "memory_leak");

	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus layer_fully_link_input_layer_test() {
	TestStatus status = TEST_FAILED;
	clock_t start, end;
	double cpu_time_used;

	NeuronClass* n_class = neuron_class_create("LIF_NEURON", LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default("SYN DEFAULT");

	Layer* l1 = layer_create(LAYER_FULLY_CONNECTED, 100, n_class, "layer1");
	Layer* l2 = layer_create(LAYER_FULLY_CONNECTED, 100, n_class, "layer2");
	Layer* l3 = layer_create(LAYER_FULLY_CONNECTED, 100, n_class, "layer3");
	layer_link_input_layer(l2, l1, s_class, 1.0f);
	layer_link_input_layer(l3, l2, s_class, 1.0f);
	layer_link_input_layer(l3, l1, s_class, 1.0f);

	start = clock();
	for (uint32_t i = 0; i < 100; ++i) {
		layer_step(l1, i);
		layer_step(l2, i);
		layer_step(l3, i);
	}
	end = clock();
	cpu_time_used = ((double)((size_t)end - start)) / CLOCKS_PER_SEC;
	printf("RUNS: %u TIME: %lf\n", 100, cpu_time_used);

	layer_destroy(l1);
	layer_destroy(l2);
	layer_destroy(l3);
	neuron_class_destroy(n_class);
	synapse_class_destroy(s_class);

	assert(memory_leak() == FALSE, "memory_leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus layer_get_min_byte_size_test() {
	TestStatus status = TEST_FAILED;

	NeuronClass* n_class = neuron_class_create("DEFAULT_N", LIF_NEURON);
	Layer* layer = layer_create_fully_connected(100, n_class, "layer");
	size_t layer_min_byte_size = layer_get_min_byte_size(layer);
	size_t layer_real_min_byte_size = sizeof(Layer) +
		(sizeof(String) + layer->name->length) +
		0 +
		100 * neuron_get_min_byte_size((Neuron*)array_get(&(layer->neurons), 0));
	assert(layer_min_byte_size == layer_real_min_byte_size, "Layer min size should be %llu, not %llu", layer_real_min_byte_size, layer_min_byte_size);

	layer_destroy(layer);
	neuron_class_destroy(n_class);

	assert(memory_leak() == FALSE, "memory_leak");
	status = TEST_SUCCESS;
error:
	return status;
}
