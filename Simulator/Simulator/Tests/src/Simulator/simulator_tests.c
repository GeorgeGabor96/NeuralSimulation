#include "simulator_tests.h"
#include "Simulator.h"
#include "callbacks/callback_visualize_layer_neurons.h"
#include "data/data_gen_constant_current.h"
#include "os.h"


TestStatus simulator_infer_test() {
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default();

	// create net linear with 2 layers
	Layer* layer_input = layer_create_fully_connected(100, n_class, s_class, "layer_input");
	Layer* layer_output = layer_create_fully_connected(1, n_class, s_class, "layer_output");
	layer_add_input_layer(layer_output, layer_input);

	Network* net = network_create();
	network_add_layer(net, layer_input, TRUE, TRUE, FALSE);
	network_add_layer(net, layer_output, TRUE, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	// create data generator
	DataGenerator* generator = data_generator_constant_current_create(10, net, 4.0f, 100);

	// create callbacks
	layer_output = *((Layer**)array_get(&(net->output_layers), 0));
	Callback* callback = callback_visualize_layer_neurons_create(layer_output, ".\\simulator_visu");

	// create the simulator
	Simulator* simulator = simulator_create(generator, net);
	assert(simulator_is_valid(simulator) == TRUE, invalid_argument("simulator"));
	simulator_add_callback(simulator, callback);
	assert(simulator_is_valid(simulator) == TRUE, invalid_argument("simulator"));

	simulator_infer(simulator);
	assert(simulator_is_valid(simulator) == TRUE, invalid_argument("simulator"));

	os_rmdir(".\\simulator_visu");
	simulator_destroy(simulator);
	neuron_class_destroy(n_class);
	synapse_class_destroy(s_class);
	assert(memory_leak() == FALSE, "Memory leak");

	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}