#include "../include/networks.h"


Network* network_3_L_3_3_3(NeuronClass* n_class, SynapseClass* s_class) {
	Layer* layer_1 = layer_create_fully_connected(3, n_class, s_class, "layer_1");
	Layer* layer_2 = layer_create_fully_connected(3, n_class, s_class, "layer_2");
	Layer* layer_3 = layer_create_fully_connected(3, n_class, s_class, "layer_3");
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