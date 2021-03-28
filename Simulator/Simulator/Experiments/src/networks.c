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
	layer_add_input_layer(layer_2, layer_1, s_class, 1.0f);
	layer_add_input_layer(layer_3, layer_2, s_class, 1.0f);
	
	network_add_layer(net, layer_1, TRUE, FALSE);
	network_add_layer(net, layer_2, FALSE, FALSE);
	network_add_layer(net, layer_3, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	return net;
}