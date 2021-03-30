#include "Simulator.h"
#include "data/data_gen_random_spikes.h"
#include "callbacks/callback_dump.h"
#include "../../include/config.h"


void time_between_spikes_experiment() {
	char result_path[256] = { 0 };
	sprintf(result_path, "%s\\\\experiments_nice_plots\\time_between_spikes_experiment\\tau_l1_10_5_n_tau_l2_100_time_between_20", result_base_folder);

	// create network
	Network* net = network_create();
	network_add_neuron_class(net, neuron_class_create("LIF_NEURON", LIF_NEURON));
	network_add_synapse_class(net, synapse_class_create("SYN_20TAU", 0.0f, 100.0f, 1, CONDUCTANCE_SYNAPSE, 1));

	NeuronClass* n_class = network_get_neuron_class_by_name(net, "LIF_NEURON");
	SynapseClass* s_class = network_get_synapse_class_by_name(net, "SYN_20TAU");

	Layer* layer_input = layer_create_fully_connected(5, n_class, "layer_in");
	Layer* layer_output = layer_create_fully_connected(1, n_class, "layer_out");
	layer_add_input_layer(layer_output, layer_input, s_class, 1.0f);

	network_add_layer(net, layer_input, TRUE, FALSE);
	network_add_layer(net, layer_output, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	// create data generator
	DataGenerator* constant_current = data_generator_with_step_between_neurons_create(1, net, 20, 101);

	// create callbacks
	layer_input = network_get_layer_by_name(net, "layer_in");
	layer_output = network_get_layer_by_name(net, "layer_out");
	Callback* c1 = callback_dump_layer_neurons_create(layer_input, result_path, TRUE);
	Callback* c2 = callback_dump_layer_neurons_create(layer_output, result_path, TRUE);

	// create simulator
	Simulator* simulator = simulator_create(constant_current, net);
	simulator_add_callback(simulator, c1);
	simulator_add_callback(simulator, c2);

	// run experiments
	simulator_infer(simulator);

	// cleanup 
	simulator_destroy(simulator);
}