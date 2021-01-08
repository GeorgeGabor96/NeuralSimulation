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
	check(show != NULL, null_argument("show"));

	uint32_t i = 0;
	for (i = 0; i < array->length; ++i) {
		show(array_get(array, i));
	}
	printf("\n");

error:
	return;
}


void array_copy_data(Array* array, void* data, uint32_t start_idx, uint32_t elem_cnt) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(data != NULL, null_argument("data"));
	check(start_idx < array->length, "@start_idx >= array->length");
	check(elem_cnt > 0, "@elem_cnt == 0");
	if (start_idx + elem_cnt > array->length) {
		log_info("@elem_cnt too big, will trucate it");
		elem_cnt = array->length - start_idx;
	}

	memcpy(array_get(array, start_idx), data, elem_cnt * array->element_size);

error:
	return;
}



Status array_swap(Array* array, uint32_t i, uint32_t j) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(i < array->length, "Out of bound value for @i: %u; @array->length: %u", i, array->length);
	check(j < array->length, "Out of bound value for @j: %u; @array->length: %u", j, array->length);

	void* data1 = array_get_fast(array, i);
	void* data2 = array_get_fast(array, j);
	void* aux = malloc(array->element_size);
	check_memory(aux);

	memcpy(aux, data1, array->element_size);
	memcpy(data1, data2, array->element_size);
	memcpy(data2, aux, array->element_size);
	
	free(aux);

	return SUCCESS;
error:
	return FAIL;
}
