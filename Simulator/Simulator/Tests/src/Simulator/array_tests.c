#include "array_tests.h"
#include "Containers.h"


TestStatus array_general_use_case_test() {
	TestStatus status = TEST_FAILED;
	
	uint32_t length = 10;
	uint32_t i = 0;
	uint32_t value = 0;
	size_t element_size = sizeof(uint32_t);

	// create
	Array* array = array_create(length, 0, element_size);
	assert(array != NULL, "@array is NULL");
	assert(array->data != NULL, "@array->data is NULL");
	assert(array->length == 0, "@array->length is %u, not 0", array->length);
	assert(array->max_length == 10, "@array->max_length is %u, not 10", array->max_length);
	assert(array->element_size == element_size, "@array->element_size is %llu, not %llu", array->element_size, element_size);

	// append
	for (i = 0; i < array->max_length; ++i) assert(array_append(array, &i) == SUCCESS, "@array_append failed");
	for (i = 0; i < array->length; ++i) {
		value = *((uint32_t*)array_get(array, i));
		assert(value == i, "@value is %u, not %u", value, i);
	}

	value = 25;
	assert(array_append(array, &value) == SUCCESS, "@array_expand failed");
	assert(array->max_length == 10 + ARRAY_EXPAND_RATE, "@array->max_length is %u, not %u", array->max_length, 10 + ARRAY_EXPAND_RATE);
	assert(array->length == 11, "@array->length is %u, not %u", array->length, 11);
	value = *((uint32_t*)array_get(array, 10));
	assert(value == 25, "@value is %u, not 25", value);

	// set
	uint32_t val_1 = 1234;
	uint32_t val_2 = 7832;
	uint32_t val_3 = 373737;
	assert(array_set(array, 9, &val_1) == SUCCESS, "Could not insert at index 9");
	assert(array_set(array, 7, &val_2) == SUCCESS, "Could not insert at index 7");
	assert(array_set(array, 0, &val_3) == SUCCESS, "Could not insert at index 0");
	
	// get
	value = *((uint32_t*)array_get(array, 7));
	assert(value == val_2, "At index 7 value %u, not %u", value, val_2);
	value = *((uint32_t*)array_get(array, 0));
	assert(value == val_3, "At index 0 value %u, not %u", value, val_3);
	value = *((uint32_t*)array_get(array, 9));
	assert(value == val_1, "At index 9 value %u, not %u", value, val_1);

	// corner cases
	assert(array_create(0, 0, 4) == NULL, "Should fail for invalid @length");
	assert(array_create(1, 2, 4) == NULL, "Should fail for invalid @initial_length");
	assert(array_create(1, 0, 0) == NULL, "Should fail for invalid @element_size");

	assert(array_append(NULL, &value) == FAIL, "Should fail for invalid @array");
	assert(array_append(array, NULL) == FAIL, "Should fail for invalid @data");

	assert(array_set(NULL, 1, &val_1) == FAIL, "Should fail for invalid array");
	assert(array_set(array, 11, &val_1) == FAIL, "Should fail for invalid index");
	assert(array_set(array, 1, NULL) == FAIL, "Should fail for invalid data");

	assert(array_get(NULL, 1) == NULL, "Should fail for invalid array");
	assert(array_get(array, 100) == NULL, "Should return NULL for invalid index");
	
	array_destroy(NULL, NULL);

	// destroy & check memory
	array_destroy(array, NULL);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus array_memory_test() {
	TestStatus status = TEST_FAILED;

	// allocate a lot of arrays and check memory
	Array* arrays[1000] = { NULL };
	uint32_t i = 0;
	uint32_t j = 0;
	for (i = 0; i < 1000; ++i) {
		arrays[i] = array_create(100, 0, sizeof(uint32_t));
		for (j = 0; j < 1000; ++j) array_append(arrays[i], &j);
	}
	for (i = 0; i < 1000; ++i) array_destroy(arrays[i], NULL);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus array_expand_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t a[4] = { 3, 12, 83, 1231453 };
	uint32_t i = 0;
	uint32_t length_orig = 2;
	uint32_t value = 0;
	Array* array = array_create(length_orig, 0,sizeof(uint32_t));
	// add elements
	for (i = 0; i < length_orig; ++i) {
		array_append(array, &(a[i]));
	}

	assert(array_expand(array) == SUCCESS, "Should be able to expand @array");
	assert(array->max_length == length_orig + ARRAY_EXPAND_RATE, "@array->max_length is %u, not %u", array->max_length, length_orig + ARRAY_EXPAND_RATE);
	assert(array->length == 2, "@array->length is %u, not %u", array->length, 2);
	// set the new memory to 0
	memset(array->data + array->element_size * length_orig, 0, array->element_size * (array->length - length_orig));

	for (i = length_orig; i < 4; ++i) {
		array_append(array, &(a[i]));
	}

	// check content
	for (i = 0; i < 4; ++i) {
		value = *((uint32_t*)array_get(array, i));
		assert(value == a[i], "Value at index %u should be %u not %u", i, a[i], value);
	}
	for (i = 4; i < array->length; i++) {
		value = *((uint32_t*)array_get(array, i));
		assert(value == 0, "Value at index %u should be 0 not %u", i, value);
	}

	// corner cases
	assert(array_expand(NULL) == FAIL, "Should FAIL for invalid array");

	// check memory
	array_destroy(array, NULL);
	assert(memory_leak() == FALSE, "Memory leak");
	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus array_show_test() {
	TestStatus status = FAIL;
	Array* array = array_create(10, 0, sizeof(uint32_t));
	uint32_t i = 0;

	for (i = 0; i < 10; i++) {
		array_append(array, &i);
	}

	array_show(array, show_uint32_t);
	printf("\n");

	// corner cases
	array_show(NULL, NULL);

	array_destroy(array, NULL);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus array_copy_data_test() {
	TestStatus status = FAIL;
	Array* array = array_create(10, 10, sizeof(uint32_t));
	uint32_t i = 0;
	uint32_t data[5] = { 0 };
	uint32_t value = 0;
	for (i = 0; i < 5; ++i) data[i] = i;

	uint32_t start_idx = 5;
	array_copy_data(array, data, start_idx, 5);
	for (i = start_idx; i < start_idx + 5; ++i) {
		value = *((uint32_t*)array_get(array, i));
		assert(value == data[i - start_idx], "@values is %u, not %u", value, data[i - start_idx]);
	}
	array_data_reset(array);

	start_idx = 2;
	array_copy_data(array, data, start_idx, 5);
	for (i = start_idx; i < start_idx + 5; ++i) {
		value = *((uint32_t*)array_get(array, i));
		assert(value == data[i - start_idx], "@values is %u, not %u", value, data[i - start_idx]);
	}
	array_data_reset(array);

	// trucate case
	start_idx = 8;
	array_copy_data(array, data, start_idx, 5);
	for (i = start_idx; i < start_idx + 2; ++i) {
		value = *((uint32_t*)array_get(array, i));
		assert(value == data[i - start_idx], "@values is %u, not %u", value, data[i - start_idx]);
	}

	// corner cases
	array_copy_data(NULL, data, 0, 5);
	array_copy_data(array, NULL, 0, 5);
	array_copy_data(array, data, 10, 5);
	array_copy_data(array, data, 0, 0);

	array_destroy(array, NULL);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus array_swap_test() {
	TestStatus status = TEST_FAILED;
	Array* array = array_create(10, 10, sizeof(uint32_t));
	uint32_t value_1 = 123;
	uint32_t value_2 = 321;
	uint32_t idx_1 = 1;
	uint32_t idx_2 = 5;
	uint32_t value = 0;

	array_set(array, idx_1, &value_1);
	array_set(array, idx_2, &value_2);

	array_swap(array, idx_1, idx_2);
	value = *((uint32_t*)array_get(array, idx_1));
	assert(value == value_2, "@value is %u, not %u", value, value_2);
	value = *((uint32_t*)array_get(array, idx_2));
	assert(value == value_1, "@value is %u, not %u", value, value_1);

	array_swap(array, idx_1, idx_2);
	value = *((uint32_t*)array_get(array, idx_1));
	assert(value == value_1, "@value is %u, not %u", value, value_1);
	value = *((uint32_t*)array_get(array, idx_2));
	assert(value == value_2, "@value is %u, not %u", value, value_2);

	// corner cases
	array_swap(NULL, 0, 1);
	array_swap(array, 100, 1);
	array_swap(array, 0, 100);

	array_destroy(array, NULL);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus array_float_get_gaussian_dist_test() {
	TestStatus status = TEST_FAILED;

	Array* array = array_create(10, 10, sizeof(float));
	uint32_t i = 0;
	float value = 0.0f;
	for (i = 0; i < 10; ++i) {
		value = (float)(i + 1);
		array_set(array, i, &value);
	}
	GaussianDist* dist = array_float_get_gaussian_dist(array);
	float_test(dist->mean, 5.5f);
	float_test(dist->std, 2.872281f);
	array_destroy(array, NULL);
	free(dist);

	array = array_create(10, 10, sizeof(size_t));
	dist = array_float_get_gaussian_dist(array);
	assert(dist == NULL, "Should return NULL for array of size_t");
	array_destroy(array, NULL);

	dist = array_float_get_gaussian_dist(NULL);
	assert(dist == NULL, "Should return NULL for invalid array");

	assert(memory_leak() == FALSE, "Memory leak");
	status = TEST_SUCCESS;
error:
	return status;
}
