#include <stdlib.h>

#include "GenericArray.h"
#include "debug.h"


GenericArray* generic_array_create(uint32_t element_max_count, uint32_t element_size_in_bytes) {
	GenericArray* array = (GenericArray*)malloc(sizeof(GenericArray));
	check_memory(array);

	array->element_size_in_bytes = element_size_in_bytes;
	array->max_length = element_max_count;
	array->length = 0;
	array->data = malloc(sizeof(element_max_count * element_size_in_bytes));
	check_memory(array->data);

	return array;

error:
	// array->data allocation failed
	if (array != NULL) {
		free(array);
	}
	// array allocation failed
	return NULL;
}


void generic_array_destroy(GenericArray* array) {
	free(array->data);
	free(array);
}

