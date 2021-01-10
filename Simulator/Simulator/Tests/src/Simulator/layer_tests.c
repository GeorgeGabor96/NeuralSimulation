#include "layer_tests.h"

#include "Layer.h"


static inline Layer* layer_get_default(NeuronClass* n_class, SynapseClass* s_class) {
	Array* l_name = string_create("Layer test");
	char* input_names[2] = { "Layer input 1", "Layer input 2" };
	Array* l_input_names = strings_create(input_names, 2);
	return layer_create(LAYER_FULLY_CONNECTED, 100, n_class, s_class, l_name, l_input_names);
}


TestStatus layer_create_destroy_test() {
	TestStatus status = TEST_FAILED;
	uint32_t i = 0;
	Neuron* neuron = NULL;

	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default();
	Layer* layer = layer_get_default(n_class, s_class);

	assert(layer != NULL, null_argument("layer"));
	assert(layer->type == LAYER_FULLY_CONNECTED, invalid_argument("layer->type"));
	assert(neuron_class_is_valid(layer->neuron_class) == TRUE, invalid_argument("layer->neuron_class"));
	assert(synapse_class_is_valid(layer->synapse_class) == TRUE, invalid_argument("layer->synapse_class"));
	assert(array_is_valid(layer->neurons) == TRUE, invalid_argument("layer->neurons"));
	assert(layer->neurons->length == 100, invalid_argument("layer->neurons->length"));
	for (i = 0; i < layer->neurons->length; ++i) {
		neuron = (Neuron*)array_get(layer->neurons, i);
		assert(neuron_is_valid(neuron) == TRUE, "neuron %u is invalid", i);
	}
	assert(layer->link == layer_link_fc, invalid_argument("layer->link"));
	assert(array_is_valid(layer->name) == TRUE, invalid_argument("layer->name"));
	assert(array_is_valid(layer->input_names) == TRUE, invalid_argument("layer->input_names"));

	layer_destroy(layer);

	status = TEST_SUCCESS;
error:
	if (n_class != NULL) neuron_class_destroy(n_class);
	if (s_class != NULL) synapse_class_destroy(s_class);

	return status;
}


TestStatus layer_step_test() {
	TestStatus status = TEST_FAILED;
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default();
	Layer* layer = layer_get_default(n_class, s_class);
	Neuron* neuron = NULL;
	Status spike = TRUE;
	uint32_t i = 0;

	// test case 1: force spike on every neuron, make a step, and verify that no neuron has spiked
	Array* spikes = array_create(layer->neurons->length, sizeof(Status));
	for (i = 0; i < spikes->length; ++i) {
		spike = TRUE;
		array_set(spikes, i, &spike);
	}
	layer_set_spikes(layer, spikes, 0);
	array_destroy(spikes, NULL);

	for (i = 0; i < layer->neurons->length; ++i) {
		neuron = (Neuron*)array_get(layer->neurons, i);
		assert(neuron->spike == TRUE, "neuron %u didn't spike", i);
	}

	layer_step(layer, 1);
	
	// neurons didn't have any input current -> imposible to spike
	spikes = layer_get_spikes(layer);
	for (i = 0; i < spikes->length; ++i) {
		spike = *((Status*)array_get(spikes, i));
		assert(spike == FALSE, "neuron %u spiked", i);
	}

	status = TEST_SUCCESS;
error:
	if (layer != NULL) layer_destroy(layer);
	if (n_class != NULL) neuron_class_destroy(n_class);
	if (s_class != NULL) synapse_class_destroy(s_class);

	return status;
}