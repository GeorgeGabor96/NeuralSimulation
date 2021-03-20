#include "data/DataGen.h"

BOOL data_element_is_valid(DataElement* element) {
	check(element != NULL, null_argument("element"));
	check(element->duration > 0, "@element->duration == 0");
	check(element->data != NULL, null_argument("element->data"));
	check(element->data_is_valid != NULL, null_argument("element->is_valid"));
	check(element->data_destroy != NULL, null_argument("element->destroy"));
	check(element->get_values != NULL, null_argument("element->get_values"));

	__DataElementData* data = (__DataElementData*)element->data;
	check(element->data_is_valid(data) == TRUE, invalid_argument("data"));

	return TRUE;
ERROR
	return FALSE;
}

void data_element_destroy(DataElement* element) {
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

	__DataElementData* data = (__DataElementData*)element->data;
	element->data_destroy(data);

	element->duration = 0;
	element->data = NULL;
	element->data_is_valid = NULL;
	element->data_destroy = NULL;
	element->get_values = NULL;
	element->remove_values = NULL;
	free(element);

ERROR
	return;
}


void data_element_base_remove_values(DataElement* element, NetworkInputs* inputs) {
	(element);
	network_values_destroy(inputs);
}