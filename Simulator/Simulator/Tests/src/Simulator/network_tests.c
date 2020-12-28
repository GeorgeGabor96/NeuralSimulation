#include "network_tests.h"
#include "Network.h"
#include "debug.h"


TestStatus network_create_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus network_destroy_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus network_is_ready_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus network_compile_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus network_get_layer_by_idx_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus network_add_layer_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus network_step_test() {
	/*
	Simple test to make sure everything is working
	*/
	TestStatus status = TEST_FAILED;
	Network* network = network_create();
	NeuronClass* neuron_class = neuron_class_create(LIF_NEURON);
	SynapseClass* synapse_class = synapse_class_create_default();
	Layer dummy_layer;

	// build network
	layer_init(&dummy_layer, LAYER_FULLY_CONNECTED, 1, neuron_class, synapse_class, NULL, NULL);
	network_add_layer(network, &dummy_layer, TRUE, FALSE);
	
	layer_init(&dummy_layer, LAYER_FULLY_CONNECTED, 1, neuron_class, synapse_class, NULL, NULL);
	network_add_layer(network, &dummy_layer, FALSE, TRUE);

	// build connections
	network_compile(network);

	Layer* layer = *(Layer**)vector_get(network->input_layers, 0);
	printf("%d\n", layer->neurons->length);

	// build input for network
	Vector* inputs = vector_create(1, sizeof(NetworkValues));
	Array* values = array_create(1, sizeof(float));
	float PSC = 1.0f;
	array_set(values, 0, &PSC);
	NetworkValues net_input;
	net_input.type = CURRENT;
	net_input.values = values;
	vector_append(inputs, &net_input);

	layer = *(Layer**)vector_get(network->input_layers, 0);
	printf("%d\n", layer->neurons->length);

	for (int i = 0; i < 100; ++i) {
		log_info("Loop %d", i);
		network_step(network, inputs, i);

		//Array* outputs = network_get_outputs(network, VOLTAGE);
		//network_values_show(outputs);
	}
	status = TEST_SUCCESS;


	array_destroy(values, NULL);
	vector_destroy(inputs, NULL);

	network_destroy(network);
	neuron_class_destroy(neuron_class);
	synapse_class_destroy(synapse_class);
	
	return status;
}