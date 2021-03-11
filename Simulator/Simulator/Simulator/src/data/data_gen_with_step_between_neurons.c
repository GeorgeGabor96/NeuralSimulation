#include "utils/MemoryManagement.h"
#include "data/data_gen_random_spikes.h"


/*************************************************************
* DATA ELEMENT FUNCTIONALITY
*************************************************************/
BOOL data_element_with_step_between_neurons_is_valid(DataElement* element);
void data_element_with_step_between_neurons_destroy(DataElement* element);
NetworkInputs* data_element_with_step_between_neurons_get_values(DataElement* element, uint32_t time);
void data_element_with_step_between_neurons_remove_values(DataElement* element, NetworkInputs* inputs);


typedef struct DataElementData {
	ArrayUint32 inputs_spikes_times;
}DataElementData;


// need the network be know how many inputs to generate and for each input how many currents to make
DataElement* data_element_with_step_between_neurons_create(Network* net, uint32_t step_between_neurons, uint32_t duration) {
	DataElement* element = NULL;
	DataElementData* data = NULL;
	uint32_t i = 0;
	uint32_t j = 0;
	Layer* layer = NULL;
	Status status = FAIL;
	uint32_t spike_time = 0;

	check(network_is_valid(net) == TRUE, invalid_argument("network"));
	check(duration > 0, "@duration == 0");
	check(step_between_neurons > 0, "@step_between_neurons == 0");

	data = (DataElementData*)malloc(sizeof(DataElementData), "data_element_random_spikes_create->data");
	check_memory(data);

	status = array_init(&(data->inputs_spikes_times), net->input_layers.length, net->input_layers.length, sizeof(Array));
	check(status == SUCCESS, "Couldn't init @data->inputs_spikes_time");

	for (i = 0; i < net->input_layers.length; ++i) {
		layer = *((Layer**)array_get(&(net->input_layers), i));
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
		
		status = array_init(array_get(&(data->inputs_spikes_times), i), layer->neurons.length, layer->neurons.length, sizeof(uint32_t));
		check(status == SUCCESS, "Couldn't init @inputs_spikes_times %u", i);

		// for each neuron set the simulation time when it should spike
		for (j = 0; j < layer->neurons.length; ++j) {
			array_set(array_get(&(data->inputs_spikes_times), i), j, &spike_time);
			spike_time += step_between_neurons;
		}
	}

	element = (DataElement*)malloc(sizeof(DataElement), "data_element_random_spikes_create->element");
	check_memory(element);
	element->duration = duration;
	element->data = data;
	element->is_valid = data_element_with_step_between_neurons_is_valid;
	element->destroy = data_element_with_step_between_neurons_destroy;
	element->get_values = data_element_with_step_between_neurons_get_values;
	element->remove_values = data_element_with_step_between_neurons_remove_values;

	return element;

ERROR
	if (element != NULL) free(element);
	if (data != NULL) { 
		while (i != 0) {
			i--;
			array_reset(array_get(&(data->inputs_spikes_times), i), NULL);
		}
		array_reset(&(data->inputs_spikes_times), NULL);
		free(data); 
	}
	return NULL;
}


BOOL data_element_with_step_between_neurons_is_valid(DataElement* element) {
	check(element != NULL, null_argument("element"));
	check(element->duration > 0, "@element->duration == 0");
	check(element->data != NULL, null_argument("element->data"));
	check(element->is_valid != NULL, null_argument("element->is_valid"));
	check(element->destroy != NULL, null_argument("element->destroy"));
	check(element->get_values != NULL, null_argument("element->get_values"));

	DataElementData* data = (DataElementData*)element->data;
	check(array_is_valid(&(data->inputs_spikes_times)) == TRUE, invalid_argument("data->inputs_spikes_times"));
	for (uint32_t i = 0; i < data->inputs_spikes_times.length; i++) 
		check(array_is_valid(array_get(&(data->inputs_spikes_times), i)) == TRUE, "Array %u in @data->inputs_spikes_times is invalid", i);

	return TRUE;
ERROR
	return FALSE;
}


void data_element_with_step_between_neurons_destroy(DataElement* element) {
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

	DataElementData* data = (DataElementData*)element->data;
	
	for (uint32_t i = 0; i < data->inputs_spikes_times.length; ++i)
		array_reset(array_get(&(data->inputs_spikes_times), i), NULL);
	array_reset(&(data->inputs_spikes_times), NULL);
	free(data);

	element->duration = 0;
	element->data = NULL;
	element->is_valid = NULL;
	element->destroy = NULL;
	element->get_values = NULL;
	free(element);

ERROR
	return;
}


NetworkInputs* data_element_with_step_between_neurons_get_values(DataElement* element, uint32_t time) {
	Status status = FAIL;
	NetworkInputs* inputs = NULL;
	DataElementData* data = NULL;
	NetworkValues net_input = { 0 };
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t spike_time = 0;
	BOOL spike = FALSE;
	ArrayUint32* spike_times = NULL;

	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

	data = (DataElementData*)element->data;

	inputs = array_create(data->inputs_spikes_times.length, 0, sizeof(NetworkValues));
	check(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
	// for each network input
	for (i = 0; i < data->inputs_spikes_times.length; ++i) {
		spike_times = array_get(&(data->inputs_spikes_times), i);
		check(array_is_valid(spike_times) == TRUE, "%s for %u idx", invalid_argument("spike_times"), i);

		status = array_init(&(net_input.values), spike_times->length, spike_times->length, sizeof(BOOL));
		check(status == SUCCESS, invalid_argument("status"));
		for (j = 0; j < spike_times->length; ++j) {
			// check if simulation time has the value of the spike time for the neuron
			spike_time = *((uint32_t*)array_get(spike_times, j));
			spike = (spike_time == time ? TRUE : FALSE);
			array_set(&(net_input.values), j, &spike);
		}
		net_input.type = SPIKES;

		// shallow copy
		status = array_append(inputs, &net_input);
		check(status == SUCCESS, invalid_argument("status"));
	}
	return inputs;

ERROR
	if (inputs != NULL)
		data_element_with_step_between_neurons_remove_values(element, inputs);
	return NULL;
}

// duplicate code
void data_element_with_step_between_neurons_remove_values(DataElement* element, NetworkInputs* inputs) {
	(element);
	check(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
	uint32_t i = 0;
	NetworkValues* net_vals = NULL;

	for (i = 0; i < inputs->length; ++i) {
		net_vals = (NetworkValues*)array_get(inputs, i);
		net_vals->type = 0;
		array_reset(&(net_vals->values), NULL);
	}
	array_destroy(inputs, NULL);

ERROR
	return;
}


/*************************************************************
* DATA GENERATOR FUNCTIONALITY
*************************************************************/
BOOL data_generator_with_step_between_neurons_is_valid(DataGenerator* data);
void data_generator_with_step_between_neurons_destroy(DataGenerator* data);
DataElement* data_generator_with_step_between_neurons_get_elem(DataGenerator* data, uint32_t idx);

typedef struct DataGeneratorData {
	uint32_t step_between_neurons;
	uint32_t duration;
	Network* net;
} DataGeneratorData;


DataGenerator* data_generator_with_step_between_neurons_create(uint32_t n_examples, Network* net, uint32_t step_between_neurons, uint32_t duration) {
	DataGeneratorData* data = NULL;
	DataGenerator* data_gen = NULL;
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	check(duration > 0, "@duration == 0");
	check(n_examples > 0, "@n_examples == 0");
	check(step_between_neurons > 0, "@step_between_neurons == 0");

	data = (DataGeneratorData*)malloc(sizeof(DataGeneratorData), "data_generator_random_spikes_create->data");
	check_memory(data);
	data->step_between_neurons = step_between_neurons;
	data->duration = duration;
	data->net = net;

	data_gen = (DataGenerator*)malloc(sizeof(DataGenerator), "data_generator_random_spikes_create->data_gen");
	check_memory(data_gen);
	data_gen->length = n_examples;
	data_gen->data = data;
	data_gen->is_valid = data_generator_with_step_between_neurons_is_valid;
	data_gen->destroy = data_generator_with_step_between_neurons_destroy;
	data_gen->get_elem = data_generator_with_step_between_neurons_get_elem;

	return data_gen;

ERROR
	if (data != NULL) free(data);
	if (data_gen != NULL) free(data_gen);

	return NULL;
}


BOOL data_generator_with_step_between_neurons_is_valid(DataGenerator* data_gen) {
	check(data_gen != NULL, null_argument("data_gen"));
	check(data_gen->length > 0, "@data_gen->lenght == 0");
	check(data_gen->data != NULL, null_argument("data_gen->data"));
	check(data_gen->is_valid != NULL, null_argument("data_gen->is_valid"));
	check(data_gen->destroy != NULL, null_argument("data_gen->destroy"));
	check(data_gen->get_elem != NULL, null_argument("data_gen->get_elem"));

	DataGeneratorData* data = data_gen->data;
	check(data->duration > 0, "@data->duration == 0");
	check(network_is_valid(data->net) == TRUE, invalid_argument("data->net"));
	check(data->step_between_neurons > 0, "@data->step_between_neurons == 0");

	return TRUE;
ERROR
	return FALSE;
}

void data_generator_with_step_between_neurons_destroy(DataGenerator* data_gen) {
	check(data_generator_is_valid(data_gen) == TRUE, invalid_argument("data_gen"));
	DataGeneratorData* data = (DataGeneratorData*)data_gen->data;
	data->duration = 0;
	data->net = NULL;
	data->step_between_neurons = 0;
	free(data);

	data_gen->data = NULL;
	data_gen->destroy = NULL;
	data_gen->get_elem = NULL;
	data_gen->is_valid = NULL;
	data_gen->length = 0;
	free(data_gen);

ERROR
	return;
}


DataElement* data_generator_with_step_between_neurons_get_elem(DataGenerator* data_gen, uint32_t idx) {
	check(data_generator_is_valid(data_gen) == TRUE, invalid_argument("data_gen"));
	check(data_gen->length > idx, "data_gen->length <= idx");  // should not go over the dataset
	DataGeneratorData* data = (DataGeneratorData*)data_gen->data;
	return data_element_with_step_between_neurons_create(data->net, data->step_between_neurons, data->duration);
ERROR
	return NULL;
}
