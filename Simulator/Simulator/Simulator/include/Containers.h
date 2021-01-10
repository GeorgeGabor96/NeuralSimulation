#ifndef __GENERIC_ARRAY_H__
#define __GENERIC_ARRAY_H__


#include "config.h"


/*
reset - function that knows how to free the memory inside an element (not the memory of the element itself)
NOTE: if elements are simple (ex: int) and no special deallocation is required, pass NULL to @destroy
*/
typedef void (*ElemReset) (void* elem);

/*************************************************************
* General Printing Functionality
*************************************************************/
typedef void(*ShowElem) (void*);
void show_float(void* data);
void show_bool(void* data);
void show_status(void* data);
void show_uint8_t(void* data);
void show_uint16_t(void* data);
void show_uint32_t(void* data);


/*******************************************************************
* Array Functionality - use when you know how many elements you need
*******************************************************************/
typedef struct Array {
	uint32_t length;
	uint32_t max_length;
	size_t element_size; // in bytes
	uint8_t* data;
} Array;

#define ARRAY_EXPAND_RATE 10 // TODO: if this is too much consider using a doubleling or some value per array that you give in the create
#define array_get_fast(a, i) ((a)->data + (a)->element_size * (i))
#define array_set_fast(a, i, d) memcpy((a)->data + (a)->element_size * (i), (void*) (d), (a)->element_size)
#define array_get_cast(a, i, t) ((t) array_get(a, i))
#define array_data_size(a) ((a)->length * (a)->element_size)
#define array_data_reset(a) (memset((a)->data, 0, array_data_size(a))) // sets all bytes in data to 0
#define array_size(length, element_size) (sizeof(Array) + (length) * (element_size))
#define array_is_full(a) ((a)->length == (a)->max_length)

/*
Verify that an @array is valid, meaning:
1. array != NULL
2. array->length <= array->array.max_length
3. array->max_length > 0
4. array->element_size > 0
5. array->data != NULL
*/
bool array_is_valid(Array* array);
Array* array_create(uint32_t length, uint32_t initial_length, size_t element_size);
void array_reset(Array* array, ElemReset reset); // resets the content of the @array->data, @array remains valid
void array_destroy(Array* array, ElemReset reset);
Status array_append(Array** array, void* data);
Status array_set(Array* array, uint32_t index, void* data);
void* array_get(Array* array, uint32_t index);
Status array_expand(Array** array);
void array_show(Array* array, ShowElem show);
void array_copy_data(Array* array, void* data, uint32_t start_idx, uint32_t elem_cnt);
Status array_swap(Array* array, uint32_t i, uint32_t j);


/*************************************************************
* String Functionality
*************************************************************/
// string have at most 256 chars currently
#define STRING_LIMIT 256
typedef Array String;
String* string_create(char* c_string_p);
void string_destroy(String* string_p);
Array* strings_create(char** strings_pp, uint32_t cnt);
void strings_destroy(Array* strings_p);
int string_compare(String* string1_p, String* string2_p);



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


#endif // __GENERIC_ARRAY_H__