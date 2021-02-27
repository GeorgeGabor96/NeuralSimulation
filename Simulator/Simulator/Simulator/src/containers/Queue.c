#include "Containers.h"
#include "utils/debug.h"
#include "utils/MemoryManagement.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status queue_is_valid(Queue* queue) {
	check(queue != NULL, null_argument("queue"));
	check(array_is_valid(&(queue->array)) == TRUE, invalid_argument("queue->array"));
	check(queue->head < queue->array.max_length, "@queue->head >= @queue->array.max_length");
	check(queue->tail < queue->array.max_length, "@queue->tail >= @queue->array.max_length");

	return TRUE;
ERROR
	return FALSE;
}


/*************************************************************
* Queue Functionality
*************************************************************/
Queue* queue_create(uint32_t length, size_t element_size) {
	Queue* queue = (Queue*)calloc(1, sizeof(Queue), "queue_create");
	Status status = FAIL;
	check_memory(queue);

	status = queue_init(queue, length, element_size);
	check(status == SUCCESS, "Could not init @queue");

	return queue;

ERROR
	if (queue != NULL) {
		free(queue);
	}
	return NULL;
}


Status queue_init(Queue* queue_p, uint32_t length, size_t element_size) {
	Status status = FAIL;
	queue_p->head = 0;
	queue_p->tail = 0;
	status = array_init(&(queue_p->array), length, 0, element_size);
	check(status == SUCCESS, "Could not init @queue->array");
	check(array_is_valid(&(queue_p->array)), invalid_argument("queue->array"));
ERROR
	return status;
}


void queue_reset(Queue* queue, ElemReset reset) {
	check(queue_is_valid(queue) == TRUE, invalid_argument("queue"));
	queue->head = 0;
	queue->tail = 0;
	array_reset(&(queue->array), reset);

ERROR
	return;
}


void queue_destroy(Queue* queue, ElemReset reset) {
	check(queue_is_valid(queue) == TRUE, invalid_argument("queue"));
	queue_reset(queue, reset);
	free(queue);

ERROR
	return;
}


Status queue_enqueue(Queue* queue, void* data) {
	Status status = FAIL;
	check(queue_is_valid(queue) == TRUE, invalid_argument("queue"));
	check(data != NULL, "NULL value for @data");

	if (queue_is_full(queue)) {
		uint32_t old_array_length = queue->array.max_length;
		status = array_expand(&(queue->array));
		check(status == SUCCESS, "Queue is full and could not allocate more memory");
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
	
	return SUCCESS;

ERROR
	return FAIL;
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

ERROR
	return element;
}


void* queue_head(Queue* queue) {
	void* element = NULL;
	check(queue_is_valid(queue) == TRUE, invalid_argument("queue"));

	if (!queue_is_empty(queue)) {
		element = array_get_fast(&(queue->array), queue->head);
	}

ERROR
	return element;
}