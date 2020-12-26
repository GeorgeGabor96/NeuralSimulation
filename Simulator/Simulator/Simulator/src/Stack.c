#include <stdlib.h>

#include "Containers.h"
#include "debug.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status stack_is_valid(Stack* stack) {
	check(stack != NULL, null_argument("stack"));
	check(array_is_valid(&(stack->array)) == TRUE, invalid_argument("stack->array"));
	check(stack->top <= stack->array.length, "@stack->top is bigger than @stack->array.length");

	return TRUE;
error:
	return FALSE;
}


/*************************************************************
* Stack Functionality
*************************************************************/
Stack* stack_create(uint32_t length, size_t element_size) {
	Stack* stack = (Stack*)malloc(sizeof(Stack));
	check_memory(stack);

	stack->array.data = malloc(length * element_size);
	check_memory(stack->array.data);

	stack->top = 0;
	stack->array.length = length;
	stack->array.element_size = element_size;

	return stack;

error:
	if (stack != NULL) {
		free(stack);
	}
	return NULL;
}


void stack_destroy(Stack* stack, ElemReset reset) {
	check(stack_is_valid(stack), invalid_argument("stack"));
	array_reset(&(stack->array), reset);
	free(stack->array.data);
	free(stack);

error:
	return;
}


Status stack_push(Stack* stack, void* data) {
	Status status = FAIL;
	check(stack_is_valid(stack), invalid_argument("stack"));
	check(data != NULL, "NULL value for @data");

	if (stack_is_full(stack)) {
		check(array_expand(&(stack->array)) == SUCCESS, "Stack is full and could not allocate more memory");
	}

	array_set_fast(&(stack->array), (stack->top)++, data);

	status = SUCCESS;

error:
	return status;
}


void* stack_pop(Stack* stack) {
	void* element = NULL;
	check(stack_is_valid(stack), invalid_argument("stack"));

	if (!stack_is_empty(stack)) {
		element = array_get_fast(&(stack->array), --(stack->top));
	}

error:
	return element;
}


void* stack_top(Stack* stack) {
	void* element = NULL;
	check(stack_is_valid(stack), invalid_argument("stack"));

	if (!stack_is_empty(stack)) {
		element = array_get_fast(&(stack->array), stack->top - 1);
	}

error:
	return element;
}