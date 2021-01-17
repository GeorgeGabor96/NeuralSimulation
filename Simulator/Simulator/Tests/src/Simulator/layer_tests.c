#include "layer_tests.h"

#include "Layer.h"


TestStatus layer_general_use_case_test() {
	TestStatus status = TEST_FAILED;
	Status s = FAIL;
	uint32_t i = 0;
	Neuron* neuron = NULL;
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default();
	
	String* l_name = string_create("Layer test");
	String* string1 = NULL;
	String* string2 = NULL;
	char* input_names[2] = { "Layer input 1", "Layer input 2" };
	Array* l_input_names = strings_create(input_names, 2);
	Layer* layer = layer_create(LAYER_FULLY_CONNECTED, 100, n_class, s_class, l_name, l_input_names);

	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(layer->neurons.length == 100, invalid_argument("layer->neurons->length"));
	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		assert(neuron_is_valid(neuron) == TRUE, "neuron %u is invalid", i);
	}
	assert(layer->link == layer_link_fc, invalid_argument("layer->link"));
	assert(string_compare(l_name, layer->name) == 0, "Should be equal");
	for (i = 0; i < l_input_names->length; ++i) {
		string1 = *((String**)array_get(l_input_names, i));
		string2 = *((String**)array_get(layer->input_names, i));
		assert(string_compare(string1, string2) == 0, "Should be equal");
	}

	// test case 1: force spike on every neuron, make a step, and verify that no neuron has spiked
	ArrayBool* spikes = array_create(layer->neurons.length, layer->neurons.length, sizeof(bool));
	bool spike = FALSE;
	for (i = 0; i < spikes->length; ++i) {
		spike = TRUE;
		array_set(spikes, i, &spike);
	}
	s = layer_force_spikes(layer, spikes, 0);
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
		spike = *((bool*)array_get(spikes, i));
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
	s = layer_inject_currents(layer, currents, 2);
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
		spike = *((bool*)array_get(spikes, i));
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
	s = layer_inject_currents(layer, currents, 4);
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
		spike = *((bool*)array_get(spikes, i));
		assert(spike == FALSE, "neuron %u spiked", i);
	}
	array_destroy(spikes, NULL);

	// test case 4: get voltages, don't care about values
	ArrayFloat* voltages = layer_get_voltages(layer);
	check(array_is_valid(voltages) == TRUE, invalid_argument("voltages"));
	array_destroy(voltages, NULL);

	// corner cases
	assert(layer_create(LIF_NEURON, 100, NULL, s_class, l_name, l_input_names) == NULL, "Should return NULL for invalid @n_class");
	assert(layer_create(LIF_NEURON, 100, n_class, NULL, l_name, l_input_names) == NULL, "Should return NULL for invalid @s_class");
	assert(layer_create(LIF_NEURON, 100, n_class, s_class, NULL, l_input_names) == NULL, "Should return NULL for invalid @name");
	assert(layer_create(LIF_NEURON, 100, n_class, s_class, l_name, NULL) == NULL, "Should return NULL for invalid @input_names");

	assert(layer_force_spikes(NULL, spikes, 0u) == FAIL, "Should fail for invalid @layer");
	assert(layer_force_spikes(layer, NULL, 0u) == FAIL, "Should fail for invalid @spikes");

	assert(layer_step(NULL, 0u) == FAIL, "Should fail for invalid @layer");

	assert(layer_get_spikes(NULL) == NULL, "Should return NULL for invalid @layer");

	layer_destroy(NULL);


	layer_destroy(layer);
	neuron_class_destroy(n_class);
	synapse_class_destroy(s_class);
	assert(memory_leak() == FALSE, "memory_leak");

	status = TEST_SUCCESS;
error:

	return status;
}


TestStatus layer_memory_test_test() {
	TestStatus status = TEST_FAILED;
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default();
	String* l_name = NULL;
	String* string1 = NULL;
	String* string2 = NULL;
	char* input_names[2] = { "Layer input 1", "Layer input 2" };
	Array* l_input_names = NULL;
	ArrayBool* spikes = NULL;
	ArrayFloat* voltages = NULL;
	ArrayFloat* currents = NULL;
	bool spike = FALSE;

	Layer* layers[100] = { NULL };
	for (i = 0; i < 100; ++i) {
		l_name = string_create("Layer test");
		l_input_names = strings_create(input_names, 2);
		layers[i] = layer_create(LAYER_FULLY_CONNECTED, 100, n_class, s_class, l_name, l_input_names);
	}

	spikes = array_create(layers[0]->neurons.length, layers[0]->neurons.length, sizeof(bool));
	spike = TRUE;
	for (i = 0; i < 100; ++i) array_set(spikes, i, &spike);
	for (i = 0; i < 100; ++i) {
		for (j = 0; j < 100; ++j) {
			layer_force_spikes(layers[i], spikes, j);
			layer_step(layers[i], j);
			voltages = layer_get_voltages(layers[i]);
			array_destroy(voltages, NULL);
		}
	}
	array_destroy(spikes, NULL);

	currents = array_create(layers[0]->neurons.length, layers[0]->neurons.length, sizeof(float));
	for (i = 0; i < 100; ++i) {
		for (j = 0; j < 100; ++j) {
			layer_inject_currents(layers[i], currents, j);
			layer_step(layers[i], j);
			voltages = layer_get_voltages(layers[i]);
			array_destroy(voltages, NULL);
		}
	}
	array_destroy(currents, NULL);

	for (i = 0; i < 100; ++i) layer_destroy(layers[i]);
	neuron_class_destroy(n_class);
	synapse_class_destroy(s_class);
	assert(memory_leak() == FALSE, "memory_leak");

	status = TEST_SUCCESS;
error:
	return status;
}