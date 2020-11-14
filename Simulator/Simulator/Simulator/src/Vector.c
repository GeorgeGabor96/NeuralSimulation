#include "Containers.h"


#include <stdlib.h>


Vector* vector_create(uint32_t length, size_t element_size) {
	Vector* vector = (Vector*)malloc(sizeof(Vector));
	check_memory(vector);

	vector->array.data = malloc(length * element_size);
	check_memory(vector->array.data);

	vector->length = 0;
	vector->array.length = length;
	vector->array.element_size = element_size;

	return vector;

error:
	if (vector != NULL) {
		free(vector);
	}
	return NULL;
}


void vector_destroy(Vector* vector) {
	check(vector != NULL, "NULL value for @vector");
	check(vector->array.data != NULL, "NULL value for @vector->array.data");
	free(vector->array.data);
	free(vector);

error:
	return;
}


Status vector_set(Vector* vector, uint32_t index, void* data) {
	return array_set(&(vector->array), index, data);

}

Status vector_append(Vector* vector, void* data) {
	Status status = FAIL;
	check(vector != NULL, "NULL value for @vector");
	check(vector->array.data != NULL, "NULL value for @vector->array.data");
	check(data != NULL, "NULL value for @data");

	if (vector_is_full(vector)) {
		check(array_expand(&(vector->array)) == SUCCESS, "Vector is full and could not allocate more memory");
	}

	array_set_fast(&(vector->array), (vector->length)++, data);

	status = SUCCESS;

error:
	return status;
}


void* vector_get(Vector* vector, uint32_t index) {
	return array_get(&(vector->array), index);
}
