#include "data/DataGen.h"


BOOL data_element_constant_current_is_valid(DataElement* element);
void data_element_constant_current_destroy(DataElement* element);
NetworkInputs* data_element_constant_current_get_values(DataElement* element, uint32_t time);
void data_element_constant_current_remove_values(DataElement* element, NetworkInputs* inputs);


typedef struct DataElementData {
	float value;
	Network* net;
}DataElementData;


// need the network be know how many inputs to generate and for each input how many currents to make
DataElement* data_element_constant_current_create(Network* net, float current_value, uint32_t duration) {
	DataElement* element = NULL;
	DataElementData* data = NULL;	
	check(network_is_valid(net) == TRUE, invalid_argument("network"));
	check(duration > 0, "@duration == 0");

	data = (DataElementData*)malloc(sizeof(DataElementData));
	check_memory(data);
	data->value = current_value;
	data->net = net;

	element = (DataElement*)malloc(sizeof(DataElement));
	check_memory(element);
	element->duration = duration;
	element->data = data;
	element->is_valid = data_element_constant_current_is_valid;
	element->destroy = data_element_constant_current_destroy;
	element->get_values = data_element_constant_current_get_values;
	element->remove_values = data_element_constant_current_remove_values;

	return element;

ERROR
	if (element != NULL) free(element);
	if (data != NULL) free(data);
	return NULL;
}


BOOL data_element_constant_current_is_valid(DataElement* element) {
	check(element != NULL, null_argument("element"));
	check(element->duration > 0, "@element->duration == 0");
	check(element->data != NULL, null_argument("element->data"));
	check(element->is_valid != NULL, null_argument("element->is_valid"));
	check(element->destroy != NULL, null_argument("element->destroy"));
	check(element->get_values != NULL, null_argument("element->get_values"));

	DataElementData* data = (DataElementData*)element->data;
	check(network_is_valid(data->net) == TRUE, invalid_argument("data->net"));

	return TRUE;
ERROR
	return FALSE;
}


void data_element_constant_current_destroy(DataElement* element) {
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

	DataElementData* data = (DataElementData*)element->data;
	data->net = NULL;
	data->value = 0.0f;
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


NetworkInputs* data_element_constant_current_get_values(DataElement* element, uint32_t time) {
	Status status = FAIL;
	NetworkInputs* inputs = NULL;
	DataElementData* data = NULL;
	Network* net = NULL;
	NetworkValues net_input = { 0 };
	uint32_t i = 0;
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
		net_input.type = CURRENT;

		status = array_append(inputs, &net_input);
		check(status == SUCCESS, invalid_argument("status"));
	}

	return inputs;

ERROR
	if (inputs != NULL)
		data_element_constant_current_remove_values(element, inputs);
	return NULL;
}


void data_element_constant_current_remove_values(DataElement* element, NetworkInputs* inputs) {
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