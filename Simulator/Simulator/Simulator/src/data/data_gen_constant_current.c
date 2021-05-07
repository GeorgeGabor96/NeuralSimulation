#include "utils/MemoryManagement.h"
#include "DataGen.h"


/*************************************************************
* DATA ELEMENT FUNCTIONALITY
*************************************************************/
typedef struct DataElementData {
	float value;
	Network* net;
}DataElementData;

BOOL data_element_constant_current_data_is_valid(DataElementData* data);
void data_element_constant_current_data_destroy(DataElementData* data);
NetworkInputs* data_element_constant_current_get_values(DataElement* element, uint32_t time);


// need the network to know how many inputs to generate and for each input how many currents to make
DataElement* data_element_constant_current_create(Network* net, float current_value, uint32_t duration) {
	DataElement* element = NULL;
	DataElementData* data = NULL;
	check(network_is_valid(net) == TRUE, invalid_argument("network"));
	check(duration > 0, "@duration == 0");

	data = (DataElementData*)malloc(sizeof(DataElementData), "data_element_constant_current_create->data");
	check_memory(data);
	data->value = current_value;
	data->net = net;

	element = (DataElement*)malloc(sizeof(DataElement), "data_element_constant_current_create->element");
	check_memory(element);
	element->duration = duration;
	element->data = data;
	element->data_is_valid = data_element_constant_current_data_is_valid;
	element->data_destroy = data_element_constant_current_data_destroy;
	element->get_values = data_element_constant_current_get_values;
	element->remove_values = data_element_base_remove_values;

	return element;

ERROR
	if (element != NULL) free(element);
	if (data != NULL) free(data);
	return NULL;
}


BOOL data_element_constant_current_data_is_valid(DataElementData* data) {
	check(data != NULL, null_argument("data"));
	check(network_is_valid(data->net) == TRUE, invalid_argument("data->net"));

	return TRUE;
ERROR
	return FALSE;
}

void data_element_constant_current_data_destroy(DataElementData* data) {
	data->net = NULL;
	data->value = 0.0f;
	free(data);
}


NetworkInputs* data_element_constant_current_get_values(DataElement* element, uint32_t time) {
	Status status = FAIL;
	NetworkInputs* inputs = NULL;
	DataElementData* data = NULL;
	Network* net = NULL;
	NetworkValues net_input = { 0 };
	uint32_t i = 0;
	uint32_t j = 0;
	float current_value = 0.0f;

	(time);
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

	data = (DataElementData*)element->data;
	current_value = data->value;
	net = data->net;

	inputs = array_create(net->input_layers.length, 0, sizeof(NetworkValues));
	check(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
	// for each network input
	for (i = 0; i < net->input_layers.length; ++i) {
		Layer* layer = *((Layer**)array_get(&(net->input_layers), i));
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

		status = array_init(&(net_input.values), layer->neurons.length, layer->neurons.length, sizeof(float));
		check(status == SUCCESS, invalid_argument("status"));
		for (j = 0; j < layer->neurons.length; ++j)
			array_set(&(net_input.values), j, &current_value);
		net_input.type = CURRENT;

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
	float value;
	uint32_t duration;
	Network* net;
} DataGeneratorData;

BOOL data_generator_constant_current_data_is_valid(DataGeneratorData* data);
void data_generator_constant_current_data_destroy(DataGeneratorData* data);
DataElement* data_generator_constant_current_get_elem(DataGeneratorData* data, uint32_t idx);


DataGenerator* data_generator_constant_current_create(uint32_t length, Network* net, float current_value, uint32_t duration) {
	DataGeneratorData* data = NULL;
	DataGenerator* data_gen = NULL;
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	check(duration > 0, "@duration == 0");
	check(length > 0, "@length == 0");

	data = (DataGeneratorData*)malloc(sizeof(DataGeneratorData), "data_generator_constant_current_create->data");
	check_memory(data);
	data->duration = duration;
	data->net = net;
	data->value = current_value;

	data_gen = (DataGenerator*)malloc(sizeof(DataGenerator), "data_generator_constant_current_create->data_gen");
	check_memory(data_gen);
	data_gen->length = length;
	data_gen->data = data;
	data_gen->data_is_valid = data_generator_constant_current_data_is_valid;
	data_gen->data_destroy = data_generator_constant_current_data_destroy;
	data_gen->get_elem = data_generator_constant_current_get_elem;

	return data_gen;

ERROR
	if (data != NULL) free(data);
	if (data_gen != NULL) free(data_gen);

	return NULL;
}


BOOL data_generator_constant_current_data_is_valid(DataGeneratorData* data) {
	check(data->duration > 0, "@data->duration == 0");
	check(network_is_valid(data->net) == TRUE, invalid_argument("data->net"));

	return TRUE;
ERROR
	return FALSE;
}

void data_generator_constant_current_data_destroy(DataGeneratorData* data) {
	data->duration = 0;
	data->net = NULL;
	data->value = 0.0f;
	free(data);
}


DataElement* data_generator_constant_current_get_elem(DataGeneratorData* data, uint32_t idx) {
	return data_element_constant_current_create(data->net, data->value, data->duration);
}
