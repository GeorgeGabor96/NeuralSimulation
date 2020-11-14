#include <stdlib.h>

#include "Containers.h"


Array* array_create(uint32_t length, size_t element_size) {
	Array* array = (Array*)malloc(sizeof(Array));
	check_memory(array);

	array->element_size = element_size;
	array->length = length;
	array->data = malloc(length * element_size);
	check_memory(array->data);

	return array;

error:
	if (array != NULL) {
		free(array);
	}
	return NULL;
}


void array_destroy(Array* array) {
	check(array != NULL, "NULL value for @array");
	check(array->data != NULL, "NULL value for @array->data");
	free(array->data);
	free(array);

error:
	return;
}


void* array_get(Array* array, uint32_t index) {
	void* out = NULL;
	check(array != NULL, "NULL value for @array");
	check(array->data != NULL, "NULL value for @array->data");
	check(index < array->length, "Out of bound value for @index: %u; @array->max_length: %u", index, array->length);
	
	out = array_get_fast(array, index);
	
error:
	return out;
}


Status array_set(Array* array, uint32_t index, void* data) {
	Status status = FAIL;
	check(array != NULL, "NULL value for @array");
	check(array->data != NULL, "NULL value for @array->data");
	check(data != NULL, "NULL value for @src_data");
	check(index < array->length, "Out of bound value for @index: %u; @array->max_lenght: %u", index, array->length);
	
	array_set_fast(array, index, data);
	status = SUCCESS;

error:
	return status;
}


// POSSIBLE OPTIMIZATION, add parameter for expand rate, not every container needs the same amount
Status array_expand(Array* array) {
	Status status = FAIL;
	check(array != NULL, "NULL value for @array");
	check(array->data != NULL, "NULL value for @array->data");

	uint32_t new_length = array->length + ARRAY_EXPAND_RATE;
	void* new_data = realloc(array->data, new_length * array->element_size);
	check_memory(new_data);

	array->length = new_length;
	array->data = new_data;
	status = SUCCESS;

error:
	return status;
}