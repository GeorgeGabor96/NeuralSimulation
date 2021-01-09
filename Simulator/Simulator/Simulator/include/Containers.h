#ifndef __GENERIC_ARRAY_H__
#define __GENERIC_ARRAY_H__

#include <stdint.h>
#include <string.h>

#include "debug.h"


typedef uint8_t bool;

/*
reset - function that knows how to free the memory inside an element (not the memory of the element itself)
NOTE: if elements are simple (ex: int) and no special deallocation is required, pass NULL to @destroy
*/
typedef void (*ElemReset) (void* elem);

/*************************************************************
* General Printing Functionality
*************************************************************/
typedef void(*ShowElem)(void*);
void show_float(void* data);
void show_bool(void* data);
void show_uint8_t(void* data);
void show_uint16_t(void* data);
void show_uint32_t(void* data);
void show_status(void* data);


/*************************************************************
* Array Functionality
*************************************************************/

// Use this when you already know how many elements you will save
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
2. array->length > 0
3. array->element_size > 0
2. array->data != NULL
*/
Status array_is_valid(Array* array);

Array* array_create(uint32_t length, size_t element_size);

// resets the content of the @array->data, @array remains valid
void array_reset(Array* array, ElemReset reset);

void array_destroy(Array* array, ElemReset reset);

void* array_get(Array* array, uint32_t index);

Status array_set(Array* array, uint32_t index, void* data);

// TODO: maybe it is better to use a more personalised value here, don't know, this may be sufficient
Status array_expand(Array* array);

void array_show(Array* array, ShowElem show);

// UNTESTED
void array_copy_data(Array* array, void* data, uint32_t start_idx, uint32_t elem_cnt);

Status array_swap(Array* array, uint32_t i, uint32_t j);



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

// use this when you are not sure about how many elements it will contain (list like functionality)
typedef struct Vector {
	uint32_t length;
	Array array;
} Vector;

#define VECTOR_EXPAND_RATE 10

#define vector_is_full(v) ((v)->length == (v)->array.length)

/*
Verify that @vector is valid, meaning:
1. vector != NULL
2. vector->length <= vector->array.length
3. vector->array is valid
*/
Status vector_is_valid(Vector* vector);

Vector* vector_create(uint32_t length, size_t element_size);

void vector_reset(Vector* vector, ElemReset destroy);
void vector_destroy(Vector* vector, ElemReset reset);

// index < vector->length
Status vector_set(Vector* vector, uint32_t index, void* data);

Status vector_append(Vector* vector, void* data);

// index < vector->length
void* vector_get(Vector* vector, uint32_t index);

void vector_show(Vector* vector, ShowElem show);

Status vector_swap(Vector* vector, uint32_t i, uint32_t j);


/*************************************************************
* String Functionality
*************************************************************/
// string have at most 256 chars currently
Array* string_create(char* string);
Vector* string_vector_create(char** strings, uint32_t cnt);
void string_destroy(Array* array);
void string_vector_destroy(Vector* strings);
int string_compare(Array* string1, Array* string2);

#endif // __GENERIC_ARRAY_H__