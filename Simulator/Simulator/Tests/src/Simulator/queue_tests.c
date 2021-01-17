#include "queue_tests.h"
#include "Containers.h"


TestStatus queue_general_use_case_test() {
	TestStatus status = TEST_FAILED;
	Status s = FAIL;

	uint32_t length = 3;
	uint32_t i = 0;
	uint32_t value = 0;
	size_t element_size = sizeof(uint32_t);

	Queue* queue = queue_create(length, element_size);
	assert(queue_is_valid(queue) == TRUE, invalid_argument("queue"));
	assert(queue->head == 0, "@queue->head is %u, not 0", queue->head);
	assert(queue->tail == 0, "@queue->tail is %u, not 0", queue->tail);
	assert(queue->array.length == 0, "@queue->array.length is %u, not 0", queue->array.length);
	assert(queue->array.max_length == length, "@queue->array.max_length is %u, not 0", queue->array.max_length);
	assert(queue->array.element_size == element_size, "@queue->array.element_size is %llu, not %llu", queue->array.element_size, element_size);

	// add 5 elements
	for (i = 0; i < 5; ++i) {
		s = queue_enqueue(queue, &i);
		check(s == SUCCESS, "could not enqueue element");
	}
	assert(queue->array.max_length == length + ARRAY_EXPAND_RATE, "@queue->array.max_length si %u, not %u", queue->array.max_length, length + ARRAY_EXPAND_RATE);
	assert(queue->array.length == 5, "@queue->array.length is %u, not %u", queue->array.length, 5);
	for (i = 0; i < 5; ++i) {
		value = *((uint32_t*)queue_head(queue));
		assert(value == i, "@value is %u, not %u", value, i);

		value = *((uint32_t*)queue_dequeue(queue));
		assert(value == i, "@value is %u, not %u", value, i);
	}
	assert(queue_is_empty(queue) != FALSE, "@queue should be empty");

	// corner cases
	assert(queue_create(0, element_size) == NULL, "Should fail for invalid @length");
	assert(queue_create(length, 0) == NULL, "Should fail for invalid @element_size");

	assert(queue_enqueue(NULL, &value) == FAIL, "Should fail for invalid @queue");
	assert(queue_enqueue(queue, NULL) == FAIL, "Should fail for invalid @data");

	assert(queue_dequeue(NULL) == NULL, "Should return NULL for invalid @queue");

	assert(queue_head(NULL) == NULL, "Should return NULL for invalid @queue");

	queue_destroy(NULL, NULL);

	queue_destroy(queue, NULL);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus queue_memory_test() {
	TestStatus status = TEST_FAILED;

	// allocate a lot of queues and check memory
	Queue* queues[1000] = { NULL };
	uint32_t i = 0;
	uint32_t j = 0;
	for (i = 0; i < 1000; ++i) {
		queues[i] = queue_create(100, sizeof(uint32_t));
		for (j = 0; j < 1000; ++j) queue_enqueue(queues[i], &j);
	}
	for (i = 0; i < 1000; ++i) queue_destroy(queues[i], NULL);
	assert(memory_leak() == FALSE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}