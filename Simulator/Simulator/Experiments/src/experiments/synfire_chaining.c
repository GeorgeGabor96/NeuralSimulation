#include "../../include/config.h"
#include "Simulator.h"
#include "data/data_gen_random_spikes.h"
#include "callbacks/callback_dump.h"
#include "../../include/networks.h"


void synfire_chain_space_mapping_connectivity_and_synaptic_strength() {
	char result_path[512] = { 0 };
	char result_path_cb_dump[512] = { 0 };
	sprintf(result_path, "%s\\\\synfire_chain\\mapping_space_connectivity_and_synaptic_strength", result_base_folder);

	Network* net = NULL;
	DataGenerator* data_gen = NULL;
	Callback* network_dump_cb = NULL;
	Simulator* simulator = NULL;
	float connectivity = 0.0f;
	float s_strength = 0.0f;

	for (connectivity = 0.0f; connectivity <= 1.0f; connectivity += 0.5f) {

		for (s_strength = 0.0f; s_strength <= 1.0f; s_strength += 0.5f) {
			printf("Running with connectivity %f and synaptic strength %f\n", connectivity, s_strength);

			memset(result_path_cb_dump, 0, 512);

			// trebuie o functie ce construieste o retea cu parametrii astia
			net = network_synfire_chain_10_layers(connectivity, s_strength);

			// create data generator
			data_gen = data_generator_spike_pulses_create(1, net, 10, 2000, 10, 0.0f, 0.2f, 1000);

			// create callbacks
			sprintf(result_path_cb_dump, "%s\\connectivity_%.2f_strength_%.2f", result_path, connectivity, s_strength);
			network_dump_cb = callback_dump_network_create(net, result_path_cb_dump);

			// create simulator and run it
			simulator = simulator_create(data_gen, net);
			simulator_add_callback(simulator, network_dump_cb);
			simulator_infer(simulator);

			// cleanup
			simulator_destroy(simulator);
		}
	}
}



void synfire_chain_random_spikes() {
	char result_path[256] = { 0 };
	sprintf(result_path, "%s\\\\synfire_chain\\exp1", result_base_folder);

	// create network
	Network* net = network_create();
	network_add_neuron_class(net, neuron_class_create("LIF_NEURON", LIF_NEURON));
	network_add_synapse_class(net, synapse_class_create("CONDUCTANCE_10_TAU", 0.0, 10, 10, CONDUCTANCE_SYNAPSE, 1));

	// excitation and inhibition
	network_add_synapse_class(net, synapse_class_create("INHIBITORY_10_TAU", -75.0f, 50, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1));
	network_add_synapse_class(net, synapse_class_create("EXCITATORY_10_TAU", 0.0f, 10, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1));

	network_add_neuron_class(net, neuron_class_create("LIF_NEURON_REFRAC", LIF_REFRACTORY_NEURON));

	NeuronClass* LIF_n_class = network_get_neuron_class_by_name(net, "LIF_NEURON");
	NeuronClass* LIF_refrac_n_class = network_get_neuron_class_by_name(net, "LIF_NEURON_REFRAC");
	
	SynapseClass* conductance_s_class = network_get_synapse_class_by_name(net, "CONDUCTANCE_10_TAU");
	SynapseClass* inhibitory_s_class = network_get_synapse_class_by_name(net, "INHIBITORY_10_TAU");
	SynapseClass* excitatory_s_class = network_get_synapse_class_by_name(net, "EXCITATORY_10_TAU");

	// build 10 layers with 100 neurons
	NeuronClass* n_class = LIF_n_class;

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
	float connectivity = 1.0f;
	layer_add_input_layer(layer2, layer1, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer2, layer1_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer2_inh, layer1, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer2_inh, layer1_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer3, layer2, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer3, layer2_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer3_inh, layer2, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer3_inh, layer2_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer4, layer3, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer4, layer3_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer4_inh, layer3, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer4_inh, layer3_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer5, layer4, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer5, layer4_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer5_inh, layer4, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer5_inh, layer4_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer6, layer5, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer6, layer5_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer6_inh, layer5, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer6_inh, layer5_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer7, layer6, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer7, layer6_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer7_inh, layer6, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer7_inh, layer6_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer8, layer7, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer8, layer7_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer8_inh, layer7, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer8_inh, layer7_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer9, layer8, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer9, layer8_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer9_inh, layer8, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer9_inh, layer8_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer10, layer9, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer10, layer9_inh, inhibitory_s_class, connectivity, 1.0f);

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
	DataGenerator* data_gen = data_generator_spike_pulses_create(1, net, 10, 200, 10, 0.0f, 0.2f, 100);

	// create callbacks
	Callback* network_dump = callback_dump_network_create(net, result_path);

	// create simulator and run it
	Simulator* simulator = simulator_create(data_gen, net);
	simulator_add_callback(simulator, network_dump);
	simulator_infer(simulator);

	// cleanup
	simulator_destroy(simulator);
}


void synfire_chain_evolution() {
	char result_path[256] = { 0 };
	sprintf(result_path, "%s\\\\synfire_chain\\10l_50s_inhi_50s_exci", result_base_folder);

	// create network
	Network* net = network_create();
	network_add_neuron_class(net, neuron_class_create("LIF_NEURON", LIF_NEURON));
	network_add_neuron_class(net, neuron_class_create("LIF_REFRACT_NEURON", LIF_REFRACTORY_NEURON));
	network_add_synapse_class(net, synapse_class_create("CONDUCTANCE_SYN", 0.0, 2.0f, 10, CONDUCTANCE_SYNAPSE, 1));
	network_add_synapse_class(net, synapse_class_create("CONDUCTANCE_SYN_2", 0.0, 3.0f, 10, CONDUCTANCE_SYNAPSE, 1));

	network_add_synapse_class(net, synapse_class_create("INHIBITORY_SYN", -75.0f, 0.5f, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1));
	network_add_synapse_class(net, synapse_class_create("EXCITATORY_SYN", 0.0, 0.5f, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1));

	NeuronClass* LIF_n_class = network_get_neuron_class_by_name(net, "LIF_NEURON");
	NeuronClass* LIF_refract_n_class = network_get_neuron_class_by_name(net, "LIF_REFRACT_NEURON");
	SynapseClass* conductance_s_class = network_get_synapse_class_by_name(net, "CONDUCTANCE_SYN");
	SynapseClass* conductance_s_class_2 = network_get_synapse_class_by_name(net, "CONDUCTANCE_SYN_2");
	SynapseClass* inhi_s_class = network_get_synapse_class_by_name(net, "INHIBITORY_SYN");
	SynapseClass* exci_s_class = network_get_synapse_class_by_name(net, "EXCITATORY_SYN");

	// build 10 layers with 100 neurons
	NeuronClass* n_class = LIF_n_class;
	SynapseClass* excitatory_s_class = exci_s_class;
	SynapseClass* inhibitory_s_class = inhi_s_class;

	uint32_t w_exc = 80;
	Layer* layer1 = layer_create_fully_connected(w_exc, n_class, "layer1");
	Layer* layer2 = layer_create_fully_connected(w_exc, n_class, "layer2");
	Layer* layer3 = layer_create_fully_connected(w_exc, n_class, "layer3");
	Layer* layer4 = layer_create_fully_connected(w_exc, n_class, "layer4");
	Layer* layer5 = layer_create_fully_connected(w_exc, n_class, "layer5");
	Layer* layer6 = layer_create_fully_connected(w_exc, n_class, "layer6");
	Layer* layer7 = layer_create_fully_connected(w_exc, n_class, "layer7");
	Layer* layer8 = layer_create_fully_connected(w_exc, n_class, "layer8");
	Layer* layer9 = layer_create_fully_connected(w_exc, n_class, "layer9");
	Layer* layer10 = layer_create_fully_connected(w_exc, n_class, "layer10");

	uint32_t w_inh = 20;
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
	float connectivity = 1.0f;
	layer_add_input_layer(layer2, layer1, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer2, layer1_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer2_inh, layer1, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer2_inh, layer1_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer3, layer2, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer3, layer2_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer3_inh, layer2, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer3_inh, layer2_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer4, layer3, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer4, layer3_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer4_inh, layer3, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer4_inh, layer3_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer5, layer4, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer5, layer4_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer5_inh, layer4, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer5_inh, layer4_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer6, layer5, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer6, layer5_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer6_inh, layer5, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer6_inh, layer5_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer7, layer6, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer7, layer6_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer7_inh, layer6, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer7_inh, layer6_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer8, layer7, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer8, layer7_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer8_inh, layer7, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer8_inh, layer7_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer9, layer8, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer9, layer8_inh, inhibitory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer9_inh, layer8, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer9_inh, layer8_inh, inhibitory_s_class, connectivity, 1.0f);

	layer_add_input_layer(layer10, layer9, excitatory_s_class, connectivity, 1.0f);
	layer_add_input_layer(layer10, layer9_inh, inhibitory_s_class, connectivity, 1.0f);

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
	DataGenerator* data_gen = data_generator_spike_pulses_create(1, net, 100, 200, 30, 0.0f, 0.05f, 1000);

	// create callbacks
	Callback* network_dump = callback_dump_network_create(net, result_path);

	// create simulator and run it
	Simulator* simulator = simulator_create(data_gen, net);
	simulator_add_callback(simulator, network_dump);
	simulator_infer(simulator);

	// cleanup
	simulator_destroy(simulator);
}