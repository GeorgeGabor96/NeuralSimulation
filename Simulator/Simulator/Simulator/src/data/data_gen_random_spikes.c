#include "MemoryManagement.h"
#include "data/data_gen_random_spikes.h"


/*************************************************************
* DATA ELEMENT FUNCTIONALITY
*************************************************************/
BOOL data_element_random_spikes_is_valid(DataElement* element);
void data_element_random_spikes_destroy(DataElement* element);
NetworkInputs* data_element_random_spikes_get_values(DataElement* element, uint32_t time);
void data_element_random_spikes_remove_values(DataElement* element, NetworkInputs* inputs);


typedef struct DataElementData {
	float spikes_percent;
	Network* net;
}DataElementData;


// need the network be know how many inputs to generate and for each input how many currents to make
DataElement* data_element_random_spikes_create(Network* net, float spikes_percent, uint32_t duration) {
	DataElement* element = NULL;
	DataElementData* data = NULL;
	check(network_is_valid(net) == TRUE, invalid_argument("network"));
	check(duration > 0, "@duration == 0");
	check(spikes_percent >= 0.0f, "@spikes_percent < 0.0f");
	check(spikes_percent <= 1.0f, "@spikes_percent > 1.0f");

	data = (DataElementData*)malloc(sizeof(DataElementData), "data_element_random_spikes_create->data");
	check_memory(data);
	data->spikes_percent = spikes_percent;
	data->net = net;

	element = (DataElement*)malloc(sizeof(DataElement), "data_element_random_spikes_create->element");
	check_memory(element);
	element->duration = duration;
	element->data = data;
	element->is_valid = data_element_random_spikes_is_valid;
	element->destroy = data_element_random_spikes_destroy;
	element->get_values = data_element_random_spikes_get_values;
	element->remove_values = data_element_random_spikes_remove_values;

	return element;

ERROR
	if (element != NULL) free(element);
	if (data != NULL) free(data);
	return NULL;
}


BOOL data_element_random_spikes_is_valid(DataElement* element) {
	check(element != NULL, null_argument("element"));
	check(element->duration > 0, "@element->duration == 0");
	check(element->data != NULL, null_argument("element->data"));
	check(element->is_valid != NULL, null_argument("element->is_valid"));
	check(element->destroy != NULL, null_argument("element->destroy"));
	check(element->get_values != NULL, null_argument("element->get_values"));

	DataElementData* data = (DataElementData*)element->data;
	check(network_is_valid(data->net) == TRUE, invalid_argument("data->net"));
	check(data->spikes_percent >= 0.0f, "@data->spikes_percent < 0.0f");
	check(data->spikes_percent <= 1.0f, "@data->spikes_percent > 1.0f");

	return TRUE;
ERROR
	return FALSE;
}


void data_element_random_spikes_destroy(DataElement* element) {
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

	DataElementData* data = (DataElementData*)element->data;
	data->net = NULL;
	data->spikes_percent = 0.0f;
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


NetworkInputs* data_element_random_spikes_get_values(DataElement* element, uint32_t time) {
	Status status = FAIL;
	NetworkInputs* inputs = NULL;
	DataElementData* data = NULL;
	Network* net = NULL;
	Layer* layer = NULL;
	NetworkValues net_input = { 0 };
	uint32_t i = 0;
	uint32_t j = 0;
	BOOL spike = FALSE;
	float spikes_percent = 0.0f;

	(time);
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

	data = (DataElementData*)element->data;
	spikes_percent = data->spikes_percent;
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
		data_element_random_spikes_remove_values(element, inputs);
	return NULL;
}


void data_element_random_spikes_remove_values(DataElement* element, NetworkInputs* inputs) {
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
BOOL data_generator_random_spikes_is_valid(DataGenerator* data);
void data_generator_random_spikes_destroy(DataGenerator* data);
DataElement* data_generator_random_spikes_get_elem(DataGenerator* data, uint32_t idx);

typedef struct DataGeneratorData {
	float spikes_percent;
	uint32_t duration;
	Network* net;
} DataGeneratorData;


DataGenerator* data_generator_random_spikes_create(uint32_t n_examples, Network* net, float spikes_percent, uint32_t duration) {
	DataGeneratorData* data = NULL;
	DataGenerator* data_gen = NULL;
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	check(duration > 0, "@duration == 0");
	check(n_examples > 0, "@n_examples == 0");
	check(spikes_percent >= 0.0f, "@spikes_percent < 0.0f");
	check(spikes_percent <= 1.0f, "@spikes_percent > 1.0f");

	data = (DataGeneratorData*)malloc(sizeof(DataGeneratorData), "data_generator_random_spikes_create->data");
	check_memory(data);
	data->spikes_percent = spikes_percent;
	data->duration = duration;
	data->net = net;

	data_gen = (DataGenerator*)malloc(sizeof(DataGenerator), "data_generator_random_spikes_create->data_gen");
	check_memory(data_gen);
	data_gen->length = n_examples;
	data_gen->data = data;
	data_gen->is_valid = data_generator_random_spikes_is_valid;
	data_gen->destroy = data_generator_random_spikes_destroy;
	data_gen->get_elem = data_generator_random_spikes_get_elem;
	
	// random seed for spike generation
	time_t t;
	srand((unsigned)time(&t));
	return data_gen;

ERROR
	if (data != NULL) free(data);
	if (data_gen != NULL) free(data_gen);

	return NULL;
}


BOOL data_generator_random_spikes_is_valid(DataGenerator* data_gen) {
	check(data_gen != NULL, null_argument("data_gen"));
	check(data_gen->length > 0, "@data_gen->lenght == 0");
	check(data_gen->data != NULL, null_argument("data_gen->data"));
	check(data_gen->is_valid != NULL, null_argument("data_gen->is_valid"));
	check(data_gen->destroy != NULL, null_argument("data_gen->destroy"));
	check(data_gen->get_elem != NULL, null_argument("data_gen->get_elem"));

	DataGeneratorData* data = data_gen->data;
	check(data->duration > 0, "@data->duration == 0");
	check(network_is_valid(data->net) == TRUE, invalid_argument("data->net"));
	check(data->spikes_percent >= 0.0f, "@data->spikes_percent < 0.0f");
	check(data->spikes_percent <= 1.0f, "@data->spikes_percent > 1.0f");

	return TRUE;
ERROR
	return FALSE;
}

void data_generator_random_spikes_destroy(DataGenerator* data_gen) {
	check(data_generator_is_valid(data_gen) == TRUE, invalid_argument("data_gen"));
	DataGeneratorData* data = (DataGeneratorData*)data_gen->data;
	data->duration = 0;
	data->net = NULL;
	data->spikes_percent = 0.0f;
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


DataElement* data_generator_random_spikes_get_elem(DataGenerator* data_gen, uint32_t idx) {
	check(data_generator_is_valid(data_gen) == TRUE, invalid_argument("data_gen"));
	check(data_gen->length > idx, "data_gen->length <= idx");  // should not go over the dataset
	DataGeneratorData* data = (DataGeneratorData*)data_gen->data;
	return data_element_random_spikes_create(data->net, data->spikes_percent, data->duration);
ERROR
	return NULL;
}