#include "../../include/config.h"
#include "Simulator.h"
#include "data/data_gen_random_spikes.h"
#include "callbacks/callback_dump.h"
#include "../../include/networks.h"


void synfire_chain_space_mapping_connectivity_and_synaptic_strength() {
	char result_path[1024] = { 0 };
	char synfire_result_file[1024] = { 0 };
	sprintf(result_path, "%s\\synfire_chain_callback\\refract_connectivity_0_025_to_1_and_synaptic_strength_0_025_to_0_1_v3", result_base_folder);
	
	Network* net = NULL;
	DataGenerator* data_gen = NULL;
	Callback* synfire_detect_cb = NULL;
	Simulator* simulator = NULL;
	float connectivity = 0.0f;
	float s_strength = 0.0f;

	for (connectivity = 0.025f; connectivity < 1.01f; connectivity += 0.1f) {

		for (s_strength = 0.025f; s_strength < 0.101f; s_strength += 0.025f) {
			printf("Running with connectivity %f and synaptic strength %f\n", connectivity, s_strength);

			memset(synfire_result_file, 0, 1024);

			// trebuie o functie ce construieste o retea cu parametrii astia
			net = network_synfire_chain_10_layers(connectivity, s_strength);

			// create data generator
			data_gen = data_generator_spike_pulses_create(1, net, 10, 2000, 20, 0.0f, 0.05f, 1000);

			// create callbacks
			sprintf(synfire_result_file, "%s\\connectivity_%.4f_strength_%.4f.txt", result_path, connectivity, s_strength);
			synfire_detect_cb = callback_detect_synfire_activity_create(net, 200, 0.5f, 1.5f, synfire_result_file);

			// create simulator and run it
			simulator = simulator_create(data_gen, net);
			simulator_add_callback(simulator, synfire_detect_cb);
			simulator_infer(simulator);

			// cleanup
			simulator_destroy(simulator);
		}
	}
}
