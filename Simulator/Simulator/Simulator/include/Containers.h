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

typedef Array ArrayBool;
typedef Array ArrayFloat;
typedef Array ArrayUint8;
typedef Array ArrayUint16;
typedef Array ArrayUint32;


#define ARRAY_EXPAND_RATE 10 // TODO: if this is too much consider using a doubleling or some value per array that you give in the create
#define array_get_fast(a, i) ((a)->data + (a)->element_size * (i))
#define array_set_fast(a, i, d) memcpy((a)->data + (a)->element_size * (i), (void*) (d), (a)->element_size)
#define array_get_cast(a, i, t) ((t) array_get(a, i))
#define array_data_size(a) ((a)->length * (a)->element_size)
#define array_data_reset(a) (memset((a)->data, 0, array_data_size(a))) // sets all bytes in data to 0
#define array_is_full(a) ((a)->length == (a)->max_length)
#define array_is_empty(a) ((a)->length == 0)

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
Status array_init(Array* array, uint32_t length, uint32_t initial_length, size_t element_size);
void array_reset(Array* array, ElemReset reset);
void array_destroy(Array* array, ElemReset reset);
Status array_append(Array* array, void* data);
Status array_set(Array* array, uint32_t index, void* data);
void* array_get(Array* array, uint32_t index);
Status array_expand(Array* array);
void array_show(Array* array, ShowElem show);
void array_copy_data(Array* array, void* data, uint32_t start_idx, uint32_t elem_cnt);
Status array_swap(Array* array, uint32_t i, uint32_t j);

// stack functionality
typedef Array Stack;
#define stack_is_full(stack) array_is_full(stack)
#define stack_is_empty(stack) array_is_empty(stack)

/*
Stack is valid == array is valid
*/
#define stack_is_valid(stack) array_is_valid(stack)
#define stack_create(length, element_size) array_create(length, 0, element_size)
#define stack_destroy(stack, reset) array_destroy(stack, reset)

Status stack_push(Stack* stack_p, void* data_p);
void* stack_pop(Stack* stack_p);
void* stack_top(Stack* stack_p);


/*************************************************************
* String Functionality
*************************************************************/
// string have at most 256 chars currently
#define STRING_LIMIT 256
typedef Array String;
#define string_is_valid(s) array_is_valid(s)
String* string_create(char* c_string_p);
void string_destroy(String* string_p);
Array* strings_create(char** strings_pp, uint32_t cnt);
void strings_destroy(Array* strings_p);
int string_compare(String* string1_p, String* string2_p);


/*************************************************************
* Queue Functionality
*************************************************************/
typedef struct Queue {
	uint32_t head;		// removed
	uint32_t tail;		// added
	Array array;
} Queue;

#define queue_is_full(q) array_is_full(&((q)->array))
#define queue_is_empty(q) array_is_empty(&((q)->array))

/*
Verify that a queue is in a valid state, meaning:
1. queue != NULL
2. queue->array is valid
3. queue->head < queue->array.length
4. queue->tail < queue->array.length
*/
Status queue_is_valid(Queue* queue);
Status queue_init(Queue* queue_p, uint32_t length, size_t element_size);
Queue* queue_create(uint32_t length, size_t element_size);
void queue_reset(Queue* queue, ElemReset reset);
void queue_destroy(Queue* queue, ElemReset reset);
Status queue_enqueue(Queue* queue, void* data);
void* queue_dequeue(Queue* queue);
void* queue_head(Queue* queue);


#endif // __GENERIC_ARRAY_H__