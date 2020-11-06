#include <stdlib.h>

#include "Containers.h"
#include "debug.h"


Queue* queue_create(uint32_t length, size_t element_size) {
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	check_memory(queue);

	queue->array.data = malloc(length * element_size);
	check_memory(queue->array.data);
	
	queue->length = 0;
	queue->head = 0;
	queue->tail = 0;

	return queue;

error:
	if (queue != NULL) {
		free(queue);
	}
	return NULL;
}


void queue_destroy(Queue* queue) {
	check(queue != NULL, "NULL value for @queue");
	if (queue->array.data != NULL) {
		free(queue->array.data);
	}
	free(queue);

error:
	return;
}


Status queue_enqueue(Queue* queue, void* data) {
	Status status = FAIL;
	check(queue != NULL, "NULL value for @queue");
	check(data != NULL, "NULL value for @data");

	if (queue_is_full(queue)) {
		uint32_t old_array_length = queue->array.length;
		check(array_expand(&(queue->array)) == SUCCESS, "Queue is full and could not allocate more memory");
		// reorder the queue if necessary
		if (queue->head != 0) {
			memcpy(array_get_fast(&(queue->array), old_array_length), &(queue->array), queue->array.element_size * queue->head);
			queue->tail = old_array_length + queue->head;
		}
		else {
			queue->tail = old_array_length;
		}

	}

	array_set_fast(&(queue->array), (queue->tail)++, data);
	if (queue->tail == queue->array.length) {
		queue->tail = 0;
	}
	(queue->length)++;
	status = SUCCESS;

error:
	return status;
}


void* queue_dequeue(Queue* queue) {
	void* element = NULL;
	check(queue != NULL, "NULL value for queue");

	element = array_get_fast(&(queue->array), (queue->head)++);
	if (queue->head == queue->array.length) {
		queue->head = 0;
	}
	(queue->length)--;

error:
	return element;
}


void* queue_head(Queue* queue) {
	void* element = NULL;
	check(queue != NULL, "NULL value for @queue");

	element = array_get_fast(&(queue->array), queue->head);

error:
	return element;
}