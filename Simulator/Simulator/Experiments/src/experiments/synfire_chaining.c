#include "../../include/config.h"
#include "Simulator.h"
#include "data/data_gen_random_spikes.h"
#include "callbacks/callback_dump_layer_neurons.h"


void synfire_chain_random_spikes() {
	char result_path[256] = { 0 };
	sprintf(result_path, "%s\\\\experiments_nice_plots\\synfire_chain\\pulse_spikes\\60n_ext_40n_inh_0_5_coonectivity_refrac_5_PSC_0_14_100ms", result_base_folder);

	// create network
	Network* net = network_create();
	network_add_neuron_class(net, neuron_class_create("LIF_NEURON", LIF_NEURON));
	network_add_synapse_class(net, synapse_class_create("CONDUCTANCE_10_TAU", 0.0, 10, 10, CONDUCTANCE_SYNAPSE, 1));

	// excitation and inhibition
	network_add_synapse_class(net, synapse_class_create("INHIBITORY_10_TAU", -75.0f, 50, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1));
	network_add_synapse_class(net, synapse_class_create("EXCITATORY_10_TAU", 0.0f, 10, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1));

	network_add_neuron_class(net, neuron_class_create("LIF_NEURON_REFRAC", LIF_REFRACTORY_NEURON));

	NeuronClass* n_class = network_get_neuron_class_by_name(net, "LIF_NEURON_REFRAC");
	SynapseClass* s_class = network_get_synapse_class_by_name(net, "CONDUCTANCE_10_TAU");

	SynapseClass* inhibitory_s_class = network_get_synapse_class_by_name(net, "INHIBITORY_10_TAU");
	SynapseClass* excitatory_s_class = network_get_synapse_class_by_name(net, "EXCITATORY_10_TAU");

	// build 10 layers with 100 neurons
	uint32_t w = 70;
	Layer* layer1 = layer_create_fully_connected(w, n_class, "layer1");
	Layer* layer2 = layer_create_fully_connected(w, n_class, "layer2");
	Layer* layer3 = layer_create_fully_connected(w, n_class, "layer3");
	Layer* layer4 = layer_create_fully_connected(w, n_class, "layer4");
	Layer* layer5 = layer_create_fully_connected(w, n_class, "layer5");
	Layer* layer6 = layer_create_fully_connected(w, n_class, "layer6");
	Layer* layer7 = layer_create_fully_connected(w, n_class, "layer7");
	Layer* layer8 = layer_create_fully_connected(w, n_class, "layer8");
	Layer* layer9 = layer_create_fully_connected(w, n_class, "layer9");
	Layer* layer10 = layer_create_fully_connected(w, n_class, "layer10");
	
	uint32_t w_inh = 30;
	Layer* layer1_inh = layer_create_fully_connected(w_inh, n_class, "layer1_inh");
	Layer* layer2_inh = layer_create_fully_connected(w_inh, n_class, "layer2_inh");
	Layer* layer3_inh = layer_create_fully_connected(w_inh, n_class, "layer3_inh");
	Layer* layer4_inh = layer_create_fully_connected(w_inh, n_class, "layer4_inh");
	Layer* layer5_inh = layer_create_fully_connected(w_inh, n_class, "layer5_inh");
	Layer* layer6_inh = layer_create_fully_connected(w_inh, n_class, "layer6_inh");
	Layer* layer7_inh = layer_create_fully_connected(w_inh, n_class, "layer7_inh");
	Layer* layer8_inh = layer_create_fully_connected(w_inh, n_class, "layer8_inh");
	Layer* layer9_inh = layer_create_fully_connected(w_inh, n_class, "layer9_inh");

	// link the neurons
	layer_add_input_layer(layer2, layer1, excitatory_s_class);
	layer_add_input_layer(layer2, layer1_inh, inhibitory_s_class);
	layer_add_input_layer(layer2_inh, layer1, excitatory_s_class);
	layer_add_input_layer(layer2_inh, layer1_inh, inhibitory_s_class);

	layer_add_input_layer(layer3, layer2, excitatory_s_class);
	layer_add_input_layer(layer3, layer2_inh, inhibitory_s_class);
	layer_add_input_layer(layer3_inh, layer2, excitatory_s_class);
	layer_add_input_layer(layer3_inh, layer2_inh, inhibitory_s_class);

	layer_add_input_layer(layer4, layer3, excitatory_s_class);
	layer_add_input_layer(layer4, layer3_inh, inhibitory_s_class);
	layer_add_input_layer(layer4_inh, layer3, excitatory_s_class);
	layer_add_input_layer(layer4_inh, layer3_inh, inhibitory_s_class);

	layer_add_input_layer(layer5, layer4, excitatory_s_class);
	layer_add_input_layer(layer5, layer4_inh, inhibitory_s_class);
	layer_add_input_layer(layer5_inh, layer4, excitatory_s_class);
	layer_add_input_layer(layer5_inh, layer4_inh, inhibitory_s_class);

	layer_add_input_layer(layer6, layer5, excitatory_s_class);
	layer_add_input_layer(layer6, layer5_inh, inhibitory_s_class);
	layer_add_input_layer(layer6_inh, layer5, excitatory_s_class);
	layer_add_input_layer(layer6_inh, layer5_inh, inhibitory_s_class);

	layer_add_input_layer(layer7, layer6, excitatory_s_class);
	layer_add_input_layer(layer7, layer6_inh, inhibitory_s_class);
	layer_add_input_layer(layer7_inh, layer6, excitatory_s_class);
	layer_add_input_layer(layer7_inh, layer6_inh, inhibitory_s_class);

	layer_add_input_layer(layer8, layer7, excitatory_s_class);
	layer_add_input_layer(layer8, layer7_inh, inhibitory_s_class);
	layer_add_input_layer(layer8_inh, layer7, excitatory_s_class);
	layer_add_input_layer(layer8_inh, layer7_inh, inhibitory_s_class);

	layer_add_input_layer(layer9, layer8, excitatory_s_class);
	layer_add_input_layer(layer9, layer8_inh, inhibitory_s_class);
	layer_add_input_layer(layer9_inh, layer8, excitatory_s_class);
	layer_add_input_layer(layer9_inh, layer8_inh, inhibitory_s_class);

	layer_add_input_layer(layer10, layer9, excitatory_s_class);
	layer_add_input_layer(layer10, layer9_inh, inhibitory_s_class);

	// add layers to network
	network_add_layer(net, layer1, TRUE, FALSE);
	network_add_layer(net, layer1_inh, TRUE, FALSE);

	network_add_layer(net, layer2, FALSE, FALSE);
	network_add_layer(net, layer2_inh, FALSE, FALSE);

	network_add_layer(net, layer3, FALSE, FALSE);
	network_add_layer(net, layer3_inh, FALSE, FALSE);

	network_add_layer(net, layer4, FALSE, FALSE);
	network_add_layer(net, layer4_inh, FALSE, FALSE);

	network_add_layer(net, layer5, FALSE, FALSE);
	network_add_layer(net, layer5_inh, FALSE, FALSE);

	network_add_layer(net, layer6, FALSE, FALSE);
	network_add_layer(net, layer6_inh, FALSE, FALSE);

	network_add_layer(net, layer7, FALSE, FALSE);
	network_add_layer(net, layer7_inh, FALSE, FALSE);

	network_add_layer(net, layer8, FALSE, FALSE);
	network_add_layer(net, layer8_inh, FALSE, FALSE);

	network_add_layer(net, layer9, FALSE, FALSE);
	network_add_layer(net, layer9_inh, FALSE, FALSE);

	network_add_layer(net, layer10, FALSE, TRUE);

	// compile and show the network
	network_compile(net);
	network_summary(net);

	// create data generator
	//DataGenerator* data_gen = data_generator_random_spikes_create(1, net, 0.001f, 1000);
	DataGenerator* data_gen = data_generator_spike_pulses_create(1, net, 10, 200, 10, -1.0f, 0.2f, 100);

	// create callbacks
	layer1 = network_get_layer_by_name(net, "layer1");
	layer2 = network_get_layer_by_name(net, "layer2");
	layer3 = network_get_layer_by_name(net, "layer3");
	layer4 = network_get_layer_by_name(net, "layer4");
	layer5 = network_get_layer_by_name(net, "layer5");
	layer6 = network_get_layer_by_name(net, "layer6");
	layer7 = network_get_layer_by_name(net, "layer7");
	layer8 = network_get_layer_by_name(net, "layer8");
	layer9 = network_get_layer_by_name(net, "layer9");
	layer10 = network_get_layer_by_name(net, "layer10");
	Callback* dump_layer1 = callback_dump_layer_neurons_create(layer1, result_path, FALSE);
	Callback* dump_layer2 = callback_dump_layer_neurons_create(layer2, result_path, FALSE);
	Callback* dump_layer3 = callback_dump_layer_neurons_create(layer3, result_path, FALSE);
	Callback* dump_layer4 = callback_dump_layer_neurons_create(layer4, result_path, FALSE);
	Callback* dump_layer5 = callback_dump_layer_neurons_create(layer5, result_path, FALSE);
	Callback* dump_layer6 = callback_dump_layer_neurons_create(layer6, result_path, FALSE);
	Callback* dump_layer7 = callback_dump_layer_neurons_create(layer7, result_path, FALSE);
	Callback* dump_layer8 = callback_dump_layer_neurons_create(layer8, result_path, FALSE);
	Callback* dump_layer9 = callback_dump_layer_neurons_create(layer9, result_path, FALSE);
	Callback* dump_layer10 = callback_dump_layer_neurons_create(layer10, result_path, FALSE);

	// create simulator and run it
	Simulator* simulator = simulator_create(data_gen, net);
	simulator_add_callback(simulator, dump_layer1);
	simulator_add_callback(simulator, dump_layer2);
	simulator_add_callback(simulator, dump_layer3);
	simulator_add_callback(simulator, dump_layer4);
	simulator_add_callback(simulator, dump_layer5);
	simulator_add_callback(simulator, dump_layer6);
	simulator_add_callback(simulator, dump_layer7);
	simulator_add_callback(simulator, dump_layer8);
	simulator_add_callback(simulator, dump_layer9);
	simulator_add_callback(simulator, dump_layer10);
	simulator_infer(simulator);

	// cleanup
	simulator_destroy(simulator);
}