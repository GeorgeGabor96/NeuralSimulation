#ifndef __GENERIC_ARRAY_H__
#define __GENERIC_ARRAY_H__

#include <stdint.h>
#include <string.h>

#include "debug.h"

/*-----------TO DO-----------------*/
// La unele teste vezi ca ai dat destroy direct daca setai ceva intern pe NULL sa verifici o preconditie, daca chiar nu era ok ar pusca
// document when to use each of them

/*************************************************************
* Array Functionality
*************************************************************/

typedef struct Array {
	uint32_t length;
	size_t element_size; // in bytes
	uint8_t* data;
} Array;

#define ARRAY_EXPAND_RATE 100 // TODO: if this is too much consider using a doubleling or some value per array that you give in the create
#define array_get_fast(a, i) ((a)->data + (a)->element_size * (i))
#define array_set_fast(a, i, d) memcpy((a)->data + (a)->element_size * (i), (void*) (d), (a)->element_size)
#define array_get_cast(a, i, t) ((t) array_get(a, i))


/*
Verify that an @array is valid, meaning:
1. array != NULL
2. arra->data != NULL
*/
Status array_is_valid(Array* array);

/*
length - number of elements in the array
element_size - the size of an element
*/
Array* array_create(uint32_t length, size_t element_size);

/*
reset - function that knows how to free the memory inside an element (not the memory of the element itself)
NOTE: if elements are simple (ex: int) and no special deallocation is required, pass NULL to @destroy
*/
typedef void (*ElemReset) (void* elem);
// resets the content of the @array->data, @array remains valid
void array_reset(Array* array, ElemReset reset);
// completely destroys the @array
void array_destroy(Array* array, ElemReset reset);

void* array_get(Array* array, uint32_t index);

Status array_set(Array* array, uint32_t index, void* data);

/*
Increases the number of elements in @array by ARRAY_EXPAND_RATE
TODO: maybe it is better to use a more personalised value here, don't know, this may be sufficient
*/
Status array_expand(Array* array);



/*************************************************************
* Stack Functionality
*************************************************************/
typedef struct Stack {
	uint32_t top;   // points to the next element
	Array array;
} Stack;

#define stack_is_full(s) ((s)->top == (s)->array.length)
#define stack_is_empty(s) ((s)->top == 0)
#define stack_pop_cast(s, t) ((t) stack_pop(s))
#define stack_top_cast(s, t) ((t) stack_top(s))

/*
Verify that the @stack is valid, meaning:
1. stack != NULL
2. stack->array is valid
3. stack->top <= stack->array.length
*/
Status stack_is_valid(Stack* stack);

Stack* stack_create(uint32_t length, size_t element_size);

void stack_destroy(Stack* stack, ElemReset reset);

Status stack_push(Stack* stack, void* data);

void* stack_pop(Stack* stack);

void* stack_top(Stack* stack);


/*************************************************************
* Queue Functionality
*************************************************************/
typedef struct Queue {
	uint32_t length;
	uint32_t head;		// removed
	uint32_t tail;		// added
	Array array;
} Queue;

#define queue_is_full(q) ((q)->length == (q)->array.length)
#define queue_is_empty(q) ((q)->length == 0)
#define queue_dequeue_cast(q, t) ((t) queue_dequeue(q))
#define queue_head_cast(q, t) ((t) queue_head(q))

/*
Verify that a queue is in a valid state, meaning:
1. queue != NULL
2. queue->array is valid
3. queue->length <= queue->array.length
4. queue->head < queue->array.length
5. queue->tail < queue->array.length
*/
Status queue_is_valid(Queue* queue);

Queue* queue_create(uint32_t length, size_t element_size);

void queue_destroy(Queue* queue, ElemReset reset);

Status queue_enqueue(Queue* queue, void* data);

void* queue_dequeue(Queue* queue);

void* queue_head(Queue* queue);


/*************************************************************
* Vector Functionality
*************************************************************/
typedef struct Vector {
	uint32_t length;
	Array array;
} Vector;

#define VECTOR_EXPAND_RATE 10

#define vector_is_full(v) ((v)->length == (v)->array.length)

/*
Verify that @vector is valid, meaning:
1. vector != NULL
2. vector->array is valid
3. vector->length <= vector->array.length
*/
Status vector_is_valid(Vector* vector);

Vector* vector_create(uint32_t length, size_t element_size);

void vector_destroy(Vector* vector, ElemReset reset);

Status vector_set(Vector* vector, uint32_t index, void* data);

Status vector_append(Vector* vector, void* data);

void* vector_get(Vector* vector, uint32_t index);


#endif // __GENERIC_ARRAY_H__