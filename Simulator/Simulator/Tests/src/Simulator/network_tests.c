#include "network_tests.h"
#include "Network.h"


TestStatus network_compile_general_use_case_test() {
	TestStatus t_status = TEST_FAILED;
	Status s_status = FAIL;

	Layer* layer = NULL;
	uint32_t layer_idx = 0;
	String* string = NULL;
	const char* neuron_class_name = "LIF NEURON";
	const char* synapse_class_name = "DEFAULT SYN";

	/*----------create_network----------*/
	Network* network = network_create();
	assert(network_is_valid(network) == TRUE, invalid_argument("network"));
	
	assert(network->neuron_classes.length == 0, "@network->neuron_classes.length should be 0 not %u", network->neuron_classes.length);
	s_status = network_add_neuron_class(network, neuron_class_create(neuron_class_name, LIF_NEURON));
	assert(s_status == SUCCESS, "@network_add_neuron_class failed");
	assert(network_is_valid(network) == TRUE, invalid_argument("network"));
	assert(network->neuron_classes.length == 1, "@network->neuron_classes.length should be 1 not %u", network->neuron_classes.length);

	assert(network->synapse_classes.length == 0, "@network->synapse_classes.length should be 0 not %u", network->synapse_classes.length);
	s_status = network_add_synapse_class(network, synapse_class_create_default(synapse_class_name));
	assert(s_status == SUCCESS, "@network_add_neuron_class failed");
	assert(network_is_valid(network) == TRUE, invalid_argument("network"));
	assert(network->synapse_classes.length == 1, "@network->synapse_classes.length should be 1 not %u", network->synapse_classes.length);

	NeuronClass* n_class = network_get_neuron_class_by_name(network, neuron_class_name);
	SynapseClass* s_class = network_get_synapse_class_by_name(network, synapse_class_name);

	// create a few layers
	char* name1 = "layer_1";
	char* name2 = "layer_2";
	char* name3 = "layer_3";
	Layer* l1 = layer_create_fully_connected(10, n_class, name1);
	Layer* l2 = layer_create_fully_connected(100, n_class,  name2);
	Layer* l3 = layer_create_fully_connected(1, n_class, name3);
	layer_add_input_layer(l2, l1, s_class);
	layer_add_input_layer(l3, l1, s_class);
	layer_add_input_layer(l3, l2, s_class);
	
	/*----------network_add_layer----------*/
	s_status = network_add_layer(network, l3, TRUE, FALSE, TRUE);
	assert(s_status == SUCCESS, "Couldn't add @l3");
	assert(network_is_valid(network) == TRUE, invalid_argument("network"));
	assert(network->layers.length == 1, "@network->layers.length is %u, not 1", network->layers.length);
	assert(network->input_names.length == 0, "@network->input_names.length is %u, not 0", network->input_names.length);
	assert(network->output_names.length == 1, "@network->output_names.length is %u, not 1", network->output_names.length);

	s_status = network_add_layer(network, l2, TRUE, FALSE, FALSE);
	assert(s_status == SUCCESS, "Couldn't add @l2");
	assert(network_is_valid(network) == TRUE, invalid_argument("network"));
	assert(network->layers.length == 2, "@network->layers.length is %u, not 2", network->layers.length);
	assert(network->input_names.length == 0, "@network->input_names.length is %u, not 0", network->input_names.length);
	assert(network->output_names.length == 1, "@network->output_names.length is %u, not 1", network->output_names.length);

	s_status = network_add_layer(network, l1, TRUE, TRUE, FALSE);
	assert(s_status == SUCCESS, "Couldn't add l1");
	assert(network_is_valid(network) == TRUE, invalid_argument("network"));
	assert(network->layers.length == 3, "@network->layers.length is %u, not 3", network->layers.length);
	assert(network->input_names.length == 1, "@network->input_names.length is %u, not 1", network->input_names.length);
	assert(network->output_names.length == 1, "@network->output_names.length is %u, not 1", network->output_names.length);

	/*----------network_get_layer_by_idx----------*/
	l3 = network_get_layer_by_idx(network, 0);
	assert(layer_is_valid(l3) == TRUE, invalid_argument("l3"));
	assert(strcmp(layer_get_name(l3), name3) == 0, invalid_argument("l3->name"));
	assert(l3->neurons.length == 1, invalid_argument("l3->neurons.length"));
	assert(l3->is_input == FALSE, "@l3->is_input is TRUE");
	
	l2 = network_get_layer_by_idx(network, 1);
	assert(layer_is_valid(l2) == TRUE, invalid_argument("l2"));
	assert(strcmp(layer_get_name(l2), name2) == 0, invalid_argument("l2->name"));
	assert(l2->neurons.length == 100, invalid_argument("l2->neurons.length"));
	assert(l2->is_input == FALSE, "@l2->is_input is TRUE");

	l1 = network_get_layer_by_idx(network, 2);
	assert(layer_is_valid(l1) == TRUE, invalid_argument("l1"));
	assert(strcmp(layer_get_name(l1), name1) == 0, invalid_argument("l1->name"));
	assert(l1->neurons.length == 10, invalid_argument("l1->neurons.length"));
	assert(l1->is_input == TRUE, "@1->is_input is FALSE");

	/*----------network_get_layer_by_name----------*/
	layer = network_get_layer_by_name(network, name2);
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(strcmp(layer_get_name(layer), name2) == 0, invalid_argument("layer->name"));
	assert(layer->neurons.length == 100, invalid_argument("layer->neurons->length"));

	// check network input names
	string = *((String**)array_get(&(network->input_names), 0));
	assert(string_is_valid(string) == TRUE, invalid_argument("string"));
	assert(strcmp(string_get_C_string(string), name1) == 0, invalid_argument("string"));

	// check network output names
	string = *((String**)array_get(&(network->output_names), 0));
	assert(string_is_valid(string) == TRUE, invalid_argument("string"));
	assert(strcmp(string_get_C_string(string), name3) == 0, invalid_argument("string"));
	
	/*----------network_compile----------*/
	s_status = network_compile(network);
	assert(s_status == SUCCESS, "could not compile @network");
	assert(network_is_valid(network) == TRUE, invalid_argument("network"));
	assert(network->input_layers.length == 1, "@network->input_layers.length is %u, not 1", network->input_layers.length);
	assert(network->output_layers.length == 1, "@network->output_layers.length is %u, not 1", network->output_layers.length);
	assert(network->compiled == TRUE, invalid_argument("network->compiled"));

	// check network input layers
	layer = *((Layer**)array_get(&(network->input_layers), 0));
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(strcmp(layer_get_name(layer), name1) == 0, invalid_argument("layer->name"));
	assert(layer->neurons.length == 10, invalid_argument("layer->neurons.length"));

	// check network output layers
	layer = *((Layer**)array_get(&(network->output_layers), 0));
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(strcmp(layer_get_name(layer), name3) == 0, invalid_argument("layer->name"));
	assert(layer->neurons.length == 1, invalid_argument("layer->neurons->length"));

	// test the reordering, it should be layer1->layer2->layer3
	layer = (Layer*)network_get_layer_by_idx(network, 0);
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(strcmp(layer_get_name(layer), name1) == 0, invalid_argument("layer"));
	assert(layer->neurons.length == 10, "@layer->neurons.length is %u, not 10", layer->neurons.length);

	layer = (Layer*)network_get_layer_by_idx(network, 1);
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(strcmp(layer_get_name(layer), name2) == 0, invalid_argument("layer"));
	assert(layer->neurons.length == 100, "@layer->neurons.length is %u, not 100", layer->neurons.length);

	layer = (Layer*)network_get_layer_by_idx(network, 2);
	assert(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	assert(strcmp(layer_get_name(layer), name3) == 0, invalid_argument("layer"));
	assert(layer->neurons.length == 1, "@layer->neurons.length is %u, not 1", layer->neurons.length);

	/*----------network_get_layer_idx_by_name----------*/
	layer_idx = network_get_layer_idx_by_name(network, name1);
	assert(layer_idx == 0, "@layer_idx is %u, not 0", layer_idx);

	layer_idx = network_get_layer_idx_by_name(network, name2);
	assert(layer_idx == 1, "@layer_idx is %u, not 1", layer_idx);

	layer_idx = network_get_layer_idx_by_name(network, name3);
	assert(layer_idx == 2, "@layer_idx is %u, not 2", layer_idx);

	/*----------corner_cases----------*/
	assert(network_add_layer(NULL, layer, FALSE, FALSE, FALSE) == FAIL, "Should fail for invalid @network");
	assert(network_add_layer(network, NULL, FALSE, FALSE, FALSE) == FAIL, "Should fail for invalid @layer");

	assert(network_get_layer_by_idx(NULL, 0) == NULL, "Should return NULL for invalid @network");
	assert(network_get_layer_by_idx(network, 100) == NULL, "Should return NULL for invalid @layer_idx");

	assert(network_get_layer_by_name(NULL, name1) == NULL, "Should return NULL for invalid @network");
	assert(network_get_layer_by_name(network, NULL) == NULL, "Should return NULL for invalid @name");

	assert(network_get_layer_idx_by_name(NULL, name1) == UINT32_MAX, "Should return UINT32_MAX for invalid @network");
	assert(network_get_layer_idx_by_name(network, NULL) == UINT32_MAX, "Should return UINT32_MAX for invalid @name");

	network_destroy(NULL);

	/*----------network_destroy----------*/
	network_destroy(network);
	assert(memory_leak() == FALSE, "Memory leak");

	t_status = TEST_SUCCESS;
error:
	return t_status;
}


TestStatus network_step_test() {
	TestStatus status = TEST_FAILED;
	Layer* layer = NULL;
	uint32_t i = 0;
	const char* neuron_class_name = "LIF NEURON";
	const char* synapse_class_name = "DEFAULT SYN";

	// create network
	Network* network = network_create();
	network_add_neuron_class(network, neuron_class_create(neuron_class_name, LIF_NEURON));
	network_add_synapse_class(network, synapse_class_create_default(synapse_class_name));
	NeuronClass* n_class = network_get_neuron_class_by_name(network, neuron_class_name);
	SynapseClass* s_class = network_get_synapse_class_by_name(network, synapse_class_name);

	// create a few layers
	uint32_t input_neuron_length = 5;
	Layer* layer1 = layer_create_fully_connected(input_neuron_length, n_class, "layer1");
	Layer* layer2 = layer_create_fully_connected(100, n_class, "layer2");
	Layer* layer3 = layer_create_fully_connected(10, n_class, "layer3");
	layer_add_input_layer(layer2, layer1, s_class);
	layer_add_input_layer(layer3, layer2, s_class);

	// add layers into a network
	network_add_layer(network, layer1, TRUE, TRUE, FALSE);
	network_add_layer(network, layer2, TRUE, FALSE, FALSE);
	network_add_layer(network, layer3, TRUE, FALSE, TRUE);
	assert(network->layers.length == 3, invalid_argument("network->layers.length"));
	network_compile(network);
	assert(network_is_valid(network) == TRUE, invalid_argument("network"));

	// build input for network
	NetworkInputs* inputs = array_create(1, 0, sizeof(NetworkValues));
	NetworkValues currents;
	currents.type = CURRENT;
	array_init(&(currents.values), input_neuron_length, input_neuron_length, sizeof(float));
	float PSC = 10.0f;
	for (i = 0; i < currents.values.length; ++i) array_set(&(currents.values), i, &PSC);
	array_append(inputs, &currents);

	for (i = 0; i < 100; ++i) {
		network_step(network, inputs, i);
		log_info("Loop %u", i);
		Array* output_spikes = network_get_output_spikes(network);
		for (uint32_t j = 0; j < output_spikes->length; ++j) {
			ArrayBool* spikes = (ArrayBool*)array_get(output_spikes, j);
			array_show(spikes, show_bool);
			array_reset(spikes, NULL);
		}
		array_destroy(output_spikes, NULL);
	}
	array_reset(&(currents.values), NULL);
	array_destroy(inputs, NULL);

	network_destroy(network);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;
error:

	return status;
}


TestStatus network_summary_test() {
	const char* neuron_class_name = "LIF NEURON";
	const char* synapse_class_name = "DEFAULT SYN";

	Network* network = network_create();
	network_add_neuron_class(network, neuron_class_create(neuron_class_name, LIF_NEURON));
	network_add_synapse_class(network, synapse_class_create_default(synapse_class_name));
	NeuronClass* n_class = network_get_neuron_class_by_name(network, neuron_class_name);
	SynapseClass* s_class = network_get_synapse_class_by_name(network, synapse_class_name);
	
	Layer* l_input_1 = layer_create_fully_connected(10, n_class, "l_input_1");
	Layer* l_input_2 = layer_create_fully_connected(100, n_class, "l_input_2");
	Layer* l_inner_1 = layer_create_fully_connected(100, n_class, "l_inner_1");
	Layer* l_inner_2 = layer_create_fully_connected(100, n_class, "l_inner_2");
	Layer* l_output_1 = layer_create_fully_connected(100, n_class, "l_output_1");
	Layer* l_output_2 = layer_create_fully_connected(10, n_class, "l_output_2");

	layer_add_input_layer(l_inner_1, l_input_1, s_class);
	layer_add_input_layer(l_inner_1, l_input_2, s_class);
	
	layer_add_input_layer(l_inner_2, l_input_1, s_class);
	layer_add_input_layer(l_inner_2, l_input_2, s_class);
	layer_add_input_layer(l_inner_2, l_inner_1, s_class);

	layer_add_input_layer(l_output_1, l_inner_2, s_class);
	
	layer_add_input_layer(l_output_2, l_inner_1, s_class);
	layer_add_input_layer(l_output_2, l_inner_2, s_class);

	network_add_layer(network, l_input_1, TRUE, TRUE, FALSE);
	network_add_layer(network, l_input_2, TRUE, TRUE, FALSE);
	network_add_layer(network, l_inner_1, TRUE, FALSE, FALSE);
	network_add_layer(network, l_inner_2, TRUE, FALSE, FALSE);
	network_add_layer(network, l_output_1, TRUE, FALSE, TRUE);
	network_add_layer(network, l_output_2, TRUE, FALSE, TRUE);

	network_summary(network);
	network_compile(network);
	network_summary(network);

	network_destroy(network);
	assert(memory_leak() == FALSE, "Memory leak");

	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}