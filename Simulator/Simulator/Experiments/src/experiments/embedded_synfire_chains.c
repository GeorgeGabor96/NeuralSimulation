#include "Simulator.h"
#include "utils/MemoryManagement.h"
#include <math.h>

#define n 1000
#define m 20     // minimum number of neurons selected for layer k
#define M 200	 // maximum number of neurons selected for layer k


double get_gaussian_value() {
	double y1 = (double)rand() / (double)RAND_MAX;
	double y2 = (double)rand() / (double)RAND_MAX;

	return cos(2.0 * 3.14 * y2) * sqrt(-2.0 * log(y1));
}


static inline Array* create_neurons(uint32_t n_neurons, NeuronClass* n_class) {
	Array* neurons = array_create(n_neurons, n_neurons, sizeof(Neuron));

	for (uint32_t i = 0; i < neurons->length; ++i) {
		Neuron* neuron = (Neuron*)array_get(neurons, i);
		neuron_init(neuron, n_class);
	}

	return neurons;
}


static inline Array* create_chains(uint32_t n_chains, Array* neuron_pool, uint32_t n_exci_neurons, uint32_t n_inhi_neurons, NeuronClass* n_class, SynapseClass* s_exci_class, SynapseClass* s_inhi_class) {
	Array* chains_input_neurons = array_create(n_chains, 0, sizeof(Array));
	
	for (uint32_t chain_idx = 0; chain_idx < n_chains; ++chain_idx) {
		printf("Building chain %d\n", chain_idx);

		// to construct a chain start with n_k = (m + M) / 2
		// and construct succesive layers until the number of neurons per
		// layer is too big or too small
		uint32_t n_k = (M + m) / 2;
		ArrayUint32* inhi_idxs_current = NULL;
		ArrayUint32* exci_idxs_current = NULL;
		ArrayUint32* exci_idxs_previous = array_create(1, 0, sizeof(uint32_t));
		Array* input_neurons = array_create(n_k + n_k / 4, 0, sizeof(uint32_t)); // I alwase asume that n_k exci and n_k / 4 inhi input neurons

		while (n_k >= m && n_k <= M) {
			printf("Build layer with %d exci neurons\n", n_k);

			// a Layer contains @n_k excitatory neurons which are connected with @m excitatory neurons from the previous layer
			exci_idxs_current = array_random_int_uint32(n_k, 0, n_exci_neurons - 1);
			for (uint32_t exci_neuron_current_idx = 0; exci_neuron_current_idx < exci_idxs_current->length; ++exci_neuron_current_idx) {
				uint32_t current_exci_idx = *((uint32_t*)array_get(exci_idxs_current, exci_neuron_current_idx));
				Neuron* exci_neuron_current = (Neuron*)array_get(neuron_pool, current_exci_idx);

				// if first layer then record input exci neuron and continue
				if (exci_idxs_previous->length == 0) {
					array_append(input_neurons, &current_exci_idx);
					continue;
				}

				// select m excitatory neurons from previous layer
				ArrayUint32* neurons_to_connect = array_random_int_uint32(m, 0, exci_idxs_previous->length - 1);
				for (uint32_t exci_neuron_previous_idx = 0; exci_neuron_previous_idx < neurons_to_connect->length; ++exci_neuron_previous_idx) {
					uint32_t idx_neuron_connect = *((uint32_t*)array_get(neurons_to_connect, exci_neuron_previous_idx));
					uint32_t idx_neuron_previous = *((uint32_t*)array_get(exci_idxs_previous, idx_neuron_connect));
					Neuron* exci_neuron_previous = (Neuron*)array_get(neuron_pool, idx_neuron_previous);

					Synapse* exci_synapse = synapse_create(s_exci_class, 1.0);
					neuron_add_in_synapse(exci_neuron_current, exci_synapse);
					neuron_add_out_synapse(exci_neuron_previous, exci_synapse);
				}
				array_destroy(neurons_to_connect, NULL);
			}

			// a Layer contains @n_k / 4 inhibitory neurons which receive input from @m excitatory neurons from the previous layer
			// and give output to 5% of the total excitatory neurons
			// the inhibitory neurons start after the excitatory ones
			inhi_idxs_current = array_random_int_uint32(n_k / 4, n_exci_neurons, n_exci_neurons + n_inhi_neurons - 1);
			for (uint32_t inhi_neuron_idx = 0; inhi_neuron_idx < inhi_idxs_current->length; ++inhi_neuron_idx) {
				uint32_t current_inhi_idx = *((uint32_t*)array_get(inhi_idxs_current, inhi_neuron_idx));
				Neuron* inhi_neuron_current = (Neuron*)array_get(neuron_pool, current_inhi_idx);

				// select 5% excitatory neurons from the pool to link with the inhibitory ones
				uint32_t n_exci_neurons_to_connect = (uint32_t)((float)n_exci_neurons * 0.05f);
				ArrayUint32* neurons_to_connect = array_random_int_uint32(n_exci_neurons_to_connect, 0, n_exci_neurons - 1);
				for (uint32_t exci_neuron_to_connect_idx = 0; exci_neuron_to_connect_idx < neurons_to_connect->length; ++exci_neuron_to_connect_idx) {
					uint32_t exci_neuron_idx = *((uint32_t*)array_get(neurons_to_connect, exci_neuron_to_connect_idx));
					Neuron* exci_neuron = (Neuron*)array_get(neuron_pool, exci_neuron_idx);

					Synapse* inhi_synapse = synapse_create(s_inhi_class, 1.0);
					neuron_add_in_synapse(exci_neuron, inhi_synapse);
					neuron_add_out_synapse(inhi_neuron_current, inhi_synapse);
				}
				array_destroy(neurons_to_connect, NULL);

				// if first layer then record input inhi neuron and continue
				if (exci_idxs_previous->length == 0) {
					array_append(input_neurons, &current_inhi_idx);
					continue;
				}

				// select m excitatory neurons from previous layer and link with them
				if (exci_idxs_previous->length == 0) break;
				neurons_to_connect = array_random_int_uint32(m, 0, exci_idxs_previous->length - 1);
				for (uint32_t exci_neuron_previous_idx = 0; exci_neuron_previous_idx < neurons_to_connect->length; ++exci_neuron_previous_idx) {
					uint32_t idx_neuron_connect = *((uint32_t*)array_get(neurons_to_connect, exci_neuron_previous_idx));
					uint32_t idx_neuron_previous = *((uint32_t*)array_get(exci_idxs_previous, idx_neuron_connect));
					Neuron* exci_neuron_previous = (Neuron*)array_get(neuron_pool, idx_neuron_previous);

					Synapse* exci_synapse = synapse_create(s_exci_class, 1.0);

					neuron_add_in_synapse(inhi_neuron_current, exci_synapse);
					neuron_add_out_synapse(exci_neuron_previous, exci_synapse);

				}
				array_destroy(neurons_to_connect, NULL);
			}
			
			// Current exci neurons become the previous ones
			array_destroy(exci_idxs_previous, NULL);
			array_destroy(inhi_idxs_current, NULL);
			exci_idxs_previous = exci_idxs_current;

			// update the @n_k variable for the next layer
			double r_value = get_gaussian_value();
			if (r_value < 0) r_value = -r_value;
			n_k = (int)((get_gaussian_value() + 1.0) * n_k);
		}

		// free the last exci neurons of the chain
		array_destroy(exci_idxs_current, NULL);

		// save the chain inputs and do a shallow free
		array_append(chains_input_neurons, input_neurons);
		free(input_neurons);
	}

	return chains_input_neurons;
}


static inline Array* get_spikes_for_neuron_pool(uint32_t n_neurons, Array* chains_input_neurons, uint32_t duration_per_chain, uint32_t pulse_duration, float spike_frequency) {
	// return an array where every element is a ArrayBool, the spike value for at each time stamp
	Array* spikes_for_neurons = array_create(n_neurons, 0, sizeof(ArrayBool*));
	uint32_t total_duration = chains_input_neurons->length * duration_per_chain + duration_per_chain; // the pluse is just to let the system evolve without input
	
	// init everything with zeros
	uint32_t neuron_idx = 0;
	for (neuron_idx = 0; neuron_idx < n_neurons; ++neuron_idx) {
		Array* no_spikes = array_zeros_bool(total_duration);
		array_append(spikes_for_neurons, &no_spikes);
	}

	// add the spikes
	uint32_t chain_idx = 0;
	uint32_t chain_time_pulse_start = 0;
	uint32_t time = 0;
	Array* chain_input_neurons = NULL;
	BOOL spike = TRUE;
	for (chain_idx = 0; chain_idx < chains_input_neurons->length; ++chain_idx) {
		chain_input_neurons = (ArrayUint32*)array_get(chains_input_neurons, chain_idx);

		// find the time when the input for current chain should start
		chain_time_pulse_start = chain_idx * duration_per_chain;
		for (time = chain_time_pulse_start; time < chain_time_pulse_start + pulse_duration; ++time) {

			// to over each neurons and add spikes
			for (uint32_t neuron_chain_idx = 0; neuron_chain_idx < chain_input_neurons->length; ++neuron_chain_idx) {
				// get actual neuron idx
				uint32_t n_idx = *((uint32_t*)array_get(chain_input_neurons, neuron_chain_idx));
				
				// get the neuron array of values
				Array* neuron_spikes = *((ArrayBool**)array_get(spikes_for_neurons, n_idx));

				if ((double)rand() / (double)RAND_MAX <= spike_frequency) {
					array_set(neuron_spikes, time, &spike);
				}
			}
		}
	}

	return spikes_for_neurons;
}


static inline void dump_neurons_full_input(Array* spikes_for_neurons) {
	char file_name[128] = { 0 };
	String* file_path = NULL;
	String* data_path = string_create("d:\\repositories\\Simulator\\experiments\\embedded_synchains\\input");
	String* data_name = string_create("spikes");
	for (uint32_t n_idx = 0; n_idx < spikes_for_neurons->length; ++n_idx) {
		ArrayBool* spikes_for_neuron = *((ArrayBool**)array_get(spikes_for_neurons, n_idx));

		sprintf(file_name, "spikes_N%u.bin", n_idx);
		file_path = string_path_join_string_and_C(data_path, file_name);
		array_bool_dump(spikes_for_neuron, file_path, data_name);
		string_destroy(file_path);
	}
	string_destroy(data_name);
	string_destroy(data_path);
}


void embedded_synfire_chains_exp() {
	// create classes and neurons
	NeuronClass* n_class = neuron_class_create("LIF_REFRACT", LIF_REFRACTORY_NEURON);
	SynapseClass* s_exci_class = synapse_class_create("AMPA", 0.0f, 1.0f, 1, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	SynapseClass* s_inhi_class = synapse_class_create("GABA_A", -90.0f, 1.0f, 6, 10, VOLTAGE_DEPENDENT_SYNAPSE, 1);
	
	// create a pool of neurons where the first @n neurons are excitatory and the next @n / 4 are inhibitory
	uint32_t n_exci_neurons = n;
	uint32_t n_inhi_neurons = n / 4;
	Array* neuron_pool = create_neurons(n_exci_neurons + n_inhi_neurons, n_class);

	// create the chains
	Array* chains_input_neurons = create_chains(10, neuron_pool, n_exci_neurons, n_inhi_neurons, n_class, s_exci_class, s_inhi_class);
	for (uint32_t chain_idx = 0; chain_idx < chains_input_neurons->length; ++chain_idx) {
		printf("For chain %d\n", chain_idx);
		Array* chain_input = (Array*)array_get(chains_input_neurons, chain_idx);
		for (uint32_t chain_input_idx = 0; chain_input_idx < chain_input->length; ++chain_input_idx) {
			uint32_t neuron_idx = *((uint32_t*)array_get(chain_input, chain_input_idx));
			printf("%d ", neuron_idx);
		}
	}

	// give them input
	uint32_t duration_per_chain = 100;
	uint32_t pulse_duration = 20;
	float pulse_spike_frequency = 0.05f; // 50Hz
	Array* spikes_for_neurons = get_spikes_for_neuron_pool(neuron_pool->length, chains_input_neurons, duration_per_chain, pulse_duration, pulse_spike_frequency);
	dump_neurons_full_input(spikes_for_neurons);


	// dump the net at every step


	// infer loop
	// tine minte ce a facut neuron cu append


	// cleanup
	for (uint32_t chain_idx = 0; chain_idx < chains_input_neurons->length; ++chain_idx) {
		Array* chain_input = (Array*)array_get(chains_input_neurons, chain_idx);
		array_reset(chain_input, NULL);
	}
	array_destroy(chains_input_neurons, NULL);
	
	for (uint32_t neuron_idx = 0; neuron_idx < spikes_for_neurons->length; ++neuron_idx) {
		ArrayBool* spikes = *((ArrayBool**)array_get(spikes_for_neurons, neuron_idx));
		array_destroy(spikes, NULL);
	}
	array_destroy(spikes_for_neurons, NULL);

	array_destroy(neuron_pool, (ElemReset)neuron_reset);
	neuron_class_destroy(n_class);
	synapse_class_destroy(s_exci_class);
	synapse_class_destroy(s_inhi_class);

	memory_manage_report();
}