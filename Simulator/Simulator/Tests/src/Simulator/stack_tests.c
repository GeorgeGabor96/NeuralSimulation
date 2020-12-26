#include "stack_tests.h"
#include "Containers.h"


TestStatus stack_create_test() {
	// setup
	uint32_t length = 10;
	size_t element_size = sizeof(uint32_t);
	Stack* stack = stack_create(length, element_size);
	TestStatus status = TEST_SUCCESS;
	
	// checks
	assert(stack != NULL, "@stack should be valid, not NULL");
	assert(stack->top == 0, "@stack->top should be 0 initially");
	assert(stack->array.length == length, "@stack->array.length should be %u", length);
	assert(stack->array.element_size == element_size, "@stack->array.element_size should be %llu", element_size);
	assert(stack->array.data != NULL, "@stack->array.data should be valid, not NULL");

	status = TEST_SUCCESS;

	// cleanup
error:
	if (stack != NULL) {
		stack_destroy(stack, NULL);
	}
	return status;
}


TestStatus stack_destroy_test() {
	// setup
	TestStatus status = TEST_FAILED;
	Stack* stack = stack_create(10, sizeof(int));
	void* data = NULL;

	// call with queue = NULL
	stack_destroy(NULL, NULL);

	// call with queue->array.data = NULL
	data = stack->array.data;
	stack->array.data = NULL;
	stack_destroy(stack, NULL);
	stack->array.data = data;

	// normal call
	stack_destroy(stack, NULL);

	status = SUCCESS;

	// cleanup
	return status;
}


TestStatus stack_push_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t length = 2;
	size_t element_size = sizeof(uint32_t);
	void* data = NULL;
	Stack* stack = stack_create(length, element_size);
	uint32_t value1 = 10;
	uint32_t value2 = 20;
	uint32_t value3 = 77;
	uint32_t* value_adr = NULL;
	
	// push elements on stack
	assert(stack_push(stack, &value1) == SUCCESS, "Should be able to push %u", value1);
	assert(stack->top == 1, "@stack->top should be 1");
	assert(stack_push(stack, &value2) == SUCCESS, "Should be able to push %u", value2);
	assert(stack->top == 2, "@stack->top should be 2");
	assert(stack_is_full(stack), "@stack should be full");
	assert(stack_push(stack, &value3) == SUCCESS, "Should be able to push %u", value3);
	assert(stack->top == 3, "@stack->top should be 3");
	assert(!stack_is_full(stack), "@stack should not be full");
	assert(stack->array.length == length + ARRAY_EXPAND_RATE, "@stack length should be %u, not %u", length + ARRAY_EXPAND_RATE, stack->array.length);

	// get the values
	value_adr = (uint32_t*)stack_pop(stack);
	assert(*value_adr == value3, "First element in stack should be %u, not %u", value3, *value_adr);
	value_adr = (uint32_t*)stack_pop(stack);
	assert(*value_adr == value2, "Second element in stack should be %u, not %u", value2, *value_adr);
	value_adr = (uint32_t*)stack_pop(stack);
	assert(*value_adr == value1, "Third eleemnt in stack should be %u, not %u", value1, *value_adr);
	assert(stack_is_empty(stack), "Stack should be empty");

	// stack = NULL
	assert(stack_push(NULL, &value1) == FAIL, "Should fail for @stack = NULL");
	// stack->array.data = NULL
	data = stack->array.data;
	stack->array.data = NULL;
	assert(stack_push(stack, &value1) == FAIL, "Should fail for @stack->array.data = NULL");
	stack->array.data = data;
	// data = NULL
	assert(stack_push(stack, NULL) == FAIL, "Should fail for @data = NULL");

	status = TEST_SUCCESS;

	// cleanup
error:
	stack_destroy(stack, NULL);
	return TEST_SUCCESS;
}


TestStatus stack_pop_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t length = 10;
	size_t element_size = sizeof(uint32_t);
	void* data = NULL;
	Stack* stack = stack_create(length, element_size);
	uint32_t value1 = 10;
	uint32_t value2 = 20;
	uint32_t value3 = 77;
	uint32_t* value_adr = NULL;

	// setup values in stack
	memcpy((uint8_t*)stack->array.data + 0 * element_size, &value1, element_size);
	memcpy((uint8_t*)stack->array.data + 1 * element_size, &value2, element_size);
	memcpy((uint8_t*)stack->array.data + 2 * element_size, &value3, element_size);
	stack->top = 3;

	// get values from stack
	value_adr = (uint32_t*)stack_pop(stack);
	assert(*value_adr == value3, "First element should have been %u, not %u", value3, *value_adr);
	assert(stack->top == 2, "@stack->top should have been 2");
	
	value_adr = (uint32_t*)stack_pop(stack);
	assert(*value_adr == value2, "Second element should have been %u, not %u", value2, *value_adr);
	assert(stack->top == 1, "@stack->top should have been 1");
	
	value_adr = (uint32_t*)stack_pop(stack);
	assert(*value_adr == value1, "Third element should have been %u, not %u", value1, *value_adr);
	assert(stack->top == 0, "@stack->top should have benn 0");
	
	value_adr = (uint32_t*)stack_pop(stack);
	assert(value_adr == NULL, "Stack should not have any more elements");
	assert(stack_is_empty(stack), "Stack should be empty");
	assert(stack->top == 0, "@stack->top should have been 0 when the stack is empty");

	// stack == NULL
	value_adr = (uint32_t*)stack_pop(NULL);
	assert(value_adr == NULL, "Result should be NULL for @stack = NULL");

	// stack->array.data = NULL
	data = stack->array.data;
	stack->array.data = NULL;
	value_adr = (uint32_t*)stack_pop(stack);
	assert(value_adr == NULL, "Result should be NULL for @stack->array.data = NULL");
	stack->array.data = data;
	
	status = TEST_SUCCESS;

	// cleanup
error:
	stack_destroy(stack, NULL);
	return status;
}


TestStatus stack_top_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t length = 10;
	size_t element_size = sizeof(uint32_t);
	void* data = NULL;
	Stack* stack = stack_create(length, element_size);
	uint32_t value1 = 10;
	uint32_t value2 = 20;
	uint32_t value3 = 77;
	uint32_t* value_adr = NULL;

	// push elements on stack
	stack_push(stack, &value1);
	stack_push(stack, &value2);
	stack_push(stack, &value3);

	// get the values
	value_adr = (uint32_t*)stack_top(stack);
	assert(*value_adr == value3, "Top element in stack should be %u, not %u", value3, *value_adr);
	assert(stack->top == 3, "@stack->top should be 3");
	stack_pop(stack);

	value_adr = (uint32_t*)stack_top(stack);
	assert(*value_adr == value2, "Top element in stack should be %u, not %u", value2, *value_adr);
	assert(stack->top == 2, "@stack->top should be 2");
	stack_pop(stack);
	
	value_adr = (uint32_t*)stack_top(stack);
	assert(*value_adr == value1, "Top eleemnt in stack should be %u, not %u", value1, *value_adr);
	assert(stack->top == 1, "@stack->top should be 1");
	stack_pop(stack);

	value_adr = (uint32_t*)stack_top(stack);
	assert(value_adr == NULL, "Should have returned NULL for empty stack");
	assert(stack->top == 0, "@stack->top should be 0");

	// stack = NULL
	assert(stack_top(NULL) == NULL, "Should fail for @stack = NULL");
	
	// stack->array.data = NULL
	data = stack->array.data;
	stack->array.data = NULL;
	assert(stack_top(stack) == NULL, "Should fail for @stack->array.data = NULL");
	stack->array.data = data;

	status = TEST_SUCCESS;

	// cleanup
error:
	stack_destroy(stack, NULL);
	return TEST_SUCCESS;
}