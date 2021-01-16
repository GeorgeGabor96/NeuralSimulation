#include <stdlib.h>

#include "Containers.h"
#include "debug.h"


Status stack_push(Stack* stack_p, void* data_p) {
	Status status = FAIL;
	check(stack_is_valid(stack_p), invalid_argument("stack_p"));
	check(data_p != NULL, "NULL value for @data_p");

	if (stack_is_full(stack_p)) {
		status = array_expand(stack_p);
		check(status == SUCCESS, "Stack is full and could not allocate more memory");
	}

	array_set_fast(stack_p, stack_p->length, data_p);
	++(stack_p->length);

	return SUCCESS;

error:
	return FAIL;
}


void* stack_pop(Stack* stack_p) {
	void* element_p = NULL;
	check(stack_is_valid(stack_p), invalid_argument("stack_p"));

	if (!stack_is_empty(stack_p)) {
		--(stack_p->length);
		element_p = array_get_fast(stack_p, stack_p->length);
	}

error:
	return element_p;
}


void* stack_top(Stack* stack_p) {
	void* element_p = NULL;
	check(stack_is_valid(stack_p), invalid_argument("stack_p"));

	if (!stack_is_empty(stack_p)) {
		element_p = array_get_fast(stack_p, stack_p->length - 1);
	}

error:
	return element_p;
}