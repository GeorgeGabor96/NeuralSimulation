#include "Simulator.h"
#include "data/data_gen_constant_current.h"
#include "callbacks/callback_visualize_layer_neurons.h"
#include "../../include/networks.h"


void constant_current_experiment() {
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);
	SynapseClass* s_class = synapse_class_create_default();

	Network* net = network_3_L_3_3_3(n_class, s_class);
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
	neuron_class_destroy(n_class);
	synapse_class_destroy(s_class);

}