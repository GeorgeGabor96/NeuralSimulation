#include <stdlib.h>

#include "Containers.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status array_is_valid(Array* array) {
	check(array != NULL, null_argument("array"));
	check(array->length > 0, "@array->length == 0");
	check(array->element_size > 0, "@array->element_size == 0");
	check(array->data != NULL, null_argument("array->data"));

	return TRUE;
error:
	return FALSE;
}


/*************************************************************
* Array Functionality
*************************************************************/
Array* array_create(uint32_t length, size_t element_size) {
	Array* array = (Array*)calloc(1, sizeof(Array));
	check_memory(array);

	array->element_size = element_size;
	array->length = length;
	array->data = malloc(length * element_size);
	check_memory(array->data);

	return array;

error:
	if (array != NULL) {
		if (array->data != NULL) free(array->data);
		free(array);
	}
	return NULL;
}


void array_reset(Array* array, ElemReset destroy) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	if (destroy != NULL) {
		void* elem = NULL;
		for (uint32_t i = 0; i < array->length; ++i) {
			elem = array_get(array, i);
			destroy(elem);
		}
	}

error:
	return;
}


void array_destroy(Array* array, ElemReset destroy) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	array_reset(array, destroy);
	free(array->data);
	free(array);

error:
	return;
}


void* array_get(Array* array, uint32_t index) {
	void* out = NULL;
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(index < array->length, "Out of bound value for @index: %u; @array->max_length: %u", index, array->length);
	
	out = array_get_fast(array, index);
	
error:
	return out;
}


Status array_set(Array* array, uint32_t index, void* data) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(data != NULL, null_argument("src_data"));
	check(index < array->length, "Out of bound value for @index: %u; @array->max_lenght: %u", index, array->length);
	
	array_set_fast(array, index, data);
	return SUCCESS;

error:
	return FAIL;
}


Status array_expand(Array* array) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));

	uint32_t new_length = array->length + ARRAY_EXPAND_RATE;
	void* new_data = realloc(array->data, new_length * array->element_size);
	check_memory(new_data);

	array->length = new_length;
	array->data = new_data;
	return SUCCESS;

error:
	return FAIL;
}


void array_show(Array* array, ShowElem show) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	uint32_t i = 0;

	for (i = 0; i < array->length; ++i) {
		show(array_get(array, i));
	}
	printf("\n");

error:
	return;
}
