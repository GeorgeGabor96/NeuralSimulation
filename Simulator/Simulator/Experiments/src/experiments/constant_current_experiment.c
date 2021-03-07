#include "Simulator.h"
#include "data/data_gen_constant_current.h"
#include "callbacks/callback_visualize_layer_neurons.h"
#include "../../include/networks.h"


void constant_current_experiment() {
	Network* net = network_3_L_3_3_3();
	DataGenerator* constant_current = data_generator_constant_current_create(1, net, 15.0f, 100);
	Layer* l1 = network_get_layer_by_idx(net, 0);
	Layer* l2 = network_get_layer_by_idx(net, 1);
	Layer* l3 = network_get_layer_by_idx(net, 2);
	Callback* visu_1 = callback_visualize_layer_neurons_create(l1, ".\\constant_current_exp");
	Callback* visu_2 = callback_visualize_layer_neurons_create(l2, ".\\constant_current_exp");
	Callback* visu_3 = callback_visualize_layer_neurons_create(l3, ".\\constant_current_exp");

	Simulator* simulator = simulator_create(constant_current, net);
	simulator_add_callback(simulator, visu_1);
	simulator_add_callback(simulator, visu_2);
	simulator_add_callback(simulator, visu_3);

	simulator_infer(simulator);

	simulator_destroy(simulator);
}

void constant_current_learning_experiment() {
	// create network
	const char* result_path = ".\\experiments\\constant_current_learning_custom_synapses_inhibitie";

	Layer* layer_input = layer_create_fully_connected(2, 
								neuron_class_create(LIF_NEURON), 
								synapse_class_create(0.0f, 20.0f, 1, CONDUCTANCE_SYNAPSE, 1), 
								"layer_in");

	Layer* layer_output = layer_create_fully_connected(1, 
								neuron_class_create(LIF_NEURON),
								synapse_class_create(-75.0f, 50.0f, 1, VOLTAGE_DEPENDENT_SYNAPSE, 1), 
								"layer_out");
	layer_add_input_layer(layer_output, layer_input);

	Network* net = network_create();
	network_add_layer(net, layer_input, TRUE, TRUE, FALSE);
	network_add_layer(net, layer_output, TRUE, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	// create data generator
	DataGenerator* constant_current = data_generator_constant_current_create(1, net, 2.0f, 100);

	// create callbacks
	layer_input = network_get_layer_by_name(net, "layer_in");
	layer_output = network_get_layer_by_name(net, "layer_out");
	Callback* c1 = callback_visualize_layer_neurons_create(layer_input, result_path);
	Callback* c2 = callback_visualize_layer_neurons_create(layer_output, result_path);

	// create simulator
	Simulator* simulator = simulator_create(constant_current, net);
	simulator_add_callback(simulator, c1);
	simulator_add_callback(simulator, c2);

	// run experiments
	simulator_infer(simulator);

	// cleanup 
	simulator_destroy(simulator);
}