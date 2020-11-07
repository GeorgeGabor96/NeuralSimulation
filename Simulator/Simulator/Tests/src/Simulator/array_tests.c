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
		array_destroy(array);
	}
	return status;
}


TestStatus array_destroy_test() {
	// setup 
	Array* array = array_create(10, sizeof(int));
	void* data = NULL;

	// call with array = NULL
	array_destroy(NULL);

	// call with array->data = NULL
	data = array->data;
	array->data = NULL;
	array_destroy(array);
	array->data = data;

	// normal call
	array_destroy(array);

	return TEST_SUCCESS;
}


TestStatus array_get_test() {
	// setup
	uint32_t value1 = 1000;
	uint32_t value2 = 20;
	uint32_t* array_value_adr = NULL;
	uint32_t index1 = 5;
	uint32_t index2 = 1;
	size_t element_size = sizeof(value1);
	Array* array = array_create(10, element_size);
	void* data = NULL;
	TestStatus status = TEST_FAILED;

	memset(array->data, 0, array->length * array->element_size);
	*((uint32_t*)((array->data) + (index1 * element_size))) = value1;
	*((uint32_t*)((array->data) + (index2 * element_size))) = value2;


	// call and check 1 - get value 1
	array_value_adr = (uint32_t*) array_get(array, index1);
	assert(*array_value_adr == value1, "Value at index %u should be %u not %u", index1, value1, *array_value_adr);

	// call and check 2 - get value 2
	array_value_adr = (uint32_t*) array_get(array, index2);
	assert(*array_value_adr == value2, "Value at index %d should be %d not %d", index2, value2, *array_value_adr);

	// call and check 3 - value unset
	array_value_adr = (uint32_t*) array_get(array, 0);
	assert(*array_value_adr == 0, "Value at index %d should be %d not %d", 0, 0, *array_value_adr);

	// call and check 4 - negative index
	array_value_adr = (uint32_t*) array_get(array, -1);
	assert(array_value_adr == NULL, "Should have returned NULL for index %d", -1);

	// call and check 5 - index too big
	array_value_adr = (uint32_t*) array_get(array, array->length);
	assert(array_value_adr == NULL, "Should have returned NULL for index %u", array->length);

	// call and check 6 - array is NULL
	array_value_adr = (uint32_t*) array_get((Array*)NULL, 0);
	assert(array_value_adr == NULL, "Should have returned NULL for input @vector NULL");

	// call and check 7 - array->data is NULL
	data = array->data;
	array->data = NULL;
	array_value_adr = (uint32_t*)array_get(array, 0);
	assert(array_value_adr == NULL, "Should have returned NULL for @array->data NULL");
	array->data = data;

	status = TEST_SUCCESS;

	// cleanup
error:
	if (array != NULL) {
		array_destroy(array);
	}
	return status;
}


TestStatus array_set_test() {
	// setup 
	uint32_t value1 = 10;
	uint32_t value2 = 20;
	uint32_t* array_value_adr = NULL;
	uint32_t index1 = 5;
	uint32_t index2 = 8;
	Array* array = array_create(10, sizeof(value1));
	void* data = NULL;
	TestStatus status = TEST_FAILED;

	memset(array->data, 0, array->length * array->element_size);

	// calls
	assert(array_set(array, index1, &value1) == SUCCESS, "Should have inserted %u at index %u", value1, index1);
	assert(array_set(array, index2, &value2) == SUCCESS, "Should have inserted %u at index %u", value1, index2);
	// negative inde
	assert(array_set(array, -1, &value1) == FAIL, "Set at index -1 should have FAILED");
	// index too big
	assert(array_set(array, 10, &value1) == FAIL, "Set at index 10 should have FAILED");
	// NULL array
	assert(array_set((Array*)NULL, 0, &value1) == FAIL, "Should have FAILED for @array == NULL");
	// NULL array->data
	data = array->data;
	array->data = NULL;
	assert(array_set(array, 0, &value1) == FAIL, "Should have FAILED for @array->data == NULL");
	array->data = data;
	// NULL src_data
	assert(array_set(array, 0, NULL) == FAIL, "Should have FAILED for @data == NULL");

	// checks
	array_value_adr = (uint32_t*) array_get(array, index1);
	assert(value1 == *array_value_adr, "Value at index %d should be %u not %u", (int32_t)index1, value1, *array_value_adr);

	array_value_adr = (uint32_t*) array_get(array, index2);
	assert(value2 == *array_value_adr, "Value at index %d should be %u not %u", (int32_t)index2, value2, *array_value_adr);

	array_value_adr = (uint32_t*) array_get(array, 2);
	assert(0 == *array_value_adr, "Value at index %d should be %d not %d", 2, 0, *array_value_adr);

	status = TEST_SUCCESS;

	// cleanup
error:
	if (array != NULL) {
		array_destroy(array);
	}
	return status;
}


TestStatus array_expand_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t length = 2;
	uint32_t value1 = 1;
	uint32_t value2 = 2;
	uint32_t value3 = 3;
	uint32_t* value_adr = NULL;
	Array* array = array_create(length, sizeof(uint32_t));
	void* data = NULL;

	// calls
	assert(array_set(array, 0, &value1) == SUCCESS, "Should be able to set value for index 0");
	assert(array_set(array, 1, &value2) == SUCCESS, "Should be able to set value for index 1");
	assert(array_set(array, 2, &value3) == FAIL, "Should not be able to set value for index 2");
	assert(array_expand(array) == SUCCESS, "Should be able to expand @array");
	assert(array->length == length + ARRAY_EXPAND_RATE, "New length is incorect");
	memset((uint8_t*)array->data + array->element_size * length, 0, array->element_size * ARRAY_EXPAND_RATE);
	assert(array_set(array, 10, &value3) == SUCCESS, "Should be able to set value for index 10");

	// get checks
	value_adr = (uint32_t*)array_get(array, 0);
	assert(*value_adr == value1, "Value at index 0 should be %u", value1);
	value_adr = (uint32_t*)array_get(array, 1);
	assert(*value_adr == value2, "Value at index 1 should be %u", value2);
	value_adr = (uint32_t*)array_get(array, 2);
	assert(*value_adr == 0, "Value at index 2 should not be %u", 0);
	value_adr = (uint32_t*)array_get(array, 10);
	assert(*value_adr == value3, "Value at index 10 should be %u", value3);

	// array == NULL
	assert(array_expand(NULL) == FAIL, "Should return FAIL for NULL @array");
	// array->data == NULL
	data = array->data;
	array->data = NULL;
	assert(array_expand(array) == FAIL, "Should return FAIL for NULL @array->data");
	array->data = data;

	/*
	TODO: a more complicated test case is when the realloc FAILS, to test that need to MOCK realloc
	*/

	status = TEST_SUCCESS;

	// cleanup
error:
	if (array != NULL) {
		array_destroy(array);
	}
	return status;
}
