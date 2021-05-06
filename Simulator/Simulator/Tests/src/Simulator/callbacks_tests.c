#include "callbacks_tests.h"

#include "utils/os.h"
#include "callbacks/callback.h"
#include "callbacks/callback_dump.h"
#include "data/data_gen_constant_current.h"


/********************
HELPER FUNCTIONS
********************/
static inline Network* get_network() {
	Network* net = network_create();
	network_add_neuron_class(net, neuron_class_create("TEST_NEURON", LIF_NEURON));
	network_add_synapse_class(net, synapse_class_create_default("TEST_SYNAPSE"));

	NeuronClass* n_class = network_get_neuron_class_by_name(net, "TEST_NEURON");
	SynapseClass* s_class = network_get_synapse_class_by_name(net, "TEST_SYNAPSE");

	Layer* l1 = layer_create(LAYER_FULLY_CONNECTED, 10, n_class, "l1");
	Layer* l2 = layer_create(LAYER_FULLY_CONNECTED, 10, n_class, "l2");
	Layer* l3 = layer_create(LAYER_FULLY_CONNECTED, 10, n_class, "l3");
	layer_add_input_layer(l2, l1, s_class, 1.0f, 1.0f);
	layer_add_input_layer(l3, l2, s_class, 1.0f, 1.0f);

	network_add_layer(net, l1, TRUE, FALSE);
	network_add_layer(net, l2, FALSE, FALSE);
	network_add_layer(net, l3, FALSE, TRUE);
	network_compile(net);

	return net;
}


static inline void simulator_step(DataGenerator* data, Network* net, Callback* cb) {
	DataElement* element = NULL;
	for (uint32_t i = 0; i < data->length; ++i) {
		element = data_generator_get_element(data, i);

		network_clear_state(net);
		for (uint32_t time = 0; time < element->duration; ++time) {
			NetworkInputs* inputs = data_element_get_values(element, time);
			network_step(net, inputs, time);
			callback_update(cb, net);
			data_element_remove_values(element, inputs);
		}
		data_element_destroy(element);
		callback_run(cb, net);
	}
}


/********************
TEST FUNCTIONS
********************/
TestStatus callback_dump_layer_neurons_test() {
	NeuronClass* n_class = neuron_class_create("TEST NEURON", LIF_NEURON);
	Layer* layer = layer_create_fully_connected(10, n_class, "layer_visu");
	Callback* callback_visu = callback_dump_layer_neurons_create(layer, ".\\callback_visu");
	ArrayFloat* constant_current = array_ones_float(10);
	assert(callback_is_valid(callback_visu) == TRUE, invalid_argument("callback"));

	Network* net = network_create();

	for (uint32_t i = 0; i < 100; ++i) {
		layer_step_inject_currents(layer, constant_current, i);
		callback_update(callback_visu, net);
	}
	callback_run(callback_visu, net);

	network_destroy(net);
	callback_destroy(callback_visu);
	layer_destroy(layer);
	neuron_class_destroy(n_class);
	array_destroy(constant_current, NULL);
	
	os_rmdir(".\\callback_visu");

	assert(memory_leak() == FALSE, "Memory leak");
	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}


TestStatus callback_dump_network_test() {
	Network* net = get_network();
	Callback* net_dump = callback_dump_network_create(net, ".\\callback_net_dump");
	DataGenerator* data = data_generator_constant_current_create(10, net, 10.f, 100);
	
	simulator_step(data, net, net_dump);

	callback_destroy(net_dump);
	data_generator_destroy(data);
	network_destroy(net);

	os_rmdir(".\\callback_net_dump");

	assert(memory_leak() == FALSE, "Memory leak");
	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}


TestStatus callback_detect_synfire_activity_test() {
	Network* net = get_network();
	const char* file_path = "test.txt";
	Callback* syn_detect = callback_detect_synfire_activity_create(net, 200, 0.5f, 1.5f, file_path);
	DataGenerator* data = data_generator_constant_current_create(10, net, 10.f, 100);
	
	simulator_step(data, net, syn_detect);

	callback_destroy(syn_detect);
	data_generator_destroy(data);
	network_destroy(net);

	os_rmfile(file_path);

	assert(memory_leak() == FALSE, "Memory leak");
	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}
