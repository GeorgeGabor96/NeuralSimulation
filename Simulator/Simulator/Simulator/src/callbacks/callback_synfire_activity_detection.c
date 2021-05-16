#include "Callback.h"

#include "utils/MemoryManagement.h"
#include "Containers.h"


// helper definitions
typedef Array SpikeStatesForNeuron;	// contains the state of the spike variable at each timestamp
typedef Array SpikeStatesForLayer;	// contains @SpikeStatesForNeuron's
typedef Array SpikeStatesForNetwork; // contains @SpikeStatesForLayer's


typedef struct C_Data {
	uint32_t first_layer_idx;
	uint32_t second_layer_idx;
	SpikeStatesForLayer spike_states_first_layer;
	SpikeStatesForLayer spike_states_second_layer;
	float min_ratio;
	float max_ratio;
	String* output_file_path;
} C_Data;


BOOL callback_detect_synfire_activity_data_is_valid(C_Data* data);
void callback_detect_synfire_activity_data_destroy(C_Data* data);
void callback_detect_synfire_activity_data_update(C_Data* data, Network* net);
void callback_detect_synfire_activity_data_run_std(C_Data* data, Network* net);
void callback_detect_synfire_activity_data_run_fp_duration(C_Data* data, Network* net);


BOOL synfire_detection_mode_is_valid(SYNFIRE_DETECTION_MODE detection_mode) {
	check(detection_mode == SYNFIRE_STD || detection_mode == SYNFIRE_FP_DURATION, "@detection_mode is %u", detection_mode);
	return TRUE;
ERROR
	return FALSE;
}


Callback* callback_detect_synfire_activity_create(
		Network* net,
		SYNFIRE_DETECTION_MODE detection_mode,
		uint32_t first_layer_idx,
		uint32_t second_layer_idx,
		float min_ratio,
		float max_ratio,
		const char* output_file_path) {
	Callback* callback = NULL;
	Layer* layer = NULL;
	SpikeStatesForLayer* spike_states_for_layer = NULL;
	C_Data* data = NULL;
	Status status = FAIL;
	uint32_t i = 0;

	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	check(synfire_detection_mode_is_valid(detection_mode) == TRUE, invalid_argument("detection_mode"));
	check(first_layer_idx < net->layers.length, invalid_argument("first_layer_idx"));
	check(second_layer_idx < net->layers.length, invalid_argument("second_layer_idx"));
	check(first_layer_idx != second_layer_idx, "first_layer_idx == second_layer_idx");
	check(min_ratio > 0.0f, "min_ratio <= 0.0f");
	check(max_ratio > 0.0f, "max_ratio <= 0.0f");
	check(output_file_path != NULL, null_argument("output_file_path"));

	data = (C_Data*)calloc(1, sizeof(C_Data), "callback_detect_synfire_activity_create data");
	check_memory(data);

	callback = (Callback*)calloc(1, sizeof(Callback), "callback_detect_synfire_activity_create callback");
	check_memory(callback);

	Layer* first_layer = (Layer*)array_get(&(net->layers), first_layer_idx);
	status = array_of_arrays_init(&(data->spike_states_first_layer), first_layer->neurons.length, sizeof(BOOL));
	check(status == SUCCESS, "status is %u", status);

	Layer* second_layer = (Layer*)array_get(&(net->layers), second_layer_idx);
	status = array_of_arrays_init(&(data->spike_states_second_layer), second_layer->neurons.length, sizeof(BOOL));
	check(status == SUCCESS, "status is %u", status);

	data->first_layer_idx = first_layer_idx;
	data->second_layer_idx = second_layer_idx;
	data->min_ratio = min_ratio;
	data->max_ratio = max_ratio;
	data->output_file_path = string_create(output_file_path);
	check(string_is_valid(data->output_file_path) == TRUE, invalid_argument("data->output_file_path"));

	callback->data = data;
	callback->data_is_valid = callback_detect_synfire_activity_data_is_valid;
	callback->data_destroy = callback_detect_synfire_activity_data_destroy;
	callback->data_update = callback_detect_synfire_activity_data_update;
	if (detection_mode == SYNFIRE_STD) {
		callback->data_run = callback_detect_synfire_activity_data_run_std;
	}
	else if (detection_mode == SYNFIRE_FP_DURATION) {
		callback->data_run = callback_detect_synfire_activity_data_run_fp_duration;
	}
	else {
		// this should never happen
		log_error(invalid_argument("detection_mode"));
	}

	return callback;

ERROR
	if (data != NULL) {
		if (data->output_file_path != NULL) string_destroy(data->output_file_path);
		if (array_is_valid(&(data->spike_states_first_layer)) == TRUE) array_of_arrays_reset(&(data->spike_states_first_layer));
		if (array_is_valid(&(data->spike_states_second_layer)) == TRUE) array_of_arrays_reset(&(data->spike_states_second_layer));
		free(data);
	}
	free(callback);
	return NULL;
}


BOOL spike_states_for_layer_is_valid(SpikeStatesForLayer* spike_states_for_layer) {
	check(array_is_valid(spike_states_for_layer) == TRUE, invalid_argument("spike_states_for_layer"));
	uint32_t neuron_idx = 0;
	SpikeStatesForNeuron* spike_states_for_neuron = NULL;
	for (neuron_idx = 0; neuron_idx < spike_states_for_layer->length; ++neuron_idx) {
		spike_states_for_neuron = (SpikeStatesForNeuron*)array_get(spike_states_for_layer, neuron_idx);
		check(array_is_valid(spike_states_for_neuron) == TRUE, invalid_argument("spike_states_for_neuron"));
	}
	return TRUE;
ERROR
	return FALSE;
}

BOOL callback_detect_synfire_activity_data_is_valid(C_Data* data) {
	check(data != NULL, null_argument("data"));
	// cannot check for first_layer_idx and second_layer_idx
	check(data->min_ratio > 0.0f, invalid_argument("data->min_ratio"));
	check(data->max_ratio > 0.0f, invalid_argument("data->max_ratio"));
	check(string_is_valid(data->output_file_path) == TRUE, invalid_argument("data->output-file_paht"));

	check(spike_states_for_layer_is_valid(&(data->spike_states_first_layer)) == TRUE, invalid_argument("data->spike_states_first_layer"));
	check(spike_states_for_layer_is_valid(&(data->spike_states_second_layer)) == TRUE, invalid_argument("data->spike_states_second_layer"));

	return TRUE;
ERROR
	return FALSE;
}


void callback_detect_synfire_activity_data_destroy(C_Data* data) {
	check(callback_detect_synfire_activity_data_is_valid(data) == TRUE, invalid_argument("data"));
	uint32_t layer_idx = 0;
	SpikeStatesForLayer* spike_states_for_layer = NULL;

	string_destroy(data->output_file_path);
	array_of_arrays_reset(&(data->spike_states_first_layer));
	array_of_arrays_reset(&(data->spike_states_second_layer));
	memset(data, 0, sizeof(C_Data));
	free(data);

ERROR
	return;
}


static inline void update_spike_states_for_layer(SpikeStatesForLayer* spike_states_for_layer, Layer* layer) {
	Neuron* neuron = NULL;
	uint32_t neuron_idx = 0;
	SpikeStatesForNeuron* spike_states_for_neuron = NULL;

	for (neuron_idx = 0; neuron_idx < layer->neurons.length; ++neuron_idx) {
		neuron = (Neuron*)array_get(&(layer->neurons), neuron_idx);
		spike_states_for_neuron = (SpikeStatesForNeuron*)array_get(spike_states_for_layer, neuron_idx);

		// save the spike state
		array_append(spike_states_for_neuron, &(neuron->spike));
	}
}

void callback_detect_synfire_activity_data_update(C_Data* data, Network* net) {
	check(callback_detect_synfire_activity_data_is_valid(data) == TRUE, invalid_argument("data"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	check(data->first_layer_idx < net->layers.length, invalid_argument("data->first_layer_idx"));
	check(data->second_layer_idx < net->layers.length, invalid_argument("data->second_layer_idx"));

	SpikeStatesForLayer* spike_states_for_layer = NULL;

	Layer* first_layer = (Layer*)array_get(&(net->layers), data->first_layer_idx);
	update_spike_states_for_layer(&(data->spike_states_first_layer), first_layer);

	Layer* second_layer = (Layer*)array_get(&(net->layers), data->second_layer_idx);
	update_spike_states_for_layer(&(data->spike_states_second_layer), second_layer);

ERROR
	return;
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
	// take a +- 2 ms window at each point, if there are like +10 neurons then syncron
	// if we cannot find a window for 10 ms then it is over
	// POTENTIAL TODO: those 3 could be given by the user
	uint32_t window_size = 2;
	uint32_t min_spikes_in_window = 10;
	uint32_t max_no_activity_duration = 10;

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

		if (spikes_in_window >= min_spikes_in_window) {
			// check if pulse starts
			if (syncrony == FALSE) {
				start = time_idx;
				syncrony = TRUE;
			}
			last_syn_window = time_idx;
		}
		else if (syncrony == TRUE) {
			// check if pulse ends, if more than 10 ms passed since last sync then its over
			if (time_idx - last_syn_window > max_no_activity_duration) {
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

void callback_detect_synfire_activity_data_run_fp_duration(C_Data* data, Network* net) {
	check(callback_detect_synfire_activity_data_is_valid(data) == TRUE, invalid_argument("data"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));

	SynfireInfo* layer_1_syn_info = get_layer_synfire_info(&(data->spike_states_first_layer));
	SynfireInfo* layer_2_syn_info = get_layer_synfire_info(&(data->spike_states_second_layer));

	const char* state = "STABLE";

	float layers_duration_ratio = 0.0f;
	// when at least one is dead
	if (layer_1_syn_info->duration == 0.0f || layer_2_syn_info->duration == 0.0f) {
		state = "NO_ACTIVITY";
	}
	else {
		layers_duration_ratio = layer_1_syn_info->duration / (layer_2_syn_info->duration + EPSILON);

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

void callback_detect_synfire_activity_data_run_std(C_Data* data, Network* net) {
	check(callback_detect_synfire_activity_data_is_valid(data) == TRUE, invalid_argument("data"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));

	GaussianDist* layer_1_dist = get_layer_spike_distribution(&(data->spike_states_first_layer));
	GaussianDist* layer_2_dist = get_layer_spike_distribution(&(data->spike_states_second_layer));
	
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

	const char* output_file_path = string_get_C_string(data->output_file_path);
	FILE* fp = fopen(output_file_path, "w");
	check(fp != NULL, "Couldn't oepn file %s for writting", output_file_path);

	fprintf(fp, state);
	fprintf(fp, "\nstd1: %.10f std2: %.10f ratio %.10f\n", layer_1_dist->std, layer_2_dist->std, layers_std_ratio);
	fprintf(fp, "mean1: %.10f mean2: %.10f\n", layer_1_dist->mean, layer_2_dist->mean);

	fclose(fp);

	free(layer_1_dist);
	free(layer_2_dist);

ERROR
	return;
}



// this is not used anywhere and was experimental
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

	return score1; //(score1 + score2) / 2.0f;
}
