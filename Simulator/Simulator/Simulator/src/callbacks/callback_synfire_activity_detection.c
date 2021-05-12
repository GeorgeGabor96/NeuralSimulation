#include "Callback.h"

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
void callback_detect_synfire_activity_data_run_with_syn_info(C_Data* data, Network* net);



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
	callback->data_run = callback_detect_synfire_activity_data_run_with_syn_info;

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


static inline GaussianDist* get_layer_spike_distribution(SpikeStatesForLayer* spike_states_for_layer) {
	uint32_t neuron_idx = 0;
	uint32_t time_idx = 0;
	float time_idx_f = 0.0f;
	float std = 0.0f;
	BOOL spike_state = FALSE;
	GaussianDist* dist = NULL;
	SpikeStatesForNeuron* spike_states_for_neuron = NULL;
	Array* spike_times = array_create(spike_states_for_layer->length, 0, sizeof(float));

	for (neuron_idx = 0; neuron_idx < spike_states_for_layer->length; ++neuron_idx) {
		spike_states_for_neuron = (SpikeStatesForNeuron*)array_get(spike_states_for_layer, neuron_idx);

		for (time_idx = 0; time_idx < spike_states_for_neuron->length; ++time_idx) {
			spike_state = *((BOOL*)array_get(spike_states_for_neuron, time_idx));
			
			if (spike_state == TRUE) {
				time_idx_f = (float)time_idx;
				array_append(spike_times, &time_idx_f);
			}
		}
	}

	dist = array_float_get_gaussian_dist(spike_times);
	array_destroy(spike_times, NULL);

	return dist;
}


typedef struct SynfireInfo {
	float mean_time;
	float duration;
} SynfireInfo;


static inline SynfireInfo* get_layer_synfire_info(SpikeStatesForLayer* spike_states_for_layer) {
	uint32_t neuron_idx = 0;
	uint32_t time_idx = 0;
	uint32_t* h_value = 0;
	BOOL spike_state = FALSE;
	SynfireInfo* syn_info = (SynfireInfo*)malloc(sizeof(SynfireInfo), "get_layer_synfire_info syn_info");
	SpikeStatesForNetwork* spike_states_for_neuron = (SpikeStatesForNeuron*)array_get(spike_states_for_layer, 0);
	Array* spike_cumm_hist = array_zeros_uint23(spike_states_for_neuron->length);

	// make the histogram
	for (neuron_idx = 0; neuron_idx < spike_states_for_layer->length; ++neuron_idx) {
		spike_states_for_neuron = (SpikeStatesForNeuron*)array_get(spike_states_for_layer, neuron_idx);

		for (time_idx = 0; time_idx < spike_states_for_neuron->length; ++time_idx) {
			spike_state = *((BOOL*)array_get(spike_states_for_neuron, time_idx));
			if (spike_state == TRUE) {
				h_value = (uint32_t*)array_get(spike_cumm_hist, time_idx);
				++(*h_value);
			}
		}
	}

	// find the portion with synfire chain
	// take a +- 2 ms window at each point, if there are like +10 neurons theyn syncron
	// if we cannot find a window for 10 ms then it is over
	uint32_t window_size = 2;
	uint32_t start = 0;
	uint32_t end = 0;
	uint32_t spikes_in_window = 0;
	uint32_t window_start = 0;
	uint32_t window_end = 0;
	uint32_t window_pos = 0;
	uint32_t last_syn_window = 0;
	BOOL syncrony = FALSE;


	for (time_idx = 0; time_idx < spike_cumm_hist->length; ++time_idx) {
		spikes_in_window = 0;

		// compute window value
		window_start = time_idx - window_size;
		window_start = window_start < 0 ? 0 : window_start;

		window_end = time_idx + window_size;
		window_end = window_end >= spike_cumm_hist->length ? spike_cumm_hist->length - 1 : window_end;

		for (window_pos = window_start; window_pos <= window_end; ++window_pos)
			spikes_in_window += *((uint32_t*)array_get(spike_cumm_hist, window_pos));

		if (spikes_in_window >= 10) {
			// check if pulse starts
			if (syncrony == FALSE) {
				start = time_idx;
				syncrony = TRUE;
			}
			last_syn_window = time_idx;
		}
		else if (syncrony == TRUE) {
			// check if pulse ends, if more than 10 ms passed since last sync then its over
			if (time_idx - last_syn_window > 10) {
				end = last_syn_window;
				break;
			}
		}

	}
	array_destroy(spike_cumm_hist, NULL);

	// modify @end if the activity doesn't end
	if (syncrony == TRUE && end == 0) end = last_syn_window;

	// if no syncrony no duration
	if (syncrony == FALSE) syn_info->duration = 0;
	else syn_info->duration = (float)end - (float)start + 1.0f;

	syn_info->mean_time = (float)start + syn_info->duration / 2.0f;
	return syn_info;
}

void callback_detect_synfire_activity_data_run_with_syn_info(C_Data* data, Network* net) {
	check(callback_detect_synfire_activity_data_is_valid(data) == TRUE, invalid_argument("data"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));

	SpikeStatesForNetwork* spike_states_for_net = &(data->spike_states_for_network);

	// TODO: let the user decide the two layers
	SpikeStatesForLayer* spike_states_second_layer = (SpikeStatesForLayer*)array_get(spike_states_for_net, 4);
	SpikeStatesForLayer* spike_states_last_layer = (SpikeStatesForLayer*)array_get(spike_states_for_net, spike_states_for_net->length - 1);

	SynfireInfo* layer_1_syn_info = get_layer_synfire_info(spike_states_second_layer);
	SynfireInfo* layer_2_syn_info = get_layer_synfire_info(spike_states_last_layer);

	const char* state = "STABLE";

	float layers_duration_ratio = 0.0f;
	// when at least one is fully syncronized
	if (layer_1_syn_info->duration == 0.0f || layer_2_syn_info->duration == 0.0f) {
		state = "NO_ACTIVITY";
	}
	else {
		layers_duration_ratio = layer_1_syn_info->duration / (layer_2_syn_info->duration + EPSILON);

		// FIRST IF MAY BE REDUNDANT
		if (layer_1_syn_info->mean_time == 0.0f || layer_2_syn_info->mean_time == 0.0f) {
			state = "NO_ACTIVITY";
		}
		else if (layers_duration_ratio < data->min_ratio) {
			state = "EPILEPSY";
		}
		else if (layers_duration_ratio > data->max_ratio) {
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
	fprintf(fp, "\nduration1: %.10f duration2: %.10f ratio %.10f\n", layer_1_syn_info->duration, layer_2_syn_info->duration, layers_duration_ratio);
	fprintf(fp, "mean_t1: %.10f mean_t2: %.10f\n", layer_1_syn_info->mean_time, layer_2_syn_info->mean_time);

	fclose(fp);

	free(layer_1_syn_info);
	free(layer_2_syn_info);

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
	uint32_t neurons_with_pulses = 0;
	uint32_t max_duration = 0;
	uint32_t duration = 0;
	BOOL spike_state = FALSE;

	uint32_t min_spike_time = UINT32_MAX;
	uint32_t max_spike_time = 0;

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

				if (time_idx < min_spike_time) min_spike_time = time_idx;
				if (last_spike > max_spike_time) max_spike_time = last_spike;

				// add the duration
				duration = last_spike - time_idx + 1;
				total_neurons_pulse_duration += duration;
				++neurons_with_pulses;
				// go to next neuron
				if (duration > max_duration) {
					max_duration = duration;
				}

				break;
			}
		}
	}
	// TODO: Ask Raul how to define a synfire chain, is this ok?
	float score1 = max_spike_time == 0 ? 0.0f : (float)(max_spike_time - min_spike_time);
	float score2 = (float)max_duration;
	float score3 = neurons_with_pulses == 0 ? 0.0f : (float)total_neurons_pulse_duration / (float)(neurons_with_pulses);
	//printf("%f %f %f\n", score1, score2, score3);
	return score1; //(score1 + score2) / 2.0f;
}


void callback_detect_synfire_activity_data_run(C_Data* data, Network* net) {
	check(callback_detect_synfire_activity_data_is_valid(data) == TRUE, invalid_argument("data"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));

	/* NOTE
		currently to compute the state of the synfire chain we compare the activity from the second layer and the last layer
		TODO allow the user to speciy what layers to compare
	*/
	SpikeStatesForNetwork* spike_states_for_net = &(data->spike_states_for_network);
	
	// TODO: let the user decide the two layers
	SpikeStatesForLayer* spike_states_second_layer = (SpikeStatesForLayer*)array_get(spike_states_for_net, 4);
	SpikeStatesForLayer* spike_states_last_layer = (SpikeStatesForLayer*)array_get(spike_states_for_net, spike_states_for_net->length - 1);

	GaussianDist* layer_1_dist = get_layer_spike_distribution(spike_states_second_layer);
	GaussianDist* layer_2_dist = get_layer_spike_distribution(spike_states_last_layer);
	
	//printf("\n%f %f\n%f %f\n", layer_1_dist->mean, layer_1_dist->std, layer_2_dist->mean, layer_2_dist->std);

	const char* state = NULL;
	float layers_std_ratio = 0.0f;
	// when at least one is fully syncronized
	if (layer_1_dist->std == 0.0f || layer_2_dist->std == 0.0f) {
		layers_std_ratio = (layer_1_dist->std + 1.0f) / (layer_2_dist->std + 1.0f);
	}
	else {
		layers_std_ratio = layer_1_dist->std / (layer_2_dist->std + EPSILON);
	}
	
	if (layer_2_dist->mean == 0.0f || layer_1_dist->mean == 0.0f) {
		state = "NO_ACTIVITY";
	}
	else if (layers_std_ratio < data->min_ratio) {
		state = "EPILEPSY";
	}
	else if (layers_std_ratio > data->max_ratio) {
		state = "NO_ACTIVITY";
	}
	else {
		state = "STABLE";
	}
	//printf("%.10f %.10f\n", layer_1_dist->mean, layer_1_dist->std);
	//printf("%.10f %.10f\n", layer_2_dist->mean, layer_2_dist->std);
	//printf("%s\n", state);
	/*
	float second_layer_mean_pulse_duration = get_layer_pulse_mean_duration(spike_states_second_layer, data->max_sync_act_duration);
	float last_layer_mean_pulse_duration = get_layer_pulse_mean_duration(spike_states_last_layer, data->max_sync_act_duration);

	// decide the state of the network
	const char* state = NULL;
	float second_last_pulse_duration_ratio = second_layer_mean_pulse_duration / (last_layer_mean_pulse_duration + 0.00001f);
	printf("%f %f %f\n", second_layer_mean_pulse_duration, last_layer_mean_pulse_duration, second_last_pulse_duration_ratio);
	if (last_layer_mean_pulse_duration == 0) {
		state = "NO_ACTIVITY";
	}
	else if (second_last_pulse_duration_ratio < data->min_ratio) {
		state = "EPILEPSY";
	}
	else if (second_last_pulse_duration_ratio > data->max_ratio) {
		state = "NO_ACTIVITY";
	}
	else {
		state = "STABLE";
	}
	*/
	const char* output_file_path = string_get_C_string(data->output_file_path);
	FILE* fp = fopen(output_file_path, "w");
	check(fp != NULL, "Couldn't oepn file %s for writting", output_file_path);

	fprintf(fp, state);
	//fprintf(fp, "\n%f %f %f\n", second_layer_mean_pulse_duration, last_layer_mean_pulse_duration, second_last_pulse_duration_ratio);
	fprintf(fp, "\nstd1: %.10f std2: %.10f ratio %.10f\n", layer_1_dist->std, layer_2_dist->std, layers_std_ratio);
	fprintf(fp, "mean1: %.10f mean2: %.10f\n", layer_1_dist->mean, layer_2_dist->mean);

	fclose(fp);

	free(layer_1_dist);
	free(layer_2_dist);

ERROR
	return;
}