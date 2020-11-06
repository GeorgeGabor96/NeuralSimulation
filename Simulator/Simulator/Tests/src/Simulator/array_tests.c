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
	Array* array = array_create(10, sizeof(int));

	array_destroy(NULL);
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

	// call and check 6 - vector is NULL
	array_value_adr = (uint32_t*) array_get((Array*)NULL, 0);
	assert(array_value_adr == NULL, "Should have returned NULL for input @vector NULL");

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
	TestStatus status = TEST_FAILED;

	memset(array->data, 0, array->length * array->element_size);

	// calls
	assert(array_set(array, index1, &value1) == SUCCESS, "Should have inserted %u at index %u", value1, index1);
	assert(array_set(array, index2, &value2) == SUCCESS, "Should have inserted %u at index %u", value1, index2);
	// negative inde
	assert(array_set(array, -1, &value1) == FAIL, "Set at index -1 should have FAILED");
	// index too big
	assert(array_set(array, 10, &value1) == FAIL, "Set at index 10 should have FAILED");
	// NULL vector
	assert(array_set((Array*)NULL, 0, &value1) == FAIL, "Should have FAILED for @array == NULL");
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
	return TEST_UNIMPLEMENTED;
}
