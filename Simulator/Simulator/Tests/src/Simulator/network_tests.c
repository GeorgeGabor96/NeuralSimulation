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

	network_add_layer(network, layer_create(LAYER_FULLY_CONNECTED, 100, neuron_class, synapse_class));
	network_add_layer(network, layer_create(LAYER_FULLY_CONNECTED, 10, neuron_class, synapse_class));

	// verify that all the layers are ok
	check(network_is_ready(network) == TRUE, "Should be ready");

	// build connections
	network_compile(network);

	network_step(network, 0);

	status = TEST_SUCCESS;
error:
	network_destroy(network);
	neuron_class_destroy(neuron_class);
	synapse_class_destroy(synapse_class);
	
	return status;
}