#include "Simulator.h"
#include "data/data_gen_random_spikes.h"
#include "callbacks/callback_dump_layer_neurons.h"
#include "../../include/networks.h"
#include "../../include/config.h"


void random_spikes_experiment() {
	char result_dir[256] = { 0 };
	sprintf(result_dir, "%s\\random_spikes_exp2", result_base_folder);

	Network* net = network_3_L_3_3_3();
	DataGenerator* constant_current = data_generator_random_spikes_create(1, net, 0.5f, 100);
	Layer* l1 = network_get_layer_by_idx(net, 0);
	Layer* l2 = network_get_layer_by_idx(net, 1);
	Layer* l3 = network_get_layer_by_idx(net, 2);
	Callback* visu_1 = callback_dump_layer_neurons_create(l1, result_dir, TRUE);
	Callback* visu_2 = callback_dump_layer_neurons_create(l2, result_dir, TRUE);
	Callback* visu_3 = callback_dump_layer_neurons_create(l3, result_dir, TRUE);

	Simulator* simulator = simulator_create(constant_current, net);
	simulator_add_callback(simulator, visu_1);
	simulator_add_callback(simulator, visu_2);
	simulator_add_callback(simulator, visu_3);

	simulator_infer(simulator);

	simulator_destroy(simulator);
}