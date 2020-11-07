#include "queue_tests.h"
#include "Containers.h"


TestStatus queue_create_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t length = 10;
	size_t element_size = sizeof(uint32_t);

	// call
	Queue* queue = queue_create(length, element_size);

	// checks
	assert(queue != NULL, "@queue should not be NULL");
	assert(queue->array.data != NULL, "@queue->array.data should not be NULL");
	assert(queue->length == 0, "@queue->length should be 0");
	assert(queue->head == 0, "@queue->head should be 0");
	assert(queue->tail == 0, "@queue->tail should be 0");
	assert(queue->array.element_size == element_size, "@queue->array.element_size should be %llu, not %llu", element_size, queue->array.element_size);
	assert(queue->array.length == length, "@queue->array.length should be %u, not %u", length, queue->array.length);

	status = TEST_SUCCESS;

error:
	queue_destroy(queue);
	return status;
}


TestStatus queue_destroy_test() {
	// setup
	TestStatus status = TEST_FAILED;
	Queue* queue = queue_create(10, sizeof(int));
	void* data = NULL;

	// call with queue = NULL
	queue_destroy(NULL);

	// call with queue->array.data = NULL
	data = queue->array.data;
	queue->array.data = NULL;
	queue_destroy(queue);
	queue->array.data = data;

	// normal call
	queue_destroy(queue);

	status = TEST_SUCCESS;

	return status;
}


TestStatus queue_dequeue_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t value1 = 83;
	uint32_t value2 = 81;
	uint32_t value3 = 1313;
	uint32_t value4 = 823;
	uint32_t length = 3;
	size_t element_size = sizeof(uint32_t);
	uint32_t* value_adr = NULL;
	Queue* queue = queue_create(length, element_size);
	void* data = NULL;

	// add 3 elements
	memcpy((uint8_t*)queue->array.data + 0 * element_size, &value1, element_size);
	memcpy((uint8_t*)queue->array.data + 1 * element_size, &value2, element_size);
	memcpy((uint8_t*)queue->array.data + 2 * element_size, &value3, element_size);
	queue->tail = 0;
	queue->length = 3;

	// checks
	assert(queue_is_full(queue), "@queue should be full");
	// dequeue 2 elements
	value_adr = (uint32_t*)queue_dequeue(queue);
	assert(*value_adr == value1, "First value in queue should be %u, not %u", value1, *value_adr);
	assert(queue->head == 1, "@queue->head should be 1");
	assert(queue->length == 2, "@queue->length should be 2");

	value_adr = (uint32_t*)queue_dequeue(queue);
	assert(*value_adr == value2, "Second value in queue should be %u, not %u", value2, *value_adr);
	assert(queue->head == 2, "@queue->head should be 2");
	assert(queue->length == 1, "@queue->length should be 1");

	// add 1 element
	memcpy((uint8_t*)queue->array.data + 0 * element_size, &value4, element_size);
	queue->tail = 1;
	queue->length++;

	// dequeue 2 elements
	value_adr = (uint32_t*)queue_dequeue(queue);
	assert(*value_adr == value3, "Third value in queue should be %u, not %u", value3, *value_adr);
	assert(queue->head == 0, "@queue->head should be 0");
	assert(queue->length == 1, "@queue->length should be 1");

	value_adr = (uint32_t*)queue_dequeue(queue);
	assert(*value_adr == value4, "Second value in queue should be %u, not %u", value4, *value_adr);
	assert(queue->head == 1, "@queue->head should be 1");
	assert(queue->length == 0, "@queue->length should be 0");
	assert(queue_is_empty(queue), "@queue should be empty");

	// try to dequeue more
	value_adr = (uint32_t*)queue_dequeue(queue);
	assert(value_adr == NULL, "Should return NULL for empty queue");
	assert(queue->head == 1, "@queue->head should be 1");
	assert(queue->length == 0, "@queue->length should be 0");


	// call with @queue = NULL
	assert(queue_dequeue(NULL) == NULL, "Should return NULL for @queue = NULL");

	// call with @queue->array.data = NULL
	data = queue->array.data;
	queue->array.data = NULL;
	assert(queue_dequeue(queue) == NULL, "Should return NULL for @queue->array.data = NULL");
	queue->array.data = data;

	status = TEST_SUCCESS;

error:
	queue_destroy(queue);
	return status;
}


TestStatus queue_enqueue_test() {
	TestStatus status = TEST_FAILED;
	uint32_t value1 = 83;
	uint32_t value2 = 81;
	uint32_t value3 = 1313;
	uint32_t value4 = 823;
	uint32_t value5 = 111;
	uint32_t length = 3;
	size_t element_size = sizeof(uint32_t);
	uint32_t* value_adr = NULL;
	Queue* queue = queue_create(length, element_size);
	void* data = NULL;

	// enqueue 3 values
	assert(queue_enqueue(queue, &value1) == SUCCESS, "Should be able to enqueue %u", value1);
	assert(queue->tail == 1, "@queue->tail should be 1");
	assert(queue->length == 1, "@queue->length should be 1");

	assert(queue_enqueue(queue, &value2) == SUCCESS, "Should be able to enqueue %u", value2);
	assert(queue->tail == 2, "@queue->tail should be 2");
	assert(queue->length == 2, "@queue->length should be 2");
	
	assert(queue_enqueue(queue, &value3) == SUCCESS, "Should be able to enqueue %u", value3);
	assert(queue->tail == 0, "@queue->tail should be 0");
	assert(queue_is_full(queue), "@queue should be full");
	assert(queue->length == 3, "@queue->length should be 3");

	// dequeue value1
	value_adr = (uint32_t*) queue_dequeue(queue);
	assert(*value_adr == value1, "First value in queue should be %u, not %u", value1, *value_adr);
	assert(!queue_is_full(queue), "@queue should not be full");
	assert(queue->head == 1, "@queue->head should be 1");
	assert(queue->length == 2, "@queue->length should be 2");

	// enqueue value4
	assert(queue_enqueue(queue, &value4) == SUCCESS, "Should be able to enqueue %u", value4);
	assert(queue->tail == 1, "@queue->tail should be 1");
	assert(queue_is_full(queue), "@queue should be full");
	assert(queue->length == 3, "@queue->length should be 3");

	// enqueue value5
	assert(queue_enqueue(queue, &value5) == SUCCESS, "Should be able to enqueue %u", value5);
	assert(queue->head == 1, "@queue->head should be 1");
	assert(queue->tail == length + 2, "@queue->tail should be %u, not %u", length + 2, queue->tail);
	assert(!queue_is_full(queue), "@queue should not be full");
	assert(queue->length == 4, "@queue->length should be 4");
	assert(queue->array.length == length + ARRAY_EXPAND_RATE, "@queue->array.length should be %u, not %u", length + ARRAY_EXPAND_RATE, queue->array.length);

	// dequeue all values
	value_adr = (uint32_t*)queue_dequeue(queue);
	assert(*value_adr == value2, "Second value in queue should be %u, not %u", value2, *value_adr);
	assert(queue->head == 2, "@queue->head should be 2");
	assert(queue->length == 3, "@queue->length should be 3");

	value_adr = (uint32_t*)queue_dequeue(queue);
	assert(*value_adr == value3, "Third value in queue should be %u, not %u", value3, *value_adr);
	assert(queue->head == 3, "@queue->head should be 3");
	assert(queue->length == 2, "@queue->length should be 2");

	value_adr = (uint32_t*)queue_dequeue(queue);
	assert(*value_adr == value4, "Fourth value in queue should be %u, not %u", value4, *value_adr);
	assert(queue->head == 4, "@queue->head should be 4");
	assert(queue->length == 1, "@queue->length should be 1");

	value_adr = (uint32_t*)queue_dequeue(queue);
	assert(*value_adr == value5, "Fifth value in queue should be %u, not %u", value5, *value_adr);
	assert(queue->head == 5, "@queue->head should be 5");
	assert(queue->length == 0, "@queue->length should be 0");
	assert(queue->tail == queue->head, "@queue->head %u should equal @queue->tail %u", queue->head, queue->tail);
	assert(queue_is_empty(queue), "@queue should be empty");

	// call with queue = NULL
	assert(queue_enqueue(NULL, &value1) == FAIL, "Should return NULL for @queue = NULL");
	
	// call with queue->array.data = NULL
	data = queue->array.data;
	queue->array.data = NULL;
	assert(queue_enqueue(queue, &value1) == FAIL, "Should reutrn NULL for @queue->array.data = NULL");
	queue->array.data = data;

	// call with @data = NULL
	assert(queue_enqueue(queue, NULL) == FAIL, "Should return NULL for @data = NULL");

	status = TEST_SUCCESS;

error:
	queue_destroy(queue);
	return status;
}


TestStatus queue_head_test() {
	// setup
	TestStatus status = TEST_FAILED;
	uint32_t value1 = 83;
	uint32_t value2 = 81;
	uint32_t value3 = 1313;
	uint32_t value4 = 823;
	uint32_t length = 3;
	size_t element_size = sizeof(uint32_t);
	uint32_t* value_adr = NULL;
	Queue* queue = queue_create(length, element_size);
	void* data = NULL;

	// enqueue all data
	queue_enqueue(queue, &value1);
	queue_enqueue(queue, &value2);
	queue_enqueue(queue, &value3);
	queue_enqueue(queue, &value4);

	// calls
	value_adr = (uint32_t*)queue_head(queue);
	assert(*value_adr == value1, "First element in queue should be %u, not %u", value1, *value_adr);
	assert(queue->tail == 4, "@queue->tail should be 4");
	assert(queue->head == 0, "@queue->head should be 0");
	queue_dequeue(queue);

	value_adr = (uint32_t*)queue_head(queue);
	assert(*value_adr == value2, "Second element in queue should be %u, not %u", value2, *value_adr);
	assert(queue->tail == 4, "@queue->tail should be 4");
	assert(queue->head == 1, "@queue->head should be 1");
	queue_dequeue(queue);

	value_adr = (uint32_t*)queue_head(queue);
	assert(*value_adr == value3, "Third element in queue should be %u, not %u", value3, *value_adr);
	assert(queue->tail == 4, "@queue->tail should be 4");
	assert(queue->head == 2, "@queue->head should be 2");
	queue_dequeue(queue);

	value_adr = (uint32_t*)queue_head(queue);
	assert(*value_adr == value4, "Fourth element in queue should be %u, not %u", value4, *value_adr);
	assert(queue->tail == 4, "@queue->tail should be 4");
	assert(queue->head == 3, "@queue->head should be 3");
	queue_dequeue(queue);

	value_adr = (uint32_t*)queue_head(queue);
	assert(queue_is_empty(queue), "@queue should be empty");
	assert(value_adr == NULL, "Should return NULL for empty queue");
	assert(queue->tail == 4, "@queue->tail should be 4");
	assert(queue->head == 4, "@queue->head should be 4");
	
	// call with @queue = NULL
	assert(queue_head(NULL) == NULL, "Should return NULL for @queue = NULL");

	// call with @queue->array.data = NULL
	data = queue->array.data;
	queue->array.data = NULL;
	assert(queue_head(queue) == NULL, "Should return NULL for @queue->array.data = NULL");
	queue->array.data = data;

	status = SUCCESS;

	// cleanup
error:
	queue_destroy(queue);
	return status;
}
