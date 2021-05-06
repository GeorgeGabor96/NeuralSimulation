#include "../include/networks.h"


Network* network_3_L_3_3_3() {
	Network* net = network_create();
	NeuronClass* n_class = neuron_class_create("TEST", LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default("TEST");
	network_add_neuron_class(net, n_class);
	network_add_synapse_class(net, s_class);

	Layer* layer_1 = layer_create_fully_connected(3, n_class, "layer_1");
	Layer* layer_2 = layer_create_fully_connected(3, n_class, "layer_2");
	Layer* layer_3 = layer_create_fully_connected(3, n_class, "layer_3");
	layer_add_input_layer(layer_2, layer_1, s_class, 1.0f, 1.0f);
	layer_add_input_layer(layer_3, layer_2, s_class, 1.0f, 1.0f);
	
	network_add_layer(net, layer_1, TRUE, FALSE);
	network_add_layer(net, layer_2, FALSE, FALSE);
	network_add_layer(net, layer_3, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	return net;
}


Network* network_synfire_chain_10_layers(float connectivity, float s_strength) {
	// create network
	Network* net = network_create();
	network_add_neuron_class(net, neuron_class_create("LIF_NEURON", LIF_NEURON));
	network_add_neuron_class(net, neuron_class_create("LIF_NEURON_REFRAC", LIF_REFRACTORY_NEURON));

	// excitation and inhibition
	network_add_synapse_class(net, synapse_class_create("GABA_A", -90.0f, 6, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1));
	network_add_synapse_class(net, synapse_class_create("AMPA", 0.0f, 3, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1));
	network_add_synapse_class(net, synapse_class_create("CONDUCTANCE_10_TAU", 0.0, 10, 10, CONDUCTANCE_SYNAPSE, 1));


	NeuronClass* LIF_n_class = network_get_neuron_class_by_name(net, "LIF_NEURON");
	NeuronClass* LIF_refrac_n_class = network_get_neuron_class_by_name(net, "LIF_NEURON_REFRAC");

	SynapseClass* conductance_s_class = network_get_synapse_class_by_name(net, "CONDUCTANCE_10_TAU");
	SynapseClass* inhibitory_s_class = network_get_synapse_class_by_name(net, "GABA_A");
	SynapseClass* excitatory_s_class = network_get_synapse_class_by_name(net, "AMPA");

	// build 10 layers with 100 neurons
	NeuronClass* n_class = LIF_n_class;

	uint32_t w = 70;
	Layer* layer1 = layer_create_fully_connected(w, n_class, "layer01");
	Layer* layer2 = layer_create_fully_connected(w, n_class, "layer02");
	Layer* layer3 = layer_create_fully_connected(w, n_class, "layer03");
	Layer* layer4 = layer_create_fully_connected(w, n_class, "layer04");
	Layer* layer5 = layer_create_fully_connected(w, n_class, "layer05");
	Layer* layer6 = layer_create_fully_connected(w, n_class, "layer06");
	Layer* layer7 = layer_create_fully_connected(w, n_class, "layer07");
	Layer* layer8 = layer_create_fully_connected(w, n_class, "layer08");
	Layer* layer9 = layer_create_fully_connected(w, n_class, "layer09");
	Layer* layer10 = layer_create_fully_connected(w, n_class, "layer10");

	uint32_t w_inh = 30;
	Layer* layer1_inh = layer_create_fully_connected(w_inh, n_class, "layer01_inh");
	Layer* layer2_inh = layer_create_fully_connected(w_inh, n_class, "layer02_inh");
	Layer* layer3_inh = layer_create_fully_connected(w_inh, n_class, "layer03_inh");
	Layer* layer4_inh = layer_create_fully_connected(w_inh, n_class, "layer04_inh");
	Layer* layer5_inh = layer_create_fully_connected(w_inh, n_class, "layer05_inh");
	Layer* layer6_inh = layer_create_fully_connected(w_inh, n_class, "layer06_inh");
	Layer* layer7_inh = layer_create_fully_connected(w_inh, n_class, "layer07_inh");
	Layer* layer8_inh = layer_create_fully_connected(w_inh, n_class, "layer08_inh");
	Layer* layer9_inh = layer_create_fully_connected(w_inh, n_class, "layer09_inh");

	// link the neurons
	layer_add_input_layer(layer2, layer1, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer2, layer1_inh, inhibitory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer2_inh, layer1, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer2_inh, layer1_inh, inhibitory_s_class, connectivity, s_strength);

	layer_add_input_layer(layer3, layer2, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer3, layer2_inh, inhibitory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer3_inh, layer2, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer3_inh, layer2_inh, inhibitory_s_class, connectivity, s_strength);

	layer_add_input_layer(layer4, layer3, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer4, layer3_inh, inhibitory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer4_inh, layer3, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer4_inh, layer3_inh, inhibitory_s_class, connectivity, s_strength);

	layer_add_input_layer(layer5, layer4, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer5, layer4_inh, inhibitory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer5_inh, layer4, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer5_inh, layer4_inh, inhibitory_s_class, connectivity, s_strength);

	layer_add_input_layer(layer6, layer5, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer6, layer5_inh, inhibitory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer6_inh, layer5, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer6_inh, layer5_inh, inhibitory_s_class, connectivity, s_strength);

	layer_add_input_layer(layer7, layer6, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer7, layer6_inh, inhibitory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer7_inh, layer6, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer7_inh, layer6_inh, inhibitory_s_class, connectivity, s_strength);

	layer_add_input_layer(layer8, layer7, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer8, layer7_inh, inhibitory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer8_inh, layer7, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer8_inh, layer7_inh, inhibitory_s_class, connectivity, s_strength);

	layer_add_input_layer(layer9, layer8, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer9, layer8_inh, inhibitory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer9_inh, layer8, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer9_inh, layer8_inh, inhibitory_s_class, connectivity, s_strength);

	layer_add_input_layer(layer10, layer9, excitatory_s_class, connectivity, s_strength);
	layer_add_input_layer(layer10, layer9_inh, inhibitory_s_class, connectivity, s_strength);

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

	return net;
}