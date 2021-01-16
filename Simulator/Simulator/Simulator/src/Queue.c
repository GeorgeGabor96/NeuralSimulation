#include <stdlib.h>

#include "Containers.h"
#include "debug.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status queue_is_valid(Queue* queue) {
	check(queue != NULL, null_argument("queue"));
	check(array_is_valid(&(queue->array)) == TRUE, invalid_argument("queue->array"));
	check(queue->head < queue->array.max_length, "@queue->head >= @queue->array.length");
	check(queue->tail < queue->array.max_length, "@queue->tail >= @queue->array.length");

	return TRUE;
error:
	return FALSE;
}


/*************************************************************
* Queue Functionality
*************************************************************/
Queue* queue_create(uint32_t length, size_t element_size) {
	Queue* queue = (Queue*)calloc(1, sizeof(Queue));
	Status status = FAIL;
	check_memory(queue);

	queue->head = 0;
	queue->tail = 0;
	status = array_init(&(queue->array), length, 0, element_size);
	check(status == SUCCESS, "Could not init @queue->array");
	check(array_is_valid(&(queue->array)), invalid_argument("queue->array"));

	return queue;

error:
	if (queue != NULL) {
		free(queue);
	}
	return NULL;
}


void queue_destroy(Queue* queue, ElemReset reset) {
	check(queue_is_valid(queue) == TRUE, invalid_argument("queue"));
	array_reset(&(queue->array), reset);
	free(queue);

error:
	return;
}


Status queue_enqueue(Queue* queue, void* data) {
	Status status = FAIL;
	check(queue_is_valid(queue) == TRUE, invalid_argument("queue"));
	check(data != NULL, "NULL value for @data");

	if (queue_is_full(queue)) {
		uint32_t old_array_length = queue->array.max_length;
		check(array_expand(&(queue->array)) == SUCCESS, "Queue is full and could not allocate more memory");
		// reorder the queue if necessary
		if (queue->head != 0) {
			memcpy(array_get_fast(&(queue->array), old_array_length), queue->array.data, queue->array.element_size * queue->head);
			queue->tail = old_array_length + queue->head;
		}
		else {
			queue->tail = old_array_length;
		}

	}

	array_set_fast(&(queue->array), (queue->tail)++, data);
	if (queue->tail == queue->array.max_length) {
		queue->tail = 0;
	}
	(queue->array.length)++;
	status = SUCCESS;

error:
	return status;
}


void* queue_dequeue(Queue* queue) {
	void* element = NULL;
	check(queue_is_valid(queue) == TRUE, invalid_argument("queue"));

	if (!queue_is_empty(queue)) {
		element = array_get_fast(&(queue->array), (queue->head)++);
		if (queue->head == queue->array.max_length) {
			queue->head = 0;
		}
		(queue->array.length)--;
	}

error:
	return element;
}


void* queue_head(Queue* queue) {
	void* element = NULL;
	check(queue_is_valid(queue) == TRUE, invalid_argument("queue"));

	if (!queue_is_empty(queue)) {
		element = array_get_fast(&(queue->array), queue->head);
	}

error:
	return element;
}