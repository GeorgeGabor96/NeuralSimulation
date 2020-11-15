#include "vector_tests.h"
#include "Containers.h"

#include <stdint.h>


TestStatus vector_create_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t length = 10;
	size_t element_size = sizeof(uint32_t);
	
	// call
	Vector* vector = vector_create(length, element_size);
	check_memory(vector);

	assert(vector->length == 0, "@vector->length should be 0");
	assert(vector->array.length == length, "@vector->array.length should be %u not %u", length, vector->array.length);
	assert(vector->array.element_size == element_size, "@vector->array.element_length should be %llu not %llu", element_size, vector->array.element_size);
	check_memory(vector->array.data);

	status = TEST_SUCCESS;

	// cleanup
error:
	// @vector->array.data failed
	if (vector != NULL) {
		free(vector);
	}
	return status;
}


TestStatus vector_destroy_test() {
	// setup
	Vector* vector = vector_create(10, sizeof(uint32_t));
	void* data = vector->array.data;

	// call with @vector = NULL
	vector_destroy(NULL);

	// call with @vector->array.data = NULL
	vector->array.data = NULL;
	vector_destroy(vector);
	vector->array.data = data;

	// call normal
	vector_destroy(vector);

	return TEST_SUCCESS;
}


TestStatus vector_append_test() {
	// setup
	TestStatus status = SUCCESS;
	uint32_t length = 2u;
	uint32_t value1 = 5u;
	uint32_t value2 = 10u;
	uint32_t value3 = 15u;
	uint32_t value = 0u;
	size_t element_size = sizeof(uint32_t);
	Vector* vector = vector_create(length, element_size);
	void* data = vector->array.data;

	// call with @vector = NULL
	assert(vector_append(NULL, &value1) == FAIL, "Should fail for @vector = NULL");
	
	// call with @vector->array.data = NULL
	vector->array.data = NULL;
	assert(vector_append(vector, &value1) == FAIL, "Should fail for @vector->array.data = NULL");
	vector->array.data = data;

	// call with @data = NULL
	assert(vector_append(vector, NULL) == FAIL, "Should fail for @data = NULL");

	// add 3 elements, should extend the vector
	assert(vector_append(vector, &value1) == SUCCESS, "Should be able to append value %u", value1);
	assert(vector_append(vector, &value2) == SUCCESS, "Should be able to append value %u", value2);
	assert(vector_append(vector, &value3) == SUCCESS, "Should be able to append value %u", value3);
	assert(vector->length == 3, "Should have %u elements not %u", 3, vector->length);

	// check that array was extended
	assert(vector->array.length == length + ARRAY_EXPAND_RATE, "@vector->array.lenght should be %u not %u", length + ARRAY_EXPAND_RATE, vector->array.length);

	// check the contained elements
	value = *(uint32_t*)vector_get(vector, 0);
	assert(value == value1, "First element should be %u not %u", value1, value);
	value = *(uint32_t*)vector_get(vector, 1);
	assert(value == value2, "Second element should be %u not %u", value2, value);
	value = *(uint32_t*)vector_get(vector, 2);
	assert(value == value3, "Third element should be %u not %u", value3, value);

	status = TEST_SUCCESS;

	// cleanup
error:
	if (vector != NULL) {
		// for if the @vector->array.data = NULL assert fails
		if (vector->array.data == NULL) {
			vector->array.data = data;
		}
		vector_destroy(vector);
	}

	return status;
}