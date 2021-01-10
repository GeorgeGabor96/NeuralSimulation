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
	Array* input_names1 = array_create(1, sizeof(Array*));
	Layer* layer1 = layer_create_fully_connected(10, n_class, s_class, name1, input_names1);

	// layer 2
	Array* name2 = string_create("layer2");
	char* inputs_2[1] = { "layer1" };
	Array* input_names2 = string_create(inputs_2, 1);
	Layer* layer2 = layer_create_fully_connected(100, n_class, s_class, name2, input_names2);

	// layer 3
	Array* name3 = string_create("layer3");
	char* inputs_3[1] = { "layer2" };
	Array* input_names3 = string_create(inputs_3, 1);
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
	layer = *((Layer**)array_get(network->input_layers, 0));
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(string_compare(layer->name, name1) == 0, invalid_argument("layer->name"));
	assert(layer->neurons->length == 10, invalid_argument("layer->neurons->length"));
	
	// check network output layers
	assert(network->output_layers->length == 1, invalid_argument("network->output_layers->length"));
	layer = *((Layer**)array_get(network->output_layers, 0));
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(string_compare(layer->name, name3) == 0, invalid_argument("layer->name"));
	assert(layer->neurons->length == 1, invalid_argument("layer->neurons->length"));

	// compile network
	assert(network_compile(network) == SUCCESS, "could not compile @network");
	assert(network->compiled == TRUE, invalid_argument("network->compiled"));

	// test the reordering, it should not changed the current order of layers
	layer = (Layer*)network_get_layer_by_idx(network, 0);
	assert(string_compare(layer->name, name1) == 0, invalid_argument("layer1"));
	layer = (Layer*)network_get_layer_by_idx(network, 1);
	assert(string_compare(layer->name, name2) == 0, invalid_argument("layer2"));
	layer = (Layer*)network_get_layer_by_idx(network, 2);
	assert(string_compare(layer->name, name3) == 0, invalid_argument("layer3"));

	network_destroy(network);

	status = TEST_SUCCESS;
error:
	if (n_class != NULL) neuron_class_destroy(n_class);
	if (s_class != NULL) synapse_class_destroy(s_class);

	return status;
}


TestStatus network_step_test() {
	TestStatus status = TEST_FAILED;
	Layer* layer = NULL;
	uint32_t i = 0;

	// create a few layers
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default();

	// layer 1
	Array* name1 = string_create("layer1");
	uint32_t input_length = 10;
	Array* input_names1 = array_create(1, sizeof(Array*));
	Layer* layer1 = layer_create_fully_connected(input_length, n_class, s_class, name1, input_names1);

	// layer 2
	Array* name2 = string_create("layer2");
	char* inputs_2[1] = { "layer1" };
	Array* input_names2 = string_create(inputs_2, 1);
	Layer* layer2 = layer_create_fully_connected(100, n_class, s_class, name2, input_names2);

	// layer 3
	Array* name3 = string_create("layer3");
	char* inputs_3[1] = { "layer2" };
	Array* input_names3 = strings_create(inputs_3, 1);
	Layer* layer3 = layer_create_fully_connected(1, n_class, s_class, name3, input_names3);

	// add layers into a network
	Network* network = network_create();
	assert(network != NULL, null_argument("network"));
	network_add_layer(network, layer1, TRUE, TRUE, FALSE);
	network_add_layer(network, layer2, TRUE, FALSE, FALSE);
	network_add_layer(network, layer3, TRUE, FALSE, TRUE);
	assert(network->layers->length == 3, invalid_argument("network->layers->length"));
	network_compile(network);

	// build input for network
	Array* inputs = array_create(1, sizeof(NetworkValues));
	Array* values = array_create(input_length, sizeof(float));
	float PSC = 1.0f;
	for (i = 0; i < values->length; ++i) array_set(values, i, &PSC);
	
	NetworkValues net_input;
	net_input.type = CURRENT;
	net_input.values = values;
	array_append(inputs, &net_input);

	for (int i = 0; i < 100; ++i) {
		log_info("Loop %d", i);
		network_step(network, inputs, i);
		Array* outputs = network_get_outputs(network, SPIKES);
		//continue; // WTF: it appears that after freeing the memory it works -> WHY::::::::::::::::
		for (uint32_t j = 0; j < outputs->length; ++j) {
			NetworkValues* values = (NetworkValues*)array_get(outputs, j);
			array_show(values->values, show_status);
			array_destroy(values->values, NULL);
		}
		array_destroy(outputs, NULL);
		//network_values_show(outputs);
	}
	
	status = TEST_SUCCESS;
error:
	if (network != NULL) network_destroy(network);
	if (n_class != NULL) neuron_class_destroy(n_class);
	if (s_class != NULL) synapse_class_destroy(s_class);

	return status;
}