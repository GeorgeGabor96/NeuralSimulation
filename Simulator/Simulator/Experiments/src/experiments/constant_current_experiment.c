#include "Simulator.h"
#include "DataGen.h"
#include "Callback.h"

#include "../../include/networks.h"
#include "../../include/config.h"


void constant_current_experiment() {
	char result_dir[256] = { 0 };
	sprintf(result_dir, "%s\\constant_current_exp", result_base_folder);

	Network* net = network_3_L_3_3_3();
	DataGenerator* constant_current = data_generator_constant_current_create(1, net, 15.0f, 100);
	Layer* l1 = network_get_layer_by_idx(net, 0);
	Layer* l2 = network_get_layer_by_idx(net, 1);
	Layer* l3 = network_get_layer_by_idx(net, 2);
	Callback* visu_1 = callback_dump_layer_neurons_create(l1, result_dir);
	Callback* visu_2 = callback_dump_layer_neurons_create(l2, result_dir);
	Callback* visu_3 = callback_dump_layer_neurons_create(l3, result_dir);

	Simulator* simulator = simulator_create(constant_current, net);
	simulator_add_callback(simulator, visu_1);
	simulator_add_callback(simulator, visu_2);
	simulator_add_callback(simulator, visu_3);

	simulator_infer(simulator);

	simulator_destroy(simulator);
}

void constant_current_learning_experiment() {
	// create network
	char result_dir[256] = { 0 };
	sprintf(result_dir, "%s\\constant_current_exp_learning", result_base_folder);

	Network* net = network_create();
	NeuronClass* n_class = neuron_class_create("TEST_N", LIF_NEURON);
	SynapseClass* s_class_conductance = synapse_class_create("TEST_CONDUCTANCE", 0.0f, 1.0f, 20.0f, 1, CONDUCTANCE_SYNAPSE, 1);
	SynapseClass* s_class_voltage = synapse_class_create("TEST_VOLTAGE", -75.0f, 1.0f, 50.0f, 1, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	network_add_neuron_class(net, n_class);
	network_add_synapse_class(net, s_class_conductance);
	network_add_synapse_class(net, s_class_voltage);

	Layer* layer_input = layer_create_fully_connected(2, n_class, "layer_in");
	Layer* layer_output = layer_create_fully_connected(1, n_class, "layer_out");
	layer_add_input_layer(layer_output, layer_input, s_class_voltage, 1.0f, 1.0f);

	network_add_layer(net, layer_input, TRUE, FALSE);
	network_add_layer(net, layer_output, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	// create data generator
	DataGenerator* constant_current = data_generator_constant_current_create(1, net, 2.0f, 100);

	// create callbacks
	layer_input = network_get_layer_by_name(net, "layer_in");
	layer_output = network_get_layer_by_name(net, "layer_out");
	Callback* c1 = callback_dump_layer_neurons_create(layer_input, result_dir);
	Callback* c2 = callback_dump_layer_neurons_create(layer_output, result_dir);

	// create simulator
	Simulator* simulator = simulator_create(constant_current, net);
	simulator_add_callback(simulator, c1);
	simulator_add_callback(simulator, c2);

	// run experiments
	simulator_infer(simulator);

	// cleanup 
	simulator_destroy(simulator);
}