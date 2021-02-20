#include "callbacks_tests.h"

#include "callbacks/callback.h"
#include "callbacks/callback_visualize_layer_neurons.h"

TestStatus callbacks_visualize_layer_neurons_test() {
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default();
	Layer* layer = layer_create_fully_connected(10, n_class, s_class, "layer_visu");
	Callback* callback_visu = callback_visualize_layer_neurons_create(layer, ".\\visu");

	assert(callback_is_valid(callback_visu) == TRUE, invalid_argument("callback"));


	callback_destroy(callback_visu);
	layer_destroy(layer);
	neuron_class_destroy(n_class);
	synapse_class_destroy(s_class);
	
	assert(memory_leak() == FALSE, "Memory leak");
	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}
