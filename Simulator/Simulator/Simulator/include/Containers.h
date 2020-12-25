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

#define ARRAY_EXPAND_RATE 100
#define array_get_fast(a, i) ((a)->data + (a)->element_size * (i))
#define array_set_fast(a, i, d) memcpy((a)->data + (a)->element_size * (i), (void*) (d), (a)->element_size)
#define array_get_cast(a, i, t) ((t) array_get(a, i))

Array* array_create(uint32_t length, size_t element_size);

/*
Preconditions:	@array != NULL
				@array->data != NULL
*/
void array_destroy(Array* array);

/*
Preconditions: @array != NULL
               @index < @array->max_length
			   @array->data != NULL
*/
void* array_get(Array* array, uint32_t index);

/*
Preconditions: @array != NULL
			   @array->data != NULL
			   @index < @array->max_length
			   @data != NULL
*/
Status array_set(Array* array, uint32_t index, void* data);

/*
Preconditions: @array != NULL
			   @array->data != NULL
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

Stack* stack_create(uint32_t length, size_t element_size);

/*
Preconditions:	@stack != NULL
				@stack->array.data != NULL
*/
void stack_destroy(Stack* stack);

/*
Preconditions:	@stack != NULL
				@stack->array.data != NULL
				@data != NULL
*/
Status stack_push(Stack* stack, void* data);

/*
Preconditions:	@stack != NULL
				@stack->array.data != NULL
*/
void* stack_pop(Stack* stack);

/*
Preconditions:	@stack != NULL
				@stack->array.data != NULL
*/
void* stack_top(Stack* stack);


/*************************************************************
* Stack Functionality
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

Queue* queue_create(uint32_t length, size_t element_size);

/*
Preconditions:	@queue != NULL
				@queue->array.data != NULL
*/
void queue_destroy(Queue* queue);

/*
Preconditions:	@queue != NULL
				@queue->array.data != NULL
				@data != NULL
*/
Status queue_enqueue(Queue* queue, void* data);

/*
Preconditions:	@queue != NULL
				@queue->array.data != NULL
*/
void* queue_dequeue(Queue* queue);

/*
Preconditions:	@queue != NULL
				@queue->array.data != NULL
*/
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

Vector* vector_create(uint32_t length, size_t element_size);

/*
Preconditions:	@vector != NULL
				@vector->array.data != NULL
*/
void vector_destroy(Vector* vector);

/*
Preconditions:	@vector != NULL
				@vector->array.data != NULL
				@data != NULL
*/
Status vector_set(Vector* vector, uint32_t index, void* data);

/*
Preconditions:	@vector != NULL
				@vector->array.data != NULL
				@data != NULL
*/
Status vector_append(Vector* vector, void* data);

/*
Preconditions:	@vector != NULL
				@vector->array.data != NULL;
*/
void* vector_get(Vector* vector, uint32_t index);


#endif // __GENERIC_ARRAY_H__