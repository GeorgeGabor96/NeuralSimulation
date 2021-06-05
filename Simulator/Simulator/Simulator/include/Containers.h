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
typedef Array ArrayDouble;
typedef Array ArrayFloat;
typedef Array ArrayUint8;
typedef Array ArrayUint16;
typedef Array ArrayUint32;
typedef Array String;


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
BOOL array_is_valid(Array* array);
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
Status array_resize(Array* array, uint32_t new_max_length);

// conversion functions
ArrayFloat* array_bool_to_float(ArrayBool* array_b, BOOL destroy_array_bool);
ArrayDouble* array_float_to_double(ArrayFloat* array_f, BOOL destroy_array_float);

// utility functions
ArrayFloat* array_arange_float(uint32_t length);
ArrayFloat* array_ones_float(uint32_t length);
ArrayBool* array_ones_bool(uint32_t length);
ArrayBool* array_zeros_bool(uint32_t length);
ArrayUint32* array_zeros_uint23(uint32_t length);
Status array_of_arrays_init(Array* data, uint32_t length, size_t inner_element_size);
Status array_of_arrays_reset(Array* data); // assumes primitives data types
ArrayUint32* array_random_int_uint32(uint32_t length, uint32_t min, uint32_t max);

// statictics functions
typedef struct GaussianDist{
	float mean;
	float std;
} GaussianDist;

GaussianDist* array_float_get_gaussian_dist(ArrayFloat* array);

// dumping funtions
void array_dump(Array* array, String* file_path, String* data_name, uint8_t type);
void array_float_dump(Array* array, String* file_path, String* data_name);
void array_bool_dump(Array* array, String* file_path, String* data_name);

/*************************************************************
* Stack Functionality
*************************************************************/
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
#define STRING_LIMIT 1024
BOOL string_is_valid(String* string);
Status string_init(String* string_p, const char* c_string_p);
String* string_create(const char* c_string_p);
const char* string_get_C_string(String* string);
String* string_copy(String* string);
void string_reset(String* string_p);
void string_destroy(String* string_p);
Array* strings_create(const char** strings_pp, uint32_t cnt);
void strings_destroy(Array* strings_p);

BOOL string_equal(String* string1_p, String* string2_p);
BOOL string_equal_C_string(String* string, const char* c_string);

// function for working with paths, will create a new string with independent memory
String* string_path_join_strings(String* string1, String* string2);
String* string_path_join_string_and_C(String* string1, const char* string2);
String* string_path_join_C_and_string(const char* string1, String* string2);

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