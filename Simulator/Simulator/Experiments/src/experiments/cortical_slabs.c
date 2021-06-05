#include "Simulator.h"
#include "Callback.h"
#include "DataGen.h"
#include "utils/os.h"

#include "../../include/config_exp.h"
#include "../../include/networks.h"
#include "../../include/experiments.h"


typedef struct cortical_slabs_exp_config {
	const char* exp_abs_path;

	uint32_t n_layers;
	uint32_t* n_neurons_layer_1_steps;
	uint32_t n_neurons_layer_1_cnt;
	uint32_t n_neurons_layer_2;
	float connectivity;
	float amplitude;

	float percent_excitatory;
	float percent_inhibitory;

	NeuronClass* neuron_class;
	SynapseClass* synapse_exci_class;
	SynapseClass* synapse_inhi_class;

	// data gen values
	uint32_t example_duration;
	float spike_frequency;

	// callback values;
	BOOL use_dump_net_callback;

}cortical_slabs_exp_config;


void cortical_slabs_exp_dump_config(const char* file_path, cortical_slabs_exp_config* config);
void cortical_slabs_exp_run_config(cortical_slabs_exp_config* config);


void cortical_slabs_exp_run() {
	cortical_slabs_exp_config config = { 0 };
	config.exp_abs_path = "d:\\repositories\\Simulator\\experiments\\cortical_slabs\\final";

	config.n_layers = 3;
	config.n_neurons_layer_1_cnt = 13;
	config.n_neurons_layer_1_steps = malloc(sizeof(uint32_t) * config.n_neurons_layer_1_cnt);
	config.n_neurons_layer_1_steps[0] = 100;
	config.n_neurons_layer_1_steps[1] = 150;
	config.n_neurons_layer_1_steps[2] = 250;
	config.n_neurons_layer_1_steps[3] = 500;
	config.n_neurons_layer_1_steps[4] = 1000;
	config.n_neurons_layer_1_steps[5] = 1500;
	config.n_neurons_layer_1_steps[6] = 2000;
	config.n_neurons_layer_1_steps[7] = 3000;
	config.n_neurons_layer_1_steps[8] = 4000;
	config.n_neurons_layer_1_steps[9] = 5000;
	config.n_neurons_layer_1_steps[10] = 6000;
	config.n_neurons_layer_1_steps[11] = 7000;
	config.n_neurons_layer_1_steps[12] = 8000;
	config.n_neurons_layer_2 = 100;
	config.percent_excitatory = 0.8f;
	config.percent_inhibitory = 0.2f;
	
	// for refract
	//config.connectivity = 0.3f;
	//config.amplitude = 0.3f;

	// for non-refract
	config.connectivity = 0.3f;
	config.amplitude = 0.06f;

	config.neuron_class = neuron_class_create("LIF_NEURON", LIF_NEURON);
	//config.neuron_class = neuron_class_create("LIF_NEURON_REFRAC", LIF_REFRACTORY_NEURON);

	config.synapse_exci_class = synapse_class_create("AMPA", 0.0f, 1.0f, 1, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	config.synapse_inhi_class = synapse_class_create("GABA_A", -90.0f, 1.0f, 6, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	//config.synapse_exci_class = synapse_class_create("CONDUCTANCE_10_TAU", 0.0, 1.0f, 10, 10, CONDUCTANCE_SYNAPSE, 1);
	//config.synapse_inhi_class = synapse_class_create("CONDUCTANCE_10_TAU", 0.0, 1.0f, 10, 10, CONDUCTANCE_SYNAPSE, 1);

	config.example_duration = 500;
	config.spike_frequency = 0.005f;  // 5HZ per second similar to the cortex

	config.use_dump_net_callback = TRUE;

	cortical_slabs_exp_run_config(&config);

	neuron_class_destroy(config.neuron_class);
	synapse_class_destroy(config.synapse_exci_class);
	synapse_class_destroy(config.synapse_inhi_class);
}


static inline Network* create_net_3_layers(uint32_t n_neurons_l1, cortical_slabs_exp_config* config) {
	Network* net = network_create();
	NeuronClass* n_class = neuron_class_copy(config->neuron_class);
	SynapseClass* s_exci_class = synapse_class_copy(config->synapse_exci_class);
	SynapseClass* s_inhi_class = synapse_class_copy(config->synapse_inhi_class);
	s_exci_class->A = config->amplitude;
	s_inhi_class->A = config->amplitude;

	network_add_neuron_class(net, n_class);
	network_add_synapse_class(net, s_exci_class);
	network_add_synapse_class(net, s_inhi_class);

	n_class = network_get_neuron_class_by_string(net, n_class->name);
	s_exci_class = network_get_synapse_class_by_string(net, s_exci_class->name);
	s_inhi_class = network_get_synapse_class_by_string(net, s_inhi_class->name);

	// Prepare layers
	Layer* l1_exci = layer_create(LAYER_FULLY_CONNECTED, (int)(n_neurons_l1 * config->percent_excitatory), n_class, "layer_1_exci");
	Layer* l1_inhi = layer_create(LAYER_FULLY_CONNECTED, (int)(n_neurons_l1 * config->percent_inhibitory), n_class, "layer_1_inhi");
	Layer* l2_exci = layer_create(LAYER_FULLY_CONNECTED, (int)(n_neurons_l1 * config->percent_excitatory), n_class, "layer_2_exci");
	Layer* l2_inhi = layer_create(LAYER_FULLY_CONNECTED, (int)(n_neurons_l1 * config->percent_inhibitory), n_class, "layer_2_inhi");
	Layer* l3_exci = layer_create(LAYER_FULLY_CONNECTED, (int)(config->n_neurons_layer_2 * config->percent_excitatory), n_class, "layer_3_exci");
	Layer* l3_inhi = layer_create(LAYER_FULLY_CONNECTED, (int)(config->n_neurons_layer_2 * config->percent_inhibitory), n_class, "layer_3_inhi");

	layer_add_input_layer(l2_exci, l1_exci, s_exci_class, config->connectivity, 1.0f);
	layer_add_input_layer(l2_exci, l1_inhi, s_inhi_class, config->connectivity, 1.0f);
	layer_add_input_layer(l2_inhi, l1_exci, s_exci_class, config->connectivity, 1.0f);
	layer_add_input_layer(l2_inhi, l1_inhi, s_inhi_class, config->connectivity, 1.0f);

	layer_add_input_layer(l3_exci, l2_exci, s_exci_class, config->connectivity, 1.0f);
	layer_add_input_layer(l3_exci, l2_inhi, s_inhi_class, config->connectivity, 1.0f);
	layer_add_input_layer(l3_inhi, l2_exci, s_exci_class, config->connectivity, 1.0f);
	layer_add_input_layer(l3_inhi, l2_inhi, s_inhi_class, config->connectivity, 1.0f);

	network_add_layer(net, l1_exci, TRUE, FALSE);
	network_add_layer(net, l1_inhi, TRUE, FALSE);
	network_add_layer(net, l2_exci, FALSE, FALSE);
	network_add_layer(net, l2_inhi, FALSE, FALSE);
	network_add_layer(net, l3_exci, FALSE, TRUE);
	network_add_layer(net, l3_inhi, FALSE, TRUE);

	network_compile(net);
	return net;
}

static inline Network* create_net_2_layers(uint32_t n_neurons_l1, cortical_slabs_exp_config* config) {
	Network* net = network_create();
	NeuronClass* n_class = neuron_class_copy(config->neuron_class);
	SynapseClass* s_exci_class = synapse_class_copy(config->synapse_exci_class);
	SynapseClass* s_inhi_class = synapse_class_copy(config->synapse_inhi_class);
	s_exci_class->A = config->amplitude;
	s_inhi_class->A = config->amplitude;

	network_add_neuron_class(net, n_class);
	network_add_synapse_class(net, s_exci_class);
	network_add_synapse_class(net, s_inhi_class);

	n_class = network_get_neuron_class_by_string(net, n_class->name);
	s_exci_class = network_get_synapse_class_by_string(net, s_exci_class->name);
	s_inhi_class = network_get_synapse_class_by_string(net, s_inhi_class->name);

	// Prepare layers
	Layer* l1_exci = layer_create(LAYER_FULLY_CONNECTED, (int)(n_neurons_l1 * config->percent_excitatory), n_class, "layer_1_exci");
	Layer* l1_inhi = layer_create(LAYER_FULLY_CONNECTED, (int)(n_neurons_l1 * config->percent_inhibitory), n_class, "layer_1_inhi");
	Layer* l2_exci = layer_create(LAYER_FULLY_CONNECTED, (int)(config->n_neurons_layer_2 * config->percent_excitatory), n_class, "layer_2_exci");
	Layer* l2_inhi = layer_create(LAYER_FULLY_CONNECTED, (int)(config->n_neurons_layer_2 * config->percent_inhibitory), n_class, "layer_2_inhi");

	layer_add_input_layer(l2_exci, l1_exci, s_exci_class, config->connectivity, 1.0f);
	layer_add_input_layer(l2_exci, l1_inhi, s_inhi_class, config->connectivity, 1.0f);
	layer_add_input_layer(l2_inhi, l1_exci, s_exci_class, config->connectivity, 1.0f);
	layer_add_input_layer(l2_inhi, l1_inhi, s_inhi_class, config->connectivity, 1.0f);

	network_add_layer(net, l1_exci, TRUE, FALSE);
	network_add_layer(net, l1_inhi, TRUE, FALSE);
	network_add_layer(net, l2_exci, FALSE, TRUE);
	network_add_layer(net, l2_inhi, FALSE, TRUE);

	network_compile(net);
	return net;
}


void cortical_slabs_exp_run_config(cortical_slabs_exp_config* config) {
	char config_file[1024] = { 0 };
	char result_folder[1024] = { 0 };
	char* folder_format = NULL;
	char callback_dump_folder[1024] = { 0 };

	// create output_folder
	if (config->neuron_class->type == LIF_REFRACTORY_NEURON) folder_format = "%s\\refract_%d_layers_conn_%.2f_ampli_%.2f";
	else folder_format = "%s\\%d_layers_conn_%.2f_ampli_%.2f";
	sprintf(result_folder, folder_format,
		config->exp_abs_path, 
		config->n_layers,
		config->connectivity, config->amplitude,
		config->example_duration);
	os_mkdir(result_folder);

	sprintf(config_file, "%s\\config.txt", result_folder);
	cortical_slabs_exp_dump_config(config_file, config);


	for (uint32_t i = 0; i < config->n_neurons_layer_1_cnt; ++i) {
		uint32_t n_neurons_l1 = config->n_neurons_layer_1_steps[i];
		printf("[INFO] Running for @n_neurons_l1=%d\n", n_neurons_l1);
		// Prepare network
		Network* net = config->n_layers == 2 ? create_net_2_layers(n_neurons_l1, config) : create_net_3_layers(n_neurons_l1, config);

		// create data
		DataGenerator* data_gen = data_generator_random_spikes_create(1, net, config->spike_frequency, config->example_duration);

		// create simulator
		Simulator* simulator = simulator_create(data_gen, net);

		// add callbacks
		memset(callback_dump_folder, 0, 1024);
		sprintf(callback_dump_folder, "%s\\t_%d", result_folder, n_neurons_l1);
		Callback* net_dump_callback = callback_dump_network_create(net, callback_dump_folder);
		simulator_add_callback(simulator, net_dump_callback);

		simulator_infer(simulator);
		simulator_destroy(simulator);
	}
}


void cortical_slabs_exp_dump_config(const char* file_path, cortical_slabs_exp_config* config) {
	FILE* fp = fopen(file_path, "w");
	if (fp == NULL) {
		fprintf(stderr, "!!!!!Couldn't open %s for writting\n", file_path);
		return;
	}
	String* n_class_desc = neuron_class_get_desc(config->neuron_class);
	String* s_exci_class_desc = synapse_class_get_desc(config->synapse_exci_class);
	String* s_inhi_class_desc = synapse_class_get_desc(config->synapse_inhi_class);

	char neuron_values[1024] = { 0 };
	char neuron_values_aux[1024] = { 0 };
	for (uint32_t i = 0; i < config->n_neurons_layer_1_cnt; ++i) {
		sprintf(neuron_values_aux, "%s %d ", neuron_values, config->n_neurons_layer_1_steps[i]);
		sprintf(neuron_values, "%s", neuron_values_aux);
	}

	fprintf(fp, "cortical_slabs_exp_dump_config\n\n"
		"exp_abs_path: %s\n\n"
		"n_layers: %u\n"
		"n_neurons_layer_1_cnt: %u\n"
		"n_neurons_layer_1_values: %s\n"
		"n_neurons_layer_2: %u\n"
		"connectivity: %f\n"
		"amplitude: %f\n\n"
		"percent_excitatory: %f\n"
		"percent_inhibitory: %f\n\n"
		"neuron_class: %s\n"
		"synapse_exci_class: %s\n"
		"synapse_inhi_class: %s\n\n"
		"example_duration: %u\n"
		"spike_frequency: %f\n"
		"use_dump_net_callback: %d",
		config->exp_abs_path,
		config->n_layers,
		config->n_neurons_layer_1_cnt, neuron_values, 
		config->n_neurons_layer_2, config->connectivity, config->amplitude,
		config->percent_excitatory, config->percent_inhibitory,
		string_get_C_string(n_class_desc), string_get_C_string(s_exci_class_desc), string_get_C_string(s_inhi_class_desc),
		config->example_duration, config->spike_frequency,
		config->use_dump_net_callback
	);
	fclose(fp);

	string_destroy(n_class_desc);
	string_destroy(s_exci_class_desc);
	string_destroy(s_inhi_class_desc);
}


typedef struct cortical_slabs_exp_growing_net_config {
	const char* exp_abs_path;

	uint32_t n_neurons_start;
	float n_neurons_scale_step;
	float connectivity;
	float amplitude;

	float percent_excitatory;
	float percent_inhibitory;

	NeuronClass* neuron_class;
	SynapseClass* synapse_exci_class;
	SynapseClass* synapse_inhi_class;

	// data gen values
	uint32_t example_duration;
	float spike_frequency;

	// callback values;
	BOOL use_dump_net_callback;

}cortical_slabs_exp_growing_net_config;


void cortical_slabs_exp_growing_net_dump_config(const char* file_path, cortical_slabs_exp_growing_net_config* config);
void ccortical_slabs_exp_growing_net_run_config(cortical_slabs_exp_growing_net_config* config);


/********************************************
Growing network experiment
********************************************/
void cortical_slabs_exp_growing_net_run() {
	cortical_slabs_exp_growing_net_config config = { 0 };
	config.exp_abs_path = "d:\\repositories\\Simulator\\experiments\\cortical_slabs\\final";

	config.n_neurons_start = 100;
	config.n_neurons_scale_step = 1.5f;
	config.percent_excitatory = 0.8f;
	config.percent_inhibitory = 0.2f;
	// for refract
	config.connectivity = 0.3f;
	config.amplitude = 0.1f;

	// for non-refract
	//config.connectivity = 0.3f;
	//config.amplitude = 0.10f;

	//config.neuron_class = neuron_class_create("LIF_NEURON", LIF_NEURON);
	config.neuron_class = neuron_class_create("LIF_NEURON_REFRAC", LIF_REFRACTORY_NEURON);

	config.synapse_exci_class = synapse_class_create("AMPA", 0.0f, 1.0f, 1, 2, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	config.synapse_inhi_class = synapse_class_create("GABA_A", -90.0f, 1.0f, 6, 2, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	//config.synapse_exci_class = synapse_class_create("CONDUCTANCE_10_TAU", 0.0, 1.0f, 10, 10, CONDUCTANCE_SYNAPSE, 1);
	//config.synapse_inhi_class = synapse_class_create("CONDUCTANCE_10_TAU", 0.0, 1.0f, 10, 10, CONDUCTANCE_SYNAPSE, 1);

	config.example_duration = 500;
	config.spike_frequency = 0.005f;  // 5HZ per second similar to the cortex

	config.use_dump_net_callback = TRUE;

	ccortical_slabs_exp_growing_net_run_config(&config);

	neuron_class_destroy(config.neuron_class);
	synapse_class_destroy(config.synapse_exci_class);
	synapse_class_destroy(config.synapse_inhi_class);
}


void ccortical_slabs_exp_growing_net_run_config(cortical_slabs_exp_growing_net_config* config) {
	char config_file[1024] = { 0 };
	char result_folder[1024] = { 0 };
	char* folder_format = NULL;
	char callback_dump_folder[1024] = { 0 };

	// create output_folder
	if (config->neuron_class->type == LIF_REFRACTORY_NEURON) folder_format = "%s\\growing_net_%d_%.3f_duration_%d_refract";
	else folder_format = "%s\\growing_net_%d_%.3f_duration_%d";
	sprintf(result_folder, folder_format,
		config->exp_abs_path,
		config->n_neurons_start, config->spike_frequency,
		config->example_duration);
	os_mkdir(result_folder);

	sprintf(config_file, "%s\\config.txt", result_folder);
	cortical_slabs_exp_growing_net_dump_config(config_file, config);

	network_sequential_n_layers_config net_config = { 0 };
	net_config.n_layers = 10;
	net_config.n_exci_neurons = (int)((float)(config->n_neurons_start) * config->percent_excitatory);
	net_config.n_inhi_neurons = (int)((float)(config->n_neurons_start) * config->percent_inhibitory);
	net_config.n_neurons_scale_step = config->n_neurons_scale_step;
	net_config.connectivity = config->connectivity;
	net_config.synapse_weight = 1.0f;
	net_config.n_class = neuron_class_copy(config->neuron_class);
	net_config.s_exci_class = synapse_class_copy(config->synapse_exci_class);
	net_config.s_exci_class->A = config->amplitude;
	net_config.s_inhi_class = synapse_class_copy(config->synapse_inhi_class);
	net_config.s_inhi_class->A = config->amplitude;
	Network* net = network_sequential_n_layers(&net_config);
	network_summary(net);
	// create data
	DataGenerator* data_gen = data_generator_random_spikes_create(1, net, config->spike_frequency, config->example_duration);

	// create simulator
	Simulator* simulator = simulator_create(data_gen, net);

	// add callbacks
	memset(callback_dump_folder, 0, 1024);
	sprintf(callback_dump_folder, "%s\\network", result_folder);
	Callback* net_dump_callback = callback_dump_network_create(net, callback_dump_folder);
	simulator_add_callback(simulator, net_dump_callback);

	simulator_infer(simulator);
	simulator_destroy(simulator);
}


void cortical_slabs_exp_growing_net_dump_config(const char* file_path, cortical_slabs_exp_growing_net_config* config) {
	FILE* fp = fopen(file_path, "w");
	if (fp == NULL) {
		fprintf(stderr, "!!!!!Couldn't open %s for writting\n", file_path);
		return;
	}
	String* n_class_desc = neuron_class_get_desc(config->neuron_class);
	String* s_exci_class_desc = synapse_class_get_desc(config->synapse_exci_class);
	String* s_inhi_class_desc = synapse_class_get_desc(config->synapse_inhi_class);

	fprintf(fp, "cortical_slabs_exp_growing_net_dump_config\n\n"
		"exp_abs_path: %s\n\n"
		"n_neurons_start: %u\n"
		"n_neurons_scale_step: %f\n"
		"connectivity: %f\n"
		"amplitude: %f\n\n"
		"percent_excitatory: %f\n"
		"percent_inhibitory: %f\n\n"
		"neuron_class: %s\n"
		"synapse_exci_class: %s\n"
		"synapse_inhi_class: %s\n\n"
		"example_duration: %u\n"
		"spike_frequency: %f\n"
		"use_dump_net_callback: %d",
		config->exp_abs_path,
		config->n_neurons_start, config->n_neurons_scale_step,
		config->connectivity, config->amplitude,
		config->percent_excitatory, config->percent_inhibitory,
		string_get_C_string(n_class_desc), string_get_C_string(s_exci_class_desc), string_get_C_string(s_inhi_class_desc),
		config->example_duration, config->spike_frequency,
		config->use_dump_net_callback
	);
	fclose(fp);

	string_destroy(n_class_desc);
	string_destroy(s_exci_class_desc);
	string_destroy(s_inhi_class_desc);
}