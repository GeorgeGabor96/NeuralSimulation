#include "../include/networks.h"


Network* network_3_L_3_3_3() {
	Layer* layer_1 = layer_create_fully_connected(3, neuron_class_create(LIF_NEURON), synapse_class_create_default(), "layer_1");
	Layer* layer_2 = layer_create_fully_connected(3, neuron_class_create(LIF_NEURON), synapse_class_create_default(), "layer_2");
	Layer* layer_3 = layer_create_fully_connected(3, neuron_class_create(LIF_NEURON), synapse_class_create_default(), "layer_3");
	layer_add_input_layer(layer_2, layer_1);
	layer_add_input_layer(layer_3, layer_2);
	
	Network* net = network_create();
	network_add_layer(net, layer_1, TRUE, TRUE, FALSE);
	network_add_layer(net, layer_2, TRUE, FALSE, FALSE);
	network_add_layer(net, layer_3, TRUE, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	return net;
}