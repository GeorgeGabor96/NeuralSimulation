#include "data/DataGen.h"


BOOL data_element_constant_current_is_valid(DataElement* element);
void data_element_constant_current_destroy(DataElement* element);
NetworkInputs* data_element_constant_current_get_values(DataElement* element, uint32_t time);


typedef struct DataElementData {
	float value;
	float length;
}DataElementData;


DataElement* data_element_constant_current_create(float current_value, uint32_t n_currents, uint32_t duration) {
	check(n_currents > 0, "@number_of_values == 0");
	check(duration > 0, "@duration == 0");
	DataElement* element = NULL;
	DataElementData* data = NULL;

	data = (DataElementData*)malloc(sizeof(DataElementData));
	check_memory(data);
	data->value = current_value;
	data->length = n_currents;

	element = (DataElement*)malloc(sizeof(DataElement));
	check_memory(element);
	element->duration = duration;
	element->data = data;
	element->is_valid = data_element_constant_current_is_valid;
	element->destroy = data_element_constant_current_destroy;
	element->get_values = data_element_constant_current_get_values;

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
	check(data->length > 0, "@data->length == 0");

	return TRUE;
ERROR
	return FALSE;
}


void data_element_constant_current_destroy(DataElement* element) {
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

	DataElementData* data = (DataElementData*)element->data;
	data->length = 0;
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

// Consider that it creates input for a single input layer -> limitation but its simple now
NetworkInputs* data_element_constant_current_get_values(DataElement* element, uint32_t time) {
	NetworkValues* currents = NULL;
	NetworkInputs* inputs = NULL;
	DataElementData* data = NULL;
	ArrayFloat* current_values = NULL;
	uint32_t i = 0;
	float current_value = 0.0f;

	(time);
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));
	
	data = (DataElementData*)element->data;
	current_value = data->value;

	current_values = array_create(data->length, data->length, sizeof(float));
	check(array_is_valid(current_values) == TRUE, invalid_argument("array"));
	for (i = 0; i < data->length; ++i)
		array_set(current_values, i, &current_value);

	currents = (NetworkValues*)malloc(sizeof(NetworkValues));
	check_memory(currents); 
	currents->values = current_values;
	currents->type = CURRENT;

	// currents should be on stack
	// wand network inputs to be array of currents not array of pointers to currents

	inputs = array_create(1, 1, sizeof(NetworkValues));
	check(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
	array_set(inputs, 0, &currents);


	return inputs;

ERROR
	return NULL;
}