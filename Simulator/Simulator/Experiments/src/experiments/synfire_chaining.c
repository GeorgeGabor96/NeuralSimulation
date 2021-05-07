#include "Simulator.h"
#include "Callback.h"
#include "DataGen.h"
#include "utils/os.h"

#include "../../include/config_exp.h"
#include "../../include/networks.h"


void synfire_chain_space_mapping_connectivity_and_amplitude() {
	char result_path[1024] = { 0 };
	char synfire_result_file[1024] = { 0 };

	// experiment variables
	float connectivity_start = 0.025f;
	float connectivity_end = 1.0f;
	float connectivity_inc = 0.025f;

	float amplitude_start = 0.025f;
	float amplitude_end = 1.0f;
	float amplitude_inc = 0.025f;
	BOOL use_refract = FALSE;

	float min_ratio = 0.5f;
	float max_ratio = 2.0f;

	sprintf(result_path, "%s\\synfire_chain_callback\\r_%u_connect_s_%.4f_e_%.4f_i_%.4f_ampli_s_%.4f_e_%.4f_i_%.4f_min_r_%.2f_max_r_%.2f", 
		result_base_folder,
		use_refract,
		connectivity_start, connectivity_end, connectivity_inc,
		amplitude_start, amplitude_end, amplitude_inc,
		min_ratio, max_ratio);
	os_mkdir(result_path);
	
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
			net = network_synfire_chain_10_layers(connectivity, amplitude, 1.0f, use_refract);

			// create data generator
			data_gen = data_generator_spike_pulses_create(1, net, 10, 2000, 20, 0.0f, 0.05f, 1000);

			// create callbacks
			sprintf(synfire_result_file, "%s\\connectivity_%.4f_amplitude_%.4f.txt", result_path, connectivity, amplitude);
			synfire_detect_cb = callback_detect_synfire_activity_create(net, 200, min_ratio, max_ratio, synfire_result_file);

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
	net = network_synfire_chain_10_layers(connectivity, amplitude, 1.0f, use_refract);

	// create data generator
	data_gen = data_generator_spike_pulses_create(1, net, 10, 2000, 20, 0.0f, 0.05f, 1000);

	// create callbacks
	sprintf(synfire_result_file, "%s\\connectivity_%.4f_amplitude_%.4f.txt", result_path, connectivity, amplitude);
	synfire_detect_cb = callback_detect_synfire_activity_create(net, 200, 0.5f, 1.5f, synfire_result_file);

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