#include "data/DataGen.h"
#include "utils/MemoryManagement.h"


/*************************************************************
* DATA GENERATOR FUNCTIONALITY
*************************************************************/
BOOL data_generator_is_valid(DataGenerator* generator) {
	check(generator != NULL, null_argument("generator"));
	check(generator->length > 0, "@generator->lenght == 0");
	check(generator->data != NULL, null_argument("generator->data"));
	check(generator->data_is_valid != NULL, null_argument("generator->data_is_valid"));
	check(generator->data_destroy != NULL, null_argument("generator->data_destroy"));
	check(generator->get_elem != NULL, null_argument("generator->get_elem"));

	__DataGeneratorData* data = (__DataGeneratorData*)generator->data;
	check(generator->data_is_valid(data) == TRUE, invalid_argument("generator->data"));

	return TRUE;
ERROR
	return FALSE;
}

void data_generator_destroy(DataGenerator* generator) {
	check(data_generator_is_valid(generator) == TRUE, invalid_argument("generator"));
	__DataGeneratorData* data = (__DataGeneratorData*)generator->data;
	generator->data_destroy(data);

	generator->data = NULL;
	generator->data_is_valid = NULL;
	generator->data_destroy = NULL;
	generator->get_elem = NULL;
	generator->length = 0;
	free(generator);

ERROR
	return;
}

DataElement* data_generator_get_element(DataGenerator* generator, uint32_t idx) {
	check(data_generator_is_valid(generator) == TRUE, invalid_argument("generator"));
	check(generator->length > idx, "data_gen->length <= idx");  // should not go over the dataset
	__DataGeneratorData* data = (__DataGeneratorData*)generator->data;
	return generator->get_elem(data, idx);
ERROR
	return NULL;
}


/*************************************************************
* DATA ELEMENT FUNCTIONALITY
*************************************************************/
BOOL data_element_is_valid(DataElement* element) {
	check(element != NULL, null_argument("element"));
	check(element->duration > 0, "@element->duration == 0");
	check(element->data != NULL, null_argument("element->data"));
	check(element->data_is_valid != NULL, null_argument("element->data_is_valid"));
	check(element->data_destroy != NULL, null_argument("element->data_destroy"));
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


NetworkInputs* data_element_get_values(DataElement* element, uint32_t time) {
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));
	return element->get_values(element, time);

ERROR
	return NULL;
}


void data_element_remove_values(DataElement* element, NetworkInputs* values) {
	check(data_element_is_valid(element) == TRUE, invalid_argument("element"));
	element->remove_values(element, values);

ERROR
	return;
}


/*************************************************************
* DATA ELEMENT HELPER FUNCTIONALITY
*************************************************************/
void data_element_base_remove_values(DataElement* element, NetworkInputs* inputs) {
	(element);
	network_values_destroy(inputs);
}