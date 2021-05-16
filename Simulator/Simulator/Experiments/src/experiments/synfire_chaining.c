#include "Simulator.h"
#include "Callback.h"
#include "DataGen.h"
#include "utils/os.h"

#include "../../include/config_exp.h"
#include "../../include/networks.h"
#include "../../include/experiments.h"


void synfire_space_exploration_connectivity_amplitude_multiple(uint32_t n_trials) {
	connectivity_amplitude_space_exp config = { 0 };
	config.connectivity_start = 0.025f;
	config.connectivity_end = 1.0f;
	config.connectivity_inc = 0.025f;

	config.amplitude_start = 0.005f;
	config.amplitude_end = 0.450f;
	config.amplitude_inc = 0.010f;

	config.use_refract = FALSE;
	config.min_ratio = 0.5f;
	config.max_ratio = 2.0f;

	char result_folder[1024] = { 0 };
	char result_trail_folder[1024] = { 0 };
	sprintf(result_folder, "%s\\synfire_chain_hist\\r_%u_connect_s_%.4f_e_%.4f_i_%.4f_ampli_s_%.4f_e_%.4f_i_%.4f_min_r_%.2f_max_r_%.2f",
		result_base_folder,
		config.use_refract,
		config.connectivity_start, config.connectivity_end, config.connectivity_inc,
		config.amplitude_start, config.amplitude_end, config.amplitude_inc,
		config.min_ratio, config.max_ratio);


	for (uint32_t i = 0; i < n_trials; ++i) {
		memset(result_trail_folder, 0, 1024);
		sprintf(result_trail_folder, "%s\\t%d", result_folder, i);
		os_mkdir(result_trail_folder);
		synfire_chain_space_mapping_connectivity_and_amplitude(&config, result_trail_folder);
	}
}


void synfire_chain_space_mapping_connectivity_and_amplitude(connectivity_amplitude_space_exp* config, const char* result_path) {
	char synfire_result_file[1024] = { 0 };

	// experiment variables
	float connectivity_start = config->connectivity_start;
	float connectivity_end = config->connectivity_end;
	float connectivity_inc = config->connectivity_inc;

	float amplitude_start = config->amplitude_start;
	float amplitude_end = config->amplitude_end;
	float amplitude_inc = config->amplitude_inc;
	BOOL use_refract = config->use_refract;

	float min_ratio = config->min_ratio;
	float max_ratio = config->max_ratio;
	
	Network* net = NULL;
	DataGenerator* data_gen = NULL;
	Callback* synfire_detect_cb = NULL;
	Simulator* simulator = NULL;
	float connectivity = 0.0f;
	float amplitude = 0.0f;

	for (connectivity = connectivity_start; connectivity < connectivity_end + 0.00001f; connectivity += connectivity_inc) {

		for (amplitude = amplitude_start; amplitude < amplitude_end + 0.00001f; amplitude += amplitude_inc) {
			printf("Running with connectivity %f and amplitude %f\n", connectivity, amplitude);

			memset(synfire_result_file, 0, 1024);

			// trebuie o functie ce construieste o retea cu parametrii astia
			net = network_synfire_chain_n_layers(10, connectivity, amplitude, 1.0f, use_refract);

			// create data generator
			data_gen = data_generator_spike_pulses_create(1, net, 10, 2000, 20, 0.0f, 0.05f, 500);

			// create callbacks
			sprintf(synfire_result_file, "%s\\connectivity_%.4f_amplitude_%.4f.txt", result_path, connectivity, amplitude);
			synfire_detect_cb = callback_detect_synfire_activity_create(net, SYNFIRE_FP_DURATION, 4, net->layers.length - 1, min_ratio, max_ratio, synfire_result_file);

			// create simulator and run it
			simulator = simulator_create(data_gen, net);
			simulator_add_callback(simulator, synfire_detect_cb);
			simulator_infer(simulator);

			// cleanup
			simulator_destroy(simulator);
		}
	}
}


void synfire_chain_space_mapping_connectivity_and_amplitude_specific(float connectivity, float amplitude, BOOL use_refract) {
	char result_path[1024] = { 0 };
	char synfire_result_file[1024] = { 0 };
	char net_dump_result_folder[1024] = { 0 };

	sprintf(result_path, "%s\\synfire_chain_callback\\r_%u_connectivity_%.4f_amplitude_%.4f",
		result_base_folder,
		use_refract,
		connectivity,
		amplitude);
	os_mkdir(result_path);

	Network* net = NULL;
	DataGenerator* data_gen = NULL;
	Callback* synfire_detect_cb = NULL;
	Callback* net_dump_cb = NULL;
	Simulator* simulator = NULL;

	printf("Running with connectivity %f and amplitude %f\n", connectivity, amplitude);

	// trebuie o functie ce construieste o retea cu parametrii astia
	net = network_synfire_chain_n_layers(10, connectivity, amplitude, 1.0f, use_refract);

	// create data generator
	data_gen = data_generator_spike_pulses_create(1, net, 10, 2000, 20, 0.01f, 0.05f, 1000);

	// create callbacks
	sprintf(synfire_result_file, "%s\\connectivity_%.4f_amplitude_%.4f.txt", result_path, connectivity, amplitude);
	synfire_detect_cb = callback_detect_synfire_activity_create(net, SYNFIRE_FP_DURATION, 4, net->layers.length - 1, 0.5f, 2.0f, synfire_result_file);

	sprintf(net_dump_result_folder, "%s\\coonectivity_%.4f_amplitude_%.4f", result_path, connectivity, amplitude);
	net_dump_cb = callback_dump_network_create(net, net_dump_result_folder);

	// create simulator and run it
	simulator = simulator_create(data_gen, net);
	simulator_add_callback(simulator, synfire_detect_cb);
	simulator_add_callback(simulator, net_dump_cb);
	simulator_infer(simulator);

	// cleanup
	simulator_destroy(simulator);
}