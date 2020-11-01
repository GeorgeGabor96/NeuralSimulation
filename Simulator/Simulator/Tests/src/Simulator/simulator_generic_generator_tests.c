#include "simulator_generic_generator_tests.h"
#include "GenericArray.h"


TestStatus simulator_generic_generator_create_test() {
	// setup
	uint32_t max_length = 10;
	size_t element_size_in_bytes = sizeof(uint32_t);
	TestStatus status = TEST_FAILED;

	// call
	GenericArray* vector = generic_array_create(max_length, element_size_in_bytes);
	
	// checks
	assert(vector != NULL, "vector should not be NULL");
	assert(vector->data != NULL, "vector->data should not be NULL");
	assert(vector->max_length == max_length, "vector->max_length should be %d", max_length);
	assert(vector->element_size_in_bytes == element_size_in_bytes, "vector->element_size_in_bytes should be %lld", element_size_in_bytes);
	assert(vector->length == 0, "vector->length should be 0");

	status = TEST_SUCCESS;

	// cleanup
error:
	if (vector != NULL) {
		generic_array_destroy(vector);
	}
	return status;
}


TestStatus simulator_generic_array_get_at_index_test() {
	// setup
	uint32_t value1 = 1000;
	uint32_t value2 = 20;
	uint32_t* vector_value_adr = NULL;
	uint32_t index1 = 5;
	uint32_t index2 = 1;
	size_t element_size = sizeof(value1);
	GenericArray* vector = generic_array_create(10, element_size);
	TestStatus status = TEST_FAILED;

	memset(vector->data, 0, vector->max_length * vector->element_size_in_bytes);
	*((uint32_t*)((vector->data) + (index1 * element_size))) = value1;
	*((uint32_t*)((vector->data) + (index2 * element_size))) = value2;
	

	// call and check 1 - get value 1
	generic_array_get_at_index(vector, index1, vector_value_adr, uint32_t*);
	assert(*vector_value_adr == value1, "Value at index %d should be %d not %d", (int32_t)index1, value1, *vector_value_adr);

	// call and check 2 - get value 2
	generic_array_get_at_index(vector, index2, vector_value_adr, uint32_t*);
	assert(*vector_value_adr == value2, "Value at index %d should be %d not %d", (int32_t)index2, value2, *vector_value_adr);

	// call and check 3 - value unset
	generic_array_get_at_index(vector, 0, vector_value_adr, uint32_t*);
	assert(*vector_value_adr == 0, "Value at index %d should be %d not %d", 0, 0, *vector_value_adr);

	// call and check 4 - negative index
	generic_array_get_at_index(vector, -1, vector_value_adr, uint32_t*);
	assert(vector_value_adr == NULL, "Should have returned NULL for index %d", -1);

	// call and check 5 - index too big
	generic_array_get_at_index(vector, vector->max_length, vector_value_adr, uint32_t*);
	assert(vector_value_adr == NULL, "Should have returned NULL for index %d", vector->max_length);

	// call and check 6 - vector is NULL
	generic_array_get_at_index((GenericArray*) NULL, 0, vector_value_adr, uint32_t*);
	assert(vector_value_adr == NULL, "Should have returned NULL for input @vector NULL");

	status = TEST_SUCCESS;

	// cleanup
error:
	if (vector != NULL) {
		generic_array_destroy(vector);
	}
	return status;
}


TestStatus simulator_generic_array_set_at_index() {
	// setup 
	uint32_t value1 = 10;
	uint32_t value2 = 20;
	uint32_t* vector_value_adr = NULL;
	size_t index1 = 5;
	size_t index2 = 8;
	GenericArray* vector = generic_array_create(10, sizeof(value1));
	TestStatus status = TEST_FAILED;

	memset(vector->data, 0, vector->max_length * vector->element_size_in_bytes);

	// calls
	generic_array_set_at_index(vector, index1, &value1);
	generic_array_set_at_index(vector, index2, &value2);
	generic_array_set_at_index(vector, -1, &value1);				// negative index
	generic_array_set_at_index(vector, 10, &value1);				// index too big
	generic_array_set_at_index((GenericArray*) NULL, 0, &value1);	// NULL vector
	generic_array_set_at_index(vector, 0, NULL);					// NULL src_data

	// checks
	generic_array_get_at_index(vector, index1, vector_value_adr, uint32_t*);
	assert(value1 == *vector_value_adr, "Value at index %d should be %d not %d", (int32_t) index1, value1, *vector_value_adr);

	generic_array_get_at_index(vector, index2, vector_value_adr, uint32_t*);
	assert(value2 == *vector_value_adr, "Value at index %d should be %d not %d", (int32_t)index2, value2, *vector_value_adr);

	generic_array_get_at_index(vector, 2, vector_value_adr, uint32_t*);
	assert(0 == *vector_value_adr, "Value at index %d should be %d not %d", 2, 0, *vector_value_adr);


	status = TEST_SUCCESS;

	// cleanup
error:
	if (vector != NULL) {
		generic_array_destroy(vector);
	}
	return status;
}


TestStatus simulator_generic_array_append() {
	// setup
	uint32_t value1 = 381034;
	uint32_t value2 = 91829182;
	uint32_t value3 = 8181;
	uint32_t* vector_value_adr = NULL;
	GenericArray* vector = generic_array_create(3, sizeof(value1));
	TestStatus status = TEST_FAILED;

	memset(vector->data, 0, vector->max_length * vector->element_size_in_bytes);

	// calls
	generic_array_append(vector, &value1);
	generic_array_append(vector, &value2);
	generic_array_append(vector, &value3);
	generic_array_append(vector, &value3);					// not enought space
	generic_array_append((GenericArray*)NULL, &value1);		// NULL vector
	generic_array_append(vector, NULL);						// NULL src_data

	// checks
	assert(vector->length == 3, "Length of array should be %d not %d", 3, vector->length);
	generic_array_get_at_index(vector, 0, vector_value_adr, uint32_t*);
	assert(value1 == *vector_value_adr, "First value should be %d not %d", (int32_t)value1, *vector_value_adr);
	generic_array_get_at_index(vector, 1, vector_value_adr, uint32_t*);
	assert(value2 == *vector_value_adr, "Second value should be %d not %d", (int32_t)value2, *vector_value_adr);
	generic_array_get_at_index(vector, 2, vector_value_adr, uint32_t*);
	assert(value3 == *vector_value_adr, "Third value should be %d not %d", (int32_t)value3, *vector_value_adr);

	status = TEST_SUCCESS;

	// cleanup
error:
	if (vector != NULL) {
		generic_array_destroy(vector);
	}
	
	return status;
}