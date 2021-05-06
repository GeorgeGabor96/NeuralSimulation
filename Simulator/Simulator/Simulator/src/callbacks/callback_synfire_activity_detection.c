#include "callbacks/callback_dump.h"

#include "utils/MemoryManagement.h"
#include "Containers.h"



// helper definitions
typedef Array SpikeStatesForNeuron;	// contains the state of the spike variable at each timestamp
typedef Array SpikeStatesForLayer;	// contains @SpikeStatesForNeuron's
typedef Array SpikeStatesForNetwork; // contains @SpikeStatesForLayer's

typedef struct C_Data {
	uint32_t max_sync_act_duration;
	float min_ratio;
	float max_ratio;
	String* output_file_path;
	SpikeStatesForNetwork spike_states_for_network;
} C_Data;


BOOL callback_detect_synfire_activity_data_is_valid(C_Data* data);
void callback_detect_synfire_activity_data_destroy(C_Data* data);
void callback_detect_synfire_activity_data_update(C_Data* data, Network* net);
void callback_detect_synfire_activity_data_run(C_Data* data, Network* net);


Callback* callback_detect_synfire_activity_create(Network* net, uint32_t max_sync_act_duration, float min_ratio, float max_ratio, const char* output_file_path) {
	Callback* callback = NULL;
	Layer* layer = NULL;
	SpikeStatesForLayer* spike_states_for_layer = NULL;
	C_Data* data = NULL;
	Status status = FAIL;
	uint32_t i = 0;
	BOOL spikes_for_network_init = FALSE;

	check(max_sync_act_duration != 0, "max_sync_act_duration is 0");
	check(min_ratio > 0.0f, "min_ratio <= 0.0f");
	check(max_ratio > 0.0f, "max_ratio <= 0.0f");
	check(network_is_valid(net) == TRUE, invalid_argument("net"));

	data = (C_Data*)calloc(1, sizeof(C_Data), "callback_detect_synfire_activity_create data");
	check_memory(data);

	callback = (Callback*)calloc(1, sizeof(Callback), "callback_detect_synfire_activity_create callback");

	// init the @SpikeStatesForNetwork
	status = array_init(&(data->spike_states_for_network), net->layers.length, net->layers.length, sizeof(SpikeStatesForLayer));
	check(status == SUCCESS, "status is %u", status);
	spikes_for_network_init = TRUE;

	// init the @SpikeStatesForLayer
	for (i = 0; i < net->layers.length; ++i) {
		layer = (Layer*)array_get(&(net->layers), i);
		spike_states_for_layer = (SpikeStatesForLayer*)array_get(&(data->spike_states_for_network), i);
		status = array_of_arrays_init(spike_states_for_layer, layer->neurons.length, sizeof(BOOL));
		check(status == SUCCESS, "status is %u", status);
	}

	data->max_sync_act_duration = max_sync_act_duration;
	data->min_ratio = min_ratio;
	data->max_ratio = max_ratio;
	data->output_file_path = string_create(output_file_path);
	check(string_is_valid(data->output_file_path) == TRUE, invalid_argument("data->output_file_path"));
	callback->data = data;
	callback->data_is_valid = callback_detect_synfire_activity_data_is_valid;
	callback->data_destroy = callback_detect_synfire_activity_data_destroy;
	callback->data_update = callback_detect_synfire_activity_data_update;
	callback->data_run = callback_detect_synfire_activity_data_run;

	return callback;

ERROR
	if (data != NULL) {
		if (data->output_file_path != NULL) string_destroy(data->output_file_path);
		free(data);
	}
	if (callback != NULL) free(data);
	if (spikes_for_network_init == TRUE) {
		while (i > 0) {
			i--;
			spike_states_for_layer = (SpikeStatesForLayer*)array_get(&(data->spike_states_for_network), i);
			array_of_arrays_reset(spike_states_for_layer);
		}
		array_reset(&(data->spike_states_for_network), NULL);
	}
	return NULL;
}


BOOL callback_detect_synfire_activity_data_is_valid(C_Data* data) {
	check(data != NULL, null_argument("data"));
	check(data->max_sync_act_duration != 0, invalid_argument("data->max_sync_act_duration"));
	check(data->min_ratio > 0.0f, invalid_argument("data->min_ratio"));
	check(data->max_ratio > 0.0f, invalid_argument("data->max_ratio"));
	check(string_is_valid(data->output_file_path) == TRUE, invalid_argument("data->output-file_paht"));

	uint32_t layer_idx = 0;
	uint32_t neuron_idx = 0;
	SpikeStatesForLayer* spike_states_for_layer = NULL;
	SpikeStatesForNeuron* spike_states_for_neuron = NULL;

	check(array_is_valid(&(data->spike_states_for_network)) == TRUE, invalid_argument("data->spike_states_for_network"));
	for (layer_idx = 0; layer_idx < data->spike_states_for_network.length; ++layer_idx) {
		spike_states_for_layer = (SpikeStatesForLayer*)array_get(&(data->spike_states_for_network), layer_idx);
		check(array_is_valid(spike_states_for_layer) == TRUE, invalid_argument("spike_states_for_layer"));

		for (neuron_idx = 0; neuron_idx < spike_states_for_layer->length; ++neuron_idx) {
			spike_states_for_neuron = (SpikeStatesForNeuron*)array_get(spike_states_for_layer, neuron_idx);
			check(array_is_valid(spike_states_for_neuron) == TRUE, invalid_argument("spike_states_for_neuron"));
		}
	}

	return TRUE;
ERROR
	return FALSE;
}



void callback_detect_synfire_activity_data_destroy(C_Data* data) {
	check(callback_detect_synfire_activity_data_is_valid(data) == TRUE, invalid_argument("data"));
	uint32_t layer_idx = 0;
	SpikeStatesForLayer* spike_states_for_layer = NULL;

	string_destroy(data->output_file_path);
	array_reset(&(data->spike_states_for_network), array_of_arrays_reset);
	memset(data, 0, sizeof(C_Data));
	free(data);

ERROR
	return;
}


void callback_detect_synfire_activity_data_update(C_Data* data, Network* net) {
	check(callback_detect_synfire_activity_data_is_valid(data) == TRUE, invalid_argument("data"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	
	uint32_t layer_idx = 0;
	uint32_t neuron_idx = 0;
	Layer* layer = NULL;
	Neuron* neuron = NULL;
	SpikeStatesForLayer* spike_states_for_layer = NULL;
	SpikeStatesForNeuron* spike_states_for_neuron = NULL;
	SpikeStatesForNetwork* spike_states_for_net = &(data->spike_states_for_network);

	// go over each layer
	for (layer_idx = 0; layer_idx < net->layers.length; ++layer_idx) {
		layer = (Layer*)array_get(&(net->layers), layer_idx);
		spike_states_for_layer = (SpikeStatesForLayer*)array_get(spike_states_for_net, layer_idx);

		// go over each neuron
		for (neuron_idx = 0; neuron_idx < layer->neurons.length; ++neuron_idx) {
			neuron = (Neuron*)array_get(&(layer->neurons), neuron_idx);
			spike_states_for_neuron = (SpikeStatesForNeuron*)array_get(spike_states_for_layer, neuron_idx);

			// save the spike state
			array_append(spike_states_for_neuron, &(neuron->spike));
		}
	}

ERROR
	return;
}



static inline float get_layer_pulse_mean_duration(SpikeStatesForLayer* spike_states_for_layer, uint32_t max_pulse_duration) {
	uint32_t total_neurons_pulse_duration = 0;

	SpikeStatesForNeuron* spike_states_for_neuron = NULL;
	uint32_t neuron_idx = 0;
	uint32_t time_idx = 0;
	uint32_t pulse_time_idx = 0;
	uint32_t last_spike = 0;
	BOOL spike_state = FALSE;

	for (neuron_idx = 0; neuron_idx < spike_states_for_layer->length; ++neuron_idx) {
		spike_states_for_neuron = (SpikeStatesForNeuron*)array_get(spike_states_for_layer, neuron_idx);

		for (time_idx = 0; time_idx < spike_states_for_neuron->length; ++time_idx) {
			spike_state = *((BOOL*)array_get(spike_states_for_neuron, time_idx));
			
			// check start of pulse
			if (spike_state == TRUE) {
				last_spike = time_idx;
				
				// find how long is it
				for (pulse_time_idx = time_idx + 1; pulse_time_idx < spike_states_for_neuron->length && pulse_time_idx <= time_idx + max_pulse_duration; ++pulse_time_idx) {
					spike_state = *((BOOL*)array_get(spike_states_for_neuron, pulse_time_idx));
					if (spike_state == TRUE) {
						last_spike = pulse_time_idx;
					}
				}

				// add the duration
				total_neurons_pulse_duration += last_spike - time_idx;
				
				// go to next neuron
				break;
			}
		}
	}

	return (float)total_neurons_pulse_duration / (float)(spike_states_for_layer->length);
}


void callback_detect_synfire_activity_data_run(C_Data* data, Network* net) {
	check(callback_detect_synfire_activity_data_is_valid(data) == TRUE, invalid_argument("data"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));

	/* NOTE
		currently to compute the state of the synfire chain we compare the activity from the second layer and the last layer
	*/
	SpikeStatesForNetwork* spike_states_for_net = &(data->spike_states_for_network);
	SpikeStatesForLayer* spike_states_second_layer = (SpikeStatesForLayer*)array_get(spike_states_for_net, 1);
	SpikeStatesForLayer* spike_states_last_layer = (SpikeStatesForLayer*)array_get(spike_states_for_net, spike_states_for_net->length - 1);

	float second_layer_mean_pulse_duration = get_layer_pulse_mean_duration(spike_states_second_layer, data->max_sync_act_duration);
	float last_layer_mean_pulse_duration = get_layer_pulse_mean_duration(spike_states_last_layer, data->max_sync_act_duration);

	// decide the state of the network
	const char* state = NULL;
	if (second_layer_mean_pulse_duration <= 0.0f || last_layer_mean_pulse_duration <= 0.0f) {
		state = "NO_ACTIVITY";
	}
	else {
		float second_last_pulse_duration_ratio = second_layer_mean_pulse_duration / last_layer_mean_pulse_duration;
		if (second_last_pulse_duration_ratio < data->min_ratio) {
			state = "EPILEPSY";
		}
		else if (second_last_pulse_duration_ratio > data->max_ratio) {
			state = "NO_ACTIVITY";
		}
		else {
			state = "STABLE";
		}
	}

	const char* output_file_path = string_get_C_string(data->output_file_path);
	FILE* fp = fopen(output_file_path, "w");
	check(fp != NULL, "Couldn't oepn file %s for writting", output_file_path);

	fprintf(fp, state);
	fclose(fp);

ERROR
	return;
}