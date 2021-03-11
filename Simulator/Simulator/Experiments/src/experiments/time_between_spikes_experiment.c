#include "Simulator.h"
#include "data/data_gen_random_spikes.h"
#include "callbacks/callback_visualize_layer_neurons.h"


void time_between_spikes_experiment() {
	// create network
	const char* result_path = ".\\experiments_nice_plots\\time_between_spikes_experiment\\tau_l1_10_5_n_tau_l2_20_time_between_20";

	Layer* layer_input = layer_create_fully_connected(5,
		neuron_class_create(LIF_NEURON),
		synapse_class_create(0.0f, 10.0f, 1, CONDUCTANCE_SYNAPSE, 1),
		"layer_in");

	Layer* layer_output = layer_create_fully_connected(1,
		neuron_class_create(LIF_NEURON),
		synapse_class_create(0.0f, 20.0f, 1, CONDUCTANCE_SYNAPSE, 1),
		"layer_out");
	layer_add_input_layer(layer_output, layer_input);

	Network* net = network_create();
	network_add_layer(net, layer_input, TRUE, TRUE, FALSE);
	network_add_layer(net, layer_output, TRUE, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	// create data generator
	DataGenerator* constant_current = data_generator_with_step_between_neurons_create(1, net, 20, 101);

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