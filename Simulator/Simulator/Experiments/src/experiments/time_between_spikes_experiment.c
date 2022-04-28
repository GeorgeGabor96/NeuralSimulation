#include "Simulator.h"
#include "Callback.h"
#include "DataGen.h"
#include "utils/os.h"

#include "../../include/config_exp.h"


void time_between_spikes_experiment() {
	char result_path[256] = { 0 };
	sprintf(result_path, "%s\\\\experiments_nice_plots\\time_between_spikes_experiment\\tau_l1_10_5_n_tau_l2_100_time_between_20", result_base_folder);

	// create network
	Network* net = network_create();
	network_add_neuron_class(net, neuron_class_create("LIF_NEURON", LIF_NEURON));
	network_add_synapse_class(net, synapse_class_create("SYN_20TAU", 0.0f, 1.0f, 100.0f, 1, CONDUCTANCE_SYNAPSE, 1));

	NeuronClass* n_class = network_get_neuron_class_by_name(net, "LIF_NEURON");
	SynapseClass* s_class = network_get_synapse_class_by_name(net, "SYN_20TAU");

	Layer* layer_input = layer_create_fully_connected(5, n_class, "layer_in");
	Layer* layer_output = layer_create_fully_connected(1, n_class, "layer_out");
	layer_add_input_layer(layer_output, layer_input, s_class, 1.0f, 1.0f);

	network_add_layer(net, layer_input, TRUE, FALSE);
	network_add_layer(net, layer_output, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	// create data generator
	DataGenerator* constant_current = data_generator_with_step_between_neurons_create(1, net, 20, 101);

	// create callbacks
	layer_input = network_get_layer_by_name(net, "layer_in");
	layer_output = network_get_layer_by_name(net, "layer_out");
	Callback* c1 = callback_dump_layer_neurons_create(layer_input, result_path);
	Callback* c2 = callback_dump_layer_neurons_create(layer_output, result_path);

	// create simulator
	Simulator* simulator = simulator_create(constant_current, net);
	simulator_add_callback(simulator, c1);
	simulator_add_callback(simulator, c2);

	// run experiments
	simulator_infer(simulator);

	// cleanup 
	simulator_destroy(simulator);
}



void run_3_inputs_1_output_step_between_neurons_cond() {
	char* result_exp_path = "d:\\repositories\\Simulator\\experiments\\experiments_imrpovements_final\\basic\\multiple_times_between_currents_tau_10_conduct";
	uint32_t example_duration = 101;
	uint32_t steps_between_neurons[4] = { 1, 5, 10, 20 };
	float tau = 10.0f;

	//SynapseClass* s_class = synapse_class_create("EXCI", 0, 0.035f, tau, 1, VOLTAGE_DEPENDENT_SYNAPSE, 1.0f);
	SynapseClass* s_class = synapse_class_create("CONDUCT", 0, 2.0f, tau, 1, CONDUCTANCE_SYNAPSE, 1.0f);
	NeuronClass * n_class = neuron_class_create("LIF_NEURON", LIF_NEURON);
	NeuronClass* net_n_class = NULL;
	SynapseClass* net_s_class = NULL;

	char buffer[1024] = { 0 };
	char config_path[1024] = { 0 };

	for (uint32_t i = 0; i < 4; ++i) {

		memset(buffer, 0, 1024);
		sprintf(buffer, "%s\\tau_%.2f_step_%u\\", result_exp_path, tau, steps_between_neurons[i]);
		os_mkdir(buffer);

		memset(config_path, 0, 1024);
		sprintf(config_path, "%s\\config.txt", buffer);

		FILE* fp = fopen(config_path, "w");
		String* n_class_desc = neuron_class_get_desc(n_class);
		String* s_class_desc = synapse_class_get_desc(s_class);
		fprintf(fp, "exp_abs_path: %s\n\n"
			"neuron_class: %s\n"
			"synapse_class: %s\n"
			"duration: %u\n\n"
			"step_between_neurons: %u\n\n"
			"tau: %.2f\n"
			"3 inputs 1 output",
			buffer,
			string_get_C_string(n_class_desc), string_get_C_string(s_class_desc),
			example_duration,
			steps_between_neurons[i],
			tau
		);
		fclose(fp);

		string_destroy(n_class_desc);
		string_destroy(s_class_desc);


		Network* net = network_create();
		network_add_neuron_class(net, neuron_class_copy(n_class));
		network_add_synapse_class(net, synapse_class_copy(s_class));

		net_n_class = network_get_neuron_class_by_name(net, n_class->name->data);
		net_s_class = network_get_synapse_class_by_name(net, s_class->name->data);
		Layer* input_layer = layer_create_fully_connected(3, net_n_class, "input_layer");
		Layer* output_layer = layer_create_fully_connected(1, net_n_class, "output_layer");
		layer_add_input_layer(output_layer, input_layer, net_s_class, 1.0f, 1.0f);
		network_add_layer(net, input_layer, TRUE, FALSE);
		network_add_layer(net, output_layer, FALSE, TRUE);
		network_compile(net);
		network_summary(net);

		DataGenerator* constant_current = data_generator_with_step_between_neurons_create(1, net, steps_between_neurons[i], example_duration);

		Callback* network_dump = callback_dump_network_create(net, buffer);

		Simulator* simulator = simulator_create(constant_current, net);
		simulator_add_callback(simulator, network_dump);

		simulator_infer(simulator);

		simulator_destroy(simulator);
	}

	neuron_class_destroy(n_class);
}


void run_3_inputs_1_output_step_between_neurons_exci_inhi() {
	char* result_exp_path = "d:\\repositories\\Simulator\\experiments\\experiments_imrpovements_final\\basic\\multiple_times_between_currents_exci_inhi_2";
	uint32_t example_duration = 101;
	uint32_t steps_between_neurons[4] = { 1, 5, 10, 20 };

	//SynapseClass* s_class = synapse_class_create("EXCI", 0, 0.035f, tau, 1, VOLTAGE_DEPENDENT_SYNAPSE, 1.0f);
	NeuronClass* n_class = neuron_class_create("LIF_NEURON", LIF_NEURON);
	SynapseClass* s_exci_class = synapse_class_create("AMPA", 0.0f, 0.25f, 1, 1, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	SynapseClass* s_inhi_class = synapse_class_create("GABA_A", -90.0f, 0.25f, 6, 1, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	NeuronClass* net_n_class = NULL;
	SynapseClass* net_s_exci_class = NULL;
	SynapseClass* net_s_inhi_class = NULL;

	char buffer[1024] = { 0 };
	char config_path[1024] = { 0 };

	for (uint32_t i = 0; i < 4; ++i) {

		memset(buffer, 0, 1024);
		sprintf(buffer, "%s\\step_%u\\", result_exp_path, steps_between_neurons[i]);
		os_mkdir(buffer);

		memset(config_path, 0, 1024);
		sprintf(config_path, "%s\\config.txt", buffer);

		FILE* fp = fopen(config_path, "w");
		String* n_class_desc = neuron_class_get_desc(n_class);
		String* s_exci_class_desc = synapse_class_get_desc(s_exci_class);
		String* s_inhi_class_desc = synapse_class_get_desc(s_inhi_class);
		fprintf(fp, "exp_abs_path: %s\n\n"
			"neuron_class: %s\n"
			"synapse_exci_class: %s\n"
			"synapse_inhi_class: %s\n"
			"duration: %u\n\n"
			"step_between_neurons: %u\n\n"
			"3 inputs 1 output",
			buffer,
			string_get_C_string(n_class_desc), string_get_C_string(s_exci_class_desc), string_get_C_string(s_inhi_class_desc),
			example_duration,
			steps_between_neurons[i]
		);
		fclose(fp);

		string_destroy(n_class_desc);
		string_destroy(s_exci_class_desc);
		string_destroy(s_inhi_class_desc);

		Network* net = network_create();
		network_add_neuron_class(net, neuron_class_copy(n_class));
		network_add_synapse_class(net, synapse_class_copy(s_exci_class));
		network_add_synapse_class(net, synapse_class_copy(s_inhi_class));

		net_n_class = network_get_neuron_class_by_name(net, n_class->name->data);
		net_s_exci_class = network_get_synapse_class_by_name(net, s_exci_class->name->data);
		net_s_inhi_class = network_get_synapse_class_by_name(net, s_inhi_class->name->data);
		Layer* input_layer_exci_1 = layer_create_fully_connected(1, net_n_class, "input_layer_exci");
		Layer* input_layer_inhi = layer_create_fully_connected(1, net_n_class, "input_layer_inhi");
		Layer* input_layer_exci_2 = layer_create_fully_connected(1, net_n_class, "input_layer_exci_2");
		Layer* output_layer = layer_create_fully_connected(1, net_n_class, "output_layer");
		layer_add_input_layer(output_layer, input_layer_exci_1, net_s_exci_class, 1.0f, 1.0f);
		layer_add_input_layer(output_layer, input_layer_inhi, net_s_inhi_class, 1.0f, 1.0f);
		layer_add_input_layer(output_layer, input_layer_exci_2, net_s_exci_class, 1.0f, 1.0f);
		network_add_layer(net, input_layer_exci_1, TRUE, FALSE);
		network_add_layer(net, input_layer_inhi, TRUE, FALSE);
		network_add_layer(net, input_layer_exci_2, TRUE, FALSE);
		network_add_layer(net, output_layer, FALSE, TRUE);
		network_compile(net);
		network_summary(net);

		DataGenerator* constant_current = data_generator_with_step_between_neurons_create(1, net, steps_between_neurons[i], example_duration);

		Callback* network_dump = callback_dump_network_create(net, buffer);

		Simulator* simulator = simulator_create(constant_current, net);
		simulator_add_callback(simulator, network_dump);

		simulator_infer(simulator);

		simulator_destroy(simulator);
	}

	neuron_class_destroy(n_class);
}