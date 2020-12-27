#include "array_tests.h"
#include "Containers.h"


TestStatus array_create_test() {
	// setup
	uint32_t length = 10;
	size_t element_size = sizeof(uint32_t);
	TestStatus status = TEST_FAILED;

	// call
	Array* array = array_create(length, element_size);
	
	// checks
	assert(array != NULL, "@array should not be NULL");
	assert(array->data != NULL, "@array->data should not be NULL");
	assert(array->length == length, "@array->length should be %u", length);
	assert(array->element_size == element_size, "@array->element_size should be %llu", element_size);

	status = TEST_SUCCESS;

	// cleanup
error:
	if (array != NULL) {
		array_destroy(array, NULL);
	}
	return status;
}


TestStatus array_destroy_test() {
	// setup 
	Array* array = array_create(10, sizeof(int));
	void* data = NULL;

	// call with array = NULL
	array_destroy(NULL, NULL);

	// call with array->data = NULL
	data = array->data;
	array->data = NULL;
	array_destroy(array, NULL);
	array->data = data;

	// normal call
	array_destroy(array, NULL);

	return TEST_SUCCESS;
}


TestStatus array_set_get_test() {
	TestStatus status = TEST_FAILED;
	Array* array = array_create(5, sizeof(float));
	float a[4] = { 1.2f, 3.103f, -0.124f, 124111.123f };
	uint32_t i = 0;
	float* value = NULL;

	for (i = 0; i < 4; ++i) {
		array_set(array, i, &(a[i]));
	}

	// get and check them
	for (i = 0; i < 4; ++i) {
		value = (float*)array_get(array, i);
		assert(*value == a[i], "Value at index %u should be %f not %f", i, a[i], *value);
	}

	// corner cases
	assert(array_set(NULL, 0, &(a[0])) == FAIL, "Should fail for invalid array");
	assert(array_set(array, 10, &(a[0])) == FAIL, "Should fail for invalid index");
	assert(array_set(array, 0, NULL) == FAIL, "Should fail for invalid data");

	assert(array_get(NULL, 0) == NULL, "Should fail for invalid array");
	assert(array_get(array, 10) == NULL, "Should fail for invalid index");

	status = TEST_SUCCESS;

error:
	array_destroy(array, NULL);

	return status;
}


TestStatus array_expand_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t a[4] = { 3, 12, 83, 1231453 };
	uint32_t i = 0;
	uint32_t length_orig = 2;
	uint32_t* value = NULL;
	Array* array = array_create(length_orig, sizeof(uint32_t));


	// calls
	for (i = 0; i < length_orig; ++i) {
		array_set(array, i, &(a[i]));
	}

	assert(array_expand(array) == SUCCESS, "Should be able to expand @array");
	assert(array->length == length_orig + ARRAY_EXPAND_RATE, "New length is incorect");
	// set the new memory to 0
	memset((uint8_t*)array->data + array->element_size * length_orig, 0, array->element_size * ARRAY_EXPAND_RATE);

	for (i = length_orig; i < 4; ++i) {
		array_set(array, i, &(a[i]));
	}

	// check content
	for (i = 0; i < 4; ++i) {
		value = (uint32_t*)array_get(array, i);
		assert(*value == a[i], "Value at index %u should be %u not %u", i, a[i], *value);
	}
	for (i = 4; i < array->length; i++) {
		value = (uint32_t*)array_get(array, i);
		assert(*value == 0, "Value at index %u should be 0 not %u", i, *value);
	}

	// corner cases
	assert(array_expand(NULL) == FAIL, "Should FAIL for invalid array");

	status = TEST_SUCCESS;

error:
	array_destroy(array, NULL);
	
	return status;
}


TestStatus array_show_test() {
	TestStatus status = FAIL;
	Array* array = array_create(10, sizeof(uint32_t));
	uint32_t i = 0;

	for (i = 0; i < 10; i++) {
		array_set(array, i, &i);
	}

	array_show(array, show_uint32_t);

	// corner cases
	array_show(NULL, NULL);

	status = TEST_SUCCESS;

	array_destroy(array, NULL);

	return status;
}
