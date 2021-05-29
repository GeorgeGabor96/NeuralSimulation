#include "Simulator.h"
#include "Callback.h"
#include "DataGen.h"
#include "utils/os.h"

#include "../../include/config_exp.h"
#include "../../include/networks.h"
#include "../../include/experiments.h"


typedef struct connectivity_amplitude_space_exp {
	const char* exp_abs_path;

	float connectivity_start;
	float connectivity_end;
	float connectivity_inc;

	float amplitude_start;
	float amplitude_end;
	float amplitude_inc;

	float min_ratio;
	float max_ratio;

	uint32_t n_excitatory;
	uint32_t n_inhibitory;

	uint32_t n_trials;

	NeuronClass* neuron_class;
	SynapseClass* synapse_exci_class;
	SynapseClass* synapse_inhi_class;

	// data gen values
	uint32_t example_duration;
	uint32_t pulse_duration;
	float between_pulse_spike_frequency;
	float pulse_spike_frequency;

	// callback values;
	BOOL use_dump_net_callback;
	BOOL use_synfire_ring;

}connectivity_amplitude_space_exp;


void dump_config(const char* file_path, connectivity_amplitude_space_exp* config);
void synfire_space_exploration_connectivity_amplitude_run_config(connectivity_amplitude_space_exp* config);


void synfire_space_exploration_connectivity_amplitude() {
	connectivity_amplitude_space_exp config = { 0 };
	config.exp_abs_path = "d:\\repositories\\Simulator\\experiments\\synfire_rings\\exp1_refract_sfort_multiple_input_pulses_100";
	
	config.connectivity_start = 0.3f;
	config.connectivity_end = 0.3f;
	config.connectivity_inc = 0.025f;

	config.amplitude_start = 0.3f;
	config.amplitude_end = 0.5f;
	config.amplitude_inc = 0.025f;

	config.min_ratio = 0.5f;
	config.max_ratio = 2.0f;

	config.n_excitatory = 80;
	config.n_inhibitory = 20;

	//config.neuron_class = neuron_class_create("LIF_NEURON", LIF_NEURON);
	config.neuron_class = neuron_class_create("LIF_NEURON_REFRAC", LIF_REFRACTORY_NEURON);

	config.synapse_exci_class = synapse_class_create("AMPA", 0.0f, 1.0f, 1, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	config.synapse_inhi_class = synapse_class_create("GABA_A", -90.0f, 1.0f, 6, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	//config.synapse_exci_class = synapse_class_create("CONDUCTANCE_10_TAU", 0.0, 1.0f, 10, 10, CONDUCTANCE_SYNAPSE, 1);
	//config.synapse_inhi_class = synapse_class_create("CONDUCTANCE_10_TAU", 0.0, 1.0f, 10, 10, CONDUCTANCE_SYNAPSE, 1);

	config.n_trials = 1;

	config.example_duration = 1000;
	config.pulse_duration = 20;
	config.between_pulse_spike_frequency = 0.0f;
	config.pulse_spike_frequency = 0.05f;

	config.use_dump_net_callback = TRUE;
	config.use_synfire_ring = TRUE;

	synfire_space_exploration_connectivity_amplitude_run_config(&config);
}


void synfire_space_exploration_connectivity_amplitude_run_config(connectivity_amplitude_space_exp* config) {
	char result_trial_folder[1024] = { 0 };
	char callback_result_folder[1024] = { 0 };
	uint32_t i = 0;
	float connectivity = 0.0f;
	float amplitude = 0.0f;
	Network* net = NULL;
	DataGenerator* data_gen = NULL;
	Callback* synfire_detect_cb = NULL;
	Callback* net_dump_cb = NULL;
	Simulator* simulator = NULL;
	network_sequential_n_layers_config net_config = { 0 };

	os_mkdir(config->exp_abs_path);
	sprintf(result_trial_folder, "%s\\config.txt", config->exp_abs_path);
	dump_config(result_trial_folder, config);

	for (i = 0; i < config->n_trials; ++i) {
		memset(result_trial_folder, 0, 1024);
		sprintf(result_trial_folder, "%s\\t%d", config->exp_abs_path, i);
		os_mkdir(result_trial_folder);
		
		for (connectivity = config->connectivity_start; connectivity < config->connectivity_end + EPSILON; connectivity += config->connectivity_inc) {

			for (amplitude = config->amplitude_start; amplitude < config->amplitude_end + EPSILON; amplitude += config->amplitude_inc) {
				printf("Running with connectivity %f and amplitude %f\n", connectivity, amplitude);

				// trebuie o functie ce construieste o retea cu parametrii astia
				net_config.n_layers = 10;
				net_config.n_exci_neurons = config->n_excitatory;
				net_config.n_inhi_neurons = config->n_inhibitory;
				net_config.connectivity = connectivity;
				net_config.synapse_weight = 1.0f;
				net_config.n_class = neuron_class_copy(config->neuron_class);
				net_config.s_exci_class = synapse_class_copy(config->synapse_exci_class);
				net_config.s_exci_class->A = amplitude;
				net_config.s_inhi_class = synapse_class_copy(config->synapse_inhi_class);
				net_config.s_inhi_class->A = amplitude;
				// choose between regular network and ring network
				if (config->use_synfire_ring == FALSE) net = network_sequential_n_layers(&net_config);
				else net = network_sequential_ring_n_layers(&net_config);

				// create data generator
				data_gen = data_generator_spike_pulses_create(1, net, 10, 90, config->pulse_duration, config->between_pulse_spike_frequency, config->pulse_spike_frequency, config->example_duration);

				// create callbacks
				memset(callback_result_folder, 0, 1024);
				sprintf(callback_result_folder, "%s\\connectivity_%.4f_amplitude_%.4f.txt", result_trial_folder, connectivity, amplitude);
				synfire_detect_cb = callback_detect_synfire_activity_create(net, SYNFIRE_FP_DURATION, 4, net->layers.length - 1, config->min_ratio, config->max_ratio, callback_result_folder);

				if (config->use_dump_net_callback == TRUE) {
					memset(callback_result_folder, 0, 1024);
					sprintf(callback_result_folder, "%s\\coonectivity_%.4f_amplitude_%.4f", result_trial_folder, connectivity, amplitude);
					net_dump_cb = callback_dump_network_create(net, callback_result_folder);
				}
				// create simulator and run it
				simulator = simulator_create(data_gen, net);
				simulator_add_callback(simulator, synfire_detect_cb);
				if (config->use_dump_net_callback == TRUE) simulator_add_callback(simulator, net_dump_cb);
				simulator_infer(simulator);

				// cleanup
				simulator_destroy(simulator);
			}
		}
	}
}


void dump_config(const char* file_path, connectivity_amplitude_space_exp* config) {
	FILE* fp = fopen(file_path, "w");
	if (fp == NULL) {
		fprintf(stderr, "!!!!!Couldn't open %s for writting\n", file_path);
		return;
	}
	String* n_class_desc = neuron_class_get_desc(config->neuron_class);
	String* s_exci_class_desc = synapse_class_get_desc(config->synapse_exci_class);
	String* s_inhi_class_desc = synapse_class_get_desc(config->synapse_inhi_class);

	fprintf(fp, "exp_abs_path: %s\n\n"
		"connectivity_start: %f\n"
		"connectivity_end: %f\n"
		"connectivity_inc: %f\n\n"
		"amplitude_start: %f\n"
		"amplitude_end: %f\n"
		"amplitude_inc: %f\n\n"
		"min_ratio: %f\n"
		"max_ratio: %f\n"
		"n_excitatory: %u\n"
		"n_inhibitory: %u\n"
		"n_trials: %u\n\n"
		"neuron_class: %s\n"
		"synapse_exci_class: %s\n"
		"synapse_inhi_class: %s\n\n"
		"example_duration: %u\n"
		"pulse_duration: %u\n"
		"between_pulse_spike_frequency: %f\n"
		"pulse_spike_frequency: %f\n\n"
		"use_dump_net_callback: %d",
		config->exp_abs_path,
		config->connectivity_start, config->connectivity_end, config->connectivity_inc,
		config->amplitude_start, config->amplitude_end, config->amplitude_inc,
		config->min_ratio, config->max_ratio,
		config->n_excitatory, config->n_inhibitory, config->n_trials,
		string_get_C_string(n_class_desc), string_get_C_string(s_exci_class_desc), string_get_C_string(s_inhi_class_desc),
		config->example_duration, config->pulse_duration, config->between_pulse_spike_frequency, config->pulse_spike_frequency,
		config->use_dump_net_callback
	);
	fclose(fp);

	string_destroy(n_class_desc);
	string_destroy(s_exci_class_desc);
	string_destroy(s_inhi_class_desc);
}