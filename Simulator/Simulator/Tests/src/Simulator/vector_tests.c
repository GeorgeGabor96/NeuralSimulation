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
	assert(vector != NULL, null_argument("vector"));

	assert(vector->length == 0, "@vector->length should be 0");
	assert(vector->array.length == length, "@vector->array.length should be %u not %u", length, vector->array.length);
	assert(vector->array.element_size == element_size, "@vector->array.element_length should be %llu not %llu", element_size, vector->array.element_size);
	assert(vector->array.data != NULL, null_argument("vector->array.data"));

	status = TEST_SUCCESS;

	// cleanup
error:
	// @vector->array.data failed
	if (vector != NULL) {
		vector_destroy(vector, NULL);
	}
	return status;
}


TestStatus vector_destroy_test() {
	// setup
	Vector* vector = vector_create(10, sizeof(uint32_t));
	void* data = vector->array.data;

	// call with @vector = NULL
	vector_destroy(NULL, NULL);

	// call with @vector->array.data = NULL
	vector->array.data = NULL;
	vector_destroy(vector, NULL);
	vector->array.data = data;

	// call normal
	vector_destroy(vector, NULL);

	return TEST_SUCCESS;
}


TestStatus vector_append_get_set_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t a[4] = { 2u, 5u, 10u, 15u };
	uint32_t value = 0;
	uint32_t length = 2u;
	uint32_t i = 0;
	size_t element_size = sizeof(uint32_t);
	Vector* vector = vector_create(length, element_size);
	void* data = vector->array.data;

	// call with @vector = NULL
	assert(vector_append(NULL, &(a[0])) == FAIL, "Should fail for @vector = NULL");
	
	// call with @vector->array.data = NULL
	vector->array.data = NULL;
	assert(vector_append(vector, &(a[0])) == FAIL, "Should fail for @vector->array.data = NULL");
	vector->array.data = data;

	// call with @data = NULL
	assert(vector_append(vector, NULL) == FAIL, "Should fail for @data = NULL");

	// add elements, should extend the vector
	for (i = 0; i < 4; ++i) {
		assert(vector_append(vector, &(a[i])) == SUCCESS, "Value %u at index %u could not be inserted", a[i], i);
	}
	assert(vector->length == 4, "Should have %u elements not %u", 3, vector->length);

	// check that array was extended
	assert(vector->array.length == length + ARRAY_EXPAND_RATE, "@vector->array.lenght should be %u not %u", length + ARRAY_EXPAND_RATE, vector->array.length);

	for (i = 0; i < 4; ++i) {
		value = *(uint32_t*)vector_get(vector, i);
		assert(value == a[i], "Value at index %u should be %u not %u", i, a[i], value);
	}

	assert(vector_set(vector, 0, &a[3]) == SUCCESS, "Should be able to set value");
	value = *(uint32_t*)vector_get(vector, 0);
	assert(value == a[3], "Value at index 0 shoule now be %u not %u", a[3], value);

	status = TEST_SUCCESS;

	// cleanup
error:
	if (vector != NULL) {
		// for if the @vector->array.data = NULL assert fails
		if (vector->array.data == NULL) {
			vector->array.data = data;
		}
		vector_destroy(vector, NULL);
	}

	return status;
}


TestStatus vector_show_test() {
	TestStatus status = FAIL;
	Vector* vector = vector_create(10, sizeof(uint32_t));
	uint32_t i = 0;

	for (i = 0; i < 7; i++) {
		vector_append(vector, &i);
	}

	vector_show(vector, show_uint32_t);

	// corner cases
	vector_show(NULL, NULL);

	status = TEST_SUCCESS;

	vector_destroy(vector, NULL);

	return status;
}