#include "stack_tests.h"
#include "Containers.h"


TestStatus stack_general_use_case_test() {
	TestStatus status = TEST_FAILED;

	uint32_t length = 3;
	uint32_t i = 0;
	uint32_t value = 0;
	size_t element_size = sizeof(uint32_t);

	Stack* stack = stack_create(length, element_size);
	assert(stack != NULL, null_argument("stack"));
	assert(stack->data != NULL, null_argument("stack->data"));
	assert(stack->length == 0, "@stack->length is %u, not 0", stack->length);
	assert(stack->max_length == length, "@stack->max_length is %u, not %u", stack->max_length, length);
	assert(stack->element_size == element_size, "@array->element_size is %llu, not %llu", stack->element_size, element_size);

	// add 5 elements
	for (i = 0; i < 5; ++i) stack_push(stack, &i);
	assert(stack->max_length == length + ARRAY_EXPAND_RATE, "@stack->max_length is %u, not %u", stack->max_length, length + ARRAY_EXPAND_RATE);
	assert(stack->length == 5, "@stack->length is %u, not %u", stack->length, 5);
	for (i = 0; i < 5; ++i) {
		value = *((uint32_t*)stack_top(stack));
		assert(value == 4 - i, "top of the stack is %u, not %u", value, 4 - i);

		value = *((uint32_t*)stack_pop(stack));
		assert(value == 4 - i, "@value is %u, not %u", value, 4 - i);
	}
	assert(stack_is_empty(stack) != FALSE, "@stack should be empty");

	// corner cases
	assert(stack_create(0, element_size) == NULL, "Should fail for invalid @length");
	assert(stack_create(length, 0) == NULL, "Should fail for invalid @element_size");

	assert(stack_push(NULL, &value) == FAIL, "Should fail for invalid @stack");
	assert(stack_push(stack, NULL) == FAIL, "Should failf for invalid @data");

	assert(stack_pop(NULL) == NULL, "Should return NULL for invalid @stack");

	assert(stack_top(NULL) == NULL, "Should return NULL for invalid @stack");

	stack_destroy(NULL, NULL);

	stack_destroy(stack, NULL);
	assert(memory_leak() == TRUE, "Memory leak");

	status = TEST_SUCCESS;

error:
	return status;
}


TestStatus stack_memory_test() {
	TestStatus status = TEST_FAILED;

	// allocate a lot of stacks and check memory
	Stack* stacks[1000] = { NULL };
	uint32_t i = 0;
	uint32_t j = 0;
	for (i = 0; i < 1000; ++i) {
		stacks[i] = stack_create(100, sizeof(uint32_t));
		for (j = 0; j < 1000; ++j) stack_push(stacks[i], &j);
	}
	for (i = 0; i < 1000; ++i) stack_destroy(stacks[i], NULL);
	assert(memory_leak() == TRUE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}