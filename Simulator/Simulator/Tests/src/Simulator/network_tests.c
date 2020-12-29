#include "network_tests.h"
#include "Network.h"
#include "debug.h"


TestStatus network_create_compile_destroy_test() {
	TestStatus status = TEST_FAILED;
	Layer* layer = NULL;

	// create a few layers
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default();

	// layer 1
	Array* name1 = string_create("layer1");
	Vector* input_names1 = vector_create(1, sizeof(Array*));
	Layer* layer1 = layer_create_fully_connected(10, n_class, s_class, name1, input_names1);

	// layer 2
	Array* name2 = string_create("layer2");
	char* inputs_2[1] = { "layer1" };
	Vector* input_names2 = string_vector_create(inputs_2, 1);
	Layer* layer2 = layer_create_fully_connected(100, n_class, s_class, name2, input_names2);

	// layer 3
	Array* name3 = string_create("layer3");
	char* inputs_3[1] = { "layer2" };
	Vector* input_names3 = string_vector_create(inputs_3, 1);
	Layer* layer3 = layer_create_fully_connected(1, n_class, s_class, name3, input_names3);

	// add layers into a network
	Network* network = network_create();
	assert(network != NULL, null_argument("network"));
	network_add_layer(network, layer1, TRUE, TRUE, FALSE);
	network_add_layer(network, layer2, TRUE, FALSE, FALSE);
	network_add_layer(network, layer3, TRUE, FALSE, TRUE);
	assert(network->layers->length == 3, invalid_argument("network->layers->length"));
	
	// check network layers
	layer1 = network_get_layer_by_idx(network, 0);
	assert(layer_is_valid(layer1) == TRUE, invalid_argument("layer1"));
	assert(string_compare(layer1->name, name1) == 0, invalid_argument("layer1->name"));
	assert(layer1->neurons->length == 10, invalid_argument("layer1->neurons->length"));
	
	layer2 = network_get_layer_by_idx(network, 1);
	assert(layer_is_valid(layer2) == TRUE, invalid_argument("layer2"));
	assert(string_compare(layer2->name, name2) == 0, invalid_argument("layer2->name"));
	assert(layer2->neurons->length == 100, invalid_argument("layer2->neurons->length"));
	
	layer3 = network_get_layer_by_idx(network, 2);
	assert(layer_is_valid(layer3) == TRUE, invalid_argument("layer3"));
	assert(string_compare(layer3->name, name3) == 0, invalid_argument("layer3->name"));
	assert(layer3->neurons->length == 1, invalid_argument("layer3->neurons->length"));

	Array* name = string_create("layer2");
	layer2 = network_get_layer_by_name(network, name);
	string_destroy(name);
	assert(layer_is_valid(layer2) == TRUE, invalid_argument("layer2"));
	assert(string_compare(layer2->name, name2) == 0, invalid_argument("layer2->name"));
	assert(layer2->neurons->length == 100, invalid_argument("layer2->neurons->length"));

	// check network input layers
	assert(network->input_layers->length == 1, invalid_argument("network->input_layers->length"));
	layer = *((Layer**)vector_get(network->input_layers, 0));
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(string_compare(layer->name, name1) == 0, invalid_argument("layer->name"));
	assert(layer->neurons->length == 10, invalid_argument("layer->neurons->length"));
	
	// check network output layers
	assert(network->output_layers->length == 1, invalid_argument("network->output_layers->length"));
	layer = *((Layer**)vector_get(network->output_layers, 0));
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(string_compare(layer->name, name3) == 0, invalid_argument("layer->name"));
	assert(layer->neurons->length == 1, invalid_argument("layer->neurons->length"));

	// compile network
	assert(network_compile(network) == SUCCESS, "could not compile @network");
	assert(network->compiled == TRUE, invalid_argument("network->compiled"));

	network_destroy(network);

	status = TEST_SUCCESS;
error:
	if (n_class != NULL) neuron_class_destroy(n_class);
	if (s_class != NULL) synapse_class_destroy(s_class);


	return status;
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
	network_add_layer(network, &dummy_layer, TRUE, TRUE, FALSE);
	
	layer_init(&dummy_layer, LAYER_FULLY_CONNECTED, 1, neuron_class, synapse_class, NULL, NULL);
	network_add_layer(network, &dummy_layer, TRUE, FALSE, TRUE);

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