#include "simulator_tests.h"
#include "Simulator.h"
#include "callbacks/callback_dump.h"
#include "data/data_gen_constant_current.h"
#include "utils/os.h"


TestStatus simulator_infer_test() {
	// create net linear with 2 layers
	const char* n_class_name = "TEST NEURON";
	const char* s_class_name = "TEST SYNAPSE";
	Network* net = network_create();
	network_add_neuron_class(net, neuron_class_create(n_class_name, LIF_NEURON));
	network_add_synapse_class(net, synapse_class_create_default(s_class_name));
	NeuronClass* n_class = network_get_neuron_class_by_name(net, n_class_name);
	SynapseClass* s_class = network_get_synapse_class_by_name(net, s_class_name);

	Layer* layer_input = layer_create_fully_connected(100, n_class, "layer_input");
	Layer* layer_output = layer_create_fully_connected(1, n_class, "layer_output");
	layer_add_input_layer(layer_output, layer_input, s_class, 1.0f);

	network_add_layer(net, layer_input, TRUE, FALSE);
	network_add_layer(net, layer_output, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	// create data generator
	DataGenerator* generator = data_generator_constant_current_create(10, net, 4.0f, 100);

	// create callbacks
	layer_output = *((Layer**)array_get(&(net->output_layers), 0));
	Callback* callback = callback_dump_layer_neurons_create(layer_output, ".\\simulator_visu", FALSE);

	// create the simulator
	Simulator* simulator = simulator_create(generator, net);
	assert(simulator_is_valid(simulator) == TRUE, invalid_argument("simulator"));
	simulator_add_callback(simulator, callback);
	assert(simulator_is_valid(simulator) == TRUE, invalid_argument("simulator"));

	simulator_infer(simulator);
	assert(simulator_is_valid(simulator) == TRUE, invalid_argument("simulator"));

	os_rmdir(".\\simulator_visu");
	simulator_destroy(simulator);
	assert(memory_leak() == FALSE, "Memory leak");

	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}