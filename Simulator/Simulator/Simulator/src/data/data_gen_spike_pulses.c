#include "utils/MemoryManagement.h"
#include "data/data_gen_random_spikes.h"



/*************************************************************
* DATA ELEMENT FUNCTIONALITY
*************************************************************/
typedef struct DataElementData {
	uint32_t first_pulse_timestamp;
	uint32_t between_pulses_duration;
	uint32_t pulse_duration;
	float between_pulses_spike_frequency;
	float pulse_spike_frequency;
	uint32_t last_transition_timestep;		// last time that i changed between a pulse and non-pulse, or vice versa
	BOOL in_pulse;							// am I in a pulse?
	Network* net;
}DataElementData;

BOOL data_element_spike_pulses_data_is_valid(DataElementData* data);
void data_element_spike_pulses_data_destroy(DataElementData* data);
NetworkInputs* data_element_spike_pulses_get_values(DataElement* element, uint32_t time);


// need the network be know how many inputs to generate and for each input how many currents to make
DataElement* data_element_spike_pulses_create(
		Network* net, 
		uint32_t first_pulse_timestamp,
		uint32_t between_pulses_duration,
		uint32_t pulse_duration,
		float between_pulses_spike_frequency,
		float pulse_spike_frequency,
		uint32_t duration) {

	DataElement* element = NULL;
	DataElementData* data = NULL;
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	check(duration > 0, "@duration == 0");
	check(first_pulse_timestamp < duration, "@first_pulse_timestamp >= @duration, should be lower, or else no pulses");
	check(between_pulses_duration > 0, "@between_pulses_duration == 0");
	check(pulse_duration > 0, "@pulse_duration == 0");
	check(between_pulses_spike_frequency >= 0.0f, "@between_pulses_spike_frequency < 0.0f");
	check(between_pulses_spike_frequency <= 1.0f, "@between_pulses_spike_frequency > 1.0f");
	check(pulse_spike_frequency >= 0.0f, "@pulse_spike_frequency < 0.0f");
	check(pulse_spike_frequency <= 1.0f, "@pulse_spike_frequency > 1.0f");

	data = (DataElementData*)malloc(sizeof(DataElementData), "data_element_random_spikes_create->data");
	check_memory(data);
	data->net = net;
	data->first_pulse_timestamp = first_pulse_timestamp;
	data->between_pulses_duration = between_pulses_duration;
	data->pulse_duration = pulse_duration;
	data->between_pulses_spike_frequency = between_pulses_spike_frequency;
	data->pulse_spike_frequency = pulse_spike_frequency;
	data->last_transition_timestep = 0;
	data->in_pulse = FALSE;

	element = (DataElement*)malloc(sizeof(DataElement), "data_element_random_spikes_create->element");
	check_memory(element);
	element->duration = duration;
	element->data = data;
	element->data_is_valid = data_element_spike_pulses_data_is_valid;
	element->data_destroy = data_element_spike_pulses_data_destroy;
	element->get_values = data_element_spike_pulses_get_values;
	element->remove_values = data_element_base_remove_values;

	return element;

	ERROR
		if (element != NULL) free(element);
	if (data != NULL) free(data);
	return NULL;
}

BOOL data_element_spike_pulses_data_is_valid(DataElementData* data) {
	check(network_is_valid(data->net) == TRUE, invalid_argument("net"));
	check(data->first_pulse_timestamp < duration, "@first_pulse_timestamp >= @duration, should be lower, or else no pulses");
	check(data->between_pulses_duration > 0, "@between_pulses_duration == 0");
	check(data->pulse_duration > 0, "@pulse_duration == 0");
	check(data->between_pulses_spike_frequency >= 0.0f, "@between_pulses_spike_frequency < 0.0f");
	check(data->between_pulses_spike_frequency <= 1.0f, "@between_pulses_spike_frequency > 1.0f");
	check(data->pulse_spike_frequency >= 0.0f, "@pulse_spike_frequency < 0.0f");
	check(data->pulse_spike_frequency <= 1.0f, "@pulse_spike_frequency > 1.0f");

	check(data->in_pulse == FALSE || data->in_pulse == TRUE, invalid_argument("data->in_pulse"));
	// TODO FIX COMMENTS

	return TRUE;
ERROR
	return FALSE;
}


void data_element_spike_pulses_data_destroy(DataElementData* data) {
	data->net = NULL;
	data->first_pulse_timestamp = 0;
	data->between_pulses_duration = 0;
	data->pulse_duration = 0;
	data->between_pulses_spike_frequency = 0.0f;
	data->pulse_spike_frequency = 0.0f;
	data->in_pulse = FALSE;
	data->last_transition_timestep = 0;
	free(data);
}


NetworkInputs* data_element_spike_pulses_get_values(DataElement* element, uint32_t time) {
	Status status = FAIL;
	NetworkInputs* inputs = NULL;
	DataElementData* data = NULL;
	Network* net = NULL;
	Layer* layer = NULL;
	NetworkValues net_input = { 0 };
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t event_duration = 0;
	BOOL spike = FALSE;
	float spikes_percent = 0.0f;

	(time);
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

	data = (DataElementData*)element->data;
	net = data->net;

	inputs = array_create(net->input_layers.length, 0, sizeof(NetworkValues));
	check(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
	// for each network input
	for (i = 0; i < net->input_layers.length; ++i) {
		layer = *((Layer**)array_get(&(net->input_layers), i));
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

		status = array_init(&(net_input.values), layer->neurons.length, layer->neurons.length, sizeof(BOOL));
		check(status == SUCCESS, invalid_argument("status"));
		for (j = 0; j < layer->neurons.length; ++j) {
			
			// check if the first pulse should start
			if (data->first_pulse_timestamp == time) {
				data->in_pulse = TRUE;
				data->last_transition_timestep = time;
				spikes_percent = data->pulse_spike_frequency;
			}
			// didn't get to the first pulse
			else if (time < data->first_pulse_timestamp) {
				spikes_percent = data->between_pulses_spike_frequency;
			}
			// after time > data->first_pulse_timestamp
			else {
				// for how long are we in the current event
				event_duration = time - data->last_transition_timestep;

				// in pulse mode or not
				if (data->in_pulse == TRUE) {
					// check if pulse is over
					if (event_duration >= data->pulse_duration) {
						// change to non pulsing
						data->in_pulse = FALSE;
						data->last_transition_timestep = time;
					}
				}
				else {
					if (event_duration >= data->between_pulses_duration) {
						// change to pulsing
						data->in_pulse = TRUE;
						data->last_transition_timestep = time;
					}
				}

				spikes_percent = data->in_pulse == TRUE ? data->pulse_spike_frequency : data->between_pulses_spike_frequency;
			}
			
			spike = (double)rand() / (double)RAND_MAX <= spikes_percent ? TRUE : FALSE;
			array_set(&(net_input.values), j, &spike);
		}
		net_input.type = SPIKES;

		status = array_append(inputs, &net_input);
		check(status == SUCCESS, invalid_argument("status"));
	}
	return inputs;

	ERROR
		if (inputs != NULL)
			element->remove_values(element, inputs);
	return NULL;
}



/*************************************************************
* DATA GENERATOR FUNCTIONALITY
*************************************************************/
typedef struct DataGeneratorData {
	uint32_t first_pulse_timestamp;
	uint32_t between_pulses_duration;
	uint32_t pulse_duration;
	float between_pulses_spike_frequency;
	float pulse_spike_frequency;
	uint32_t duration;
	Network* net;
} DataGeneratorData;

BOOL data_generator_spike_pulses_is_valid(DataGeneratorData* data);
void data_generator_spike_pulses_destroy(DataGeneratorData* data);
DataElement* data_generator_spike_pulses_get_elem(DataGeneratorData* data, uint32_t idx);


DataGenerator* data_generator_spike_pulses_create(
		uint32_t n_examples,
		Network* net,
		uint32_t first_pulse_timestamp,
		uint32_t between_pulses_duration,
		uint32_t pulse_duration,
		float between_pulses_spike_frequency,
		float pulse_spike_frequency,
		uint32_t duration) {

	DataGeneratorData* data = NULL;
	DataGenerator* data_gen = NULL;
	check(n_examples > 0, "@n_examples == 0");
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	check(duration > 0, "@duration == 0");
	check(first_pulse_timestamp < duration, "@first_pulse_timestamp >= @duration, should be lower, or else no pulses");
	check(between_pulses_duration > 0, "@between_pulses_duration == 0");
	check(pulse_duration > 0, "@pulse_duration == 0");
	check(between_pulses_spike_frequency >= 0.0f, "@between_pulses_spike_frequency < 0.0f");
	check(between_pulses_spike_frequency <= 1.0f, "@between_pulses_spike_frequency > 1.0f");
	check(pulse_spike_frequency >= 0.0f, "@pulse_spike_frequency < 0.0f");
	check(pulse_spike_frequency <= 1.0f, "@pulse_spike_frequency > 1.0f");

	data = (DataGeneratorData*)malloc(sizeof(DataGeneratorData), "data_generator_spike_pulses_create->data");
	check_memory(data);
	data->net = net;
	data->duration = duration;
	data->first_pulse_timestamp = first_pulse_timestamp;
	data->between_pulses_duration = between_pulses_duration;
	data->pulse_duration = pulse_duration;
	data->between_pulses_spike_frequency = between_pulses_spike_frequency;
	data->pulse_spike_frequency= pulse_spike_frequency;

	data_gen = (DataGenerator*)malloc(sizeof(DataGenerator), "data_generator_random_spikes_create->data_gen");
	check_memory(data_gen);
	data_gen->length = n_examples;
	data_gen->data = data;
	data_gen->data_is_valid = data_generator_spike_pulses_is_valid;
	data_gen->data_destroy = data_generator_spike_pulses_destroy;
	data_gen->get_elem = data_generator_spike_pulses_get_elem;

	// random seed for spike generation
	time_t t;
	srand((unsigned)time(&t));
	return data_gen;

	ERROR
		if (data != NULL) free(data);
	if (data_gen != NULL) free(data_gen);

	return NULL;
}


BOOL data_generator_spike_pulses_is_valid(DataGeneratorData* data) {
	check(data->duration > 0, "@data->duration == 0");
	check(network_is_valid(data->net) == TRUE, invalid_argument("net"));
	check(data->duration > 0, "@duration == 0");
	check(data->first_pulse_timestamp < duration, "@first_pulse_timestamp >= @duration, should be lower, or else no pulses");
	check(data->between_pulses_duration > 0, "@between_pulses_duration == 0");
	check(data->pulse_duration > 0, "@pulse_duration == 0");
	check(data->between_pulses_spike_frequency >= 0.0f, "@between_pulses_spike_frequency < 0.0f");
	check(data->between_pulses_spike_frequency <= 1.0f, "@between_pulses_spike_frequency > 1.0f");
	check(data->pulse_spike_frequency >= 0.0f, "@pulse_spike_frequency < 0.0f");
	check(data->pulse_spike_frequency <= 1.0f, "@pulse_spike_frequency > 1.0f");

	return TRUE;
ERROR
	return FALSE;
}

void data_generator_spike_pulses_destroy(DataGeneratorData* data) {
	data->duration = 0;
	data->net = NULL;
	data->first_pulse_timestamp = 0;
	data->between_pulses_duration = 0;
	data->pulse_duration = 0;
	data->between_pulses_spike_frequency = 0.0f;
	data->pulse_spike_frequency = 0.0f;
	free(data);
}


DataElement* data_generator_spike_pulses_get_elem(DataGeneratorData* data, uint32_t idx) {
	return data_element_spike_pulses_create(
		data->net,
		data->first_pulse_timestamp,
		data->between_pulses_duration,
		data->pulse_duration,
		data->between_pulses_spike_frequency,
		data->pulse_spike_frequency,
		data->duration);
}