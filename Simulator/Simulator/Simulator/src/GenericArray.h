#ifndef __GENERIC_ARRAY_H__
#define __GENERIC_ARRAY_H__

#include <stdint.h>
#include <string.h>

#include "debug.h"


typedef struct GenericArray {
	uint32_t max_length;
	uint32_t length;
	size_t element_size_in_bytes; // needs to be a size_t to avoid overflow at pointer indexint in x64
	uint8_t* data;
} GenericArray;


/*
* CONSTRUCTOR function for GenericArray
* Parameters: - @max_length			   - IN - the maximum number of elements that are needed
*             - @element_size_in_bytes - IN - the size in memory that an element occupies
*/
GenericArray* generic_array_create(uint32_t max_length, size_t element_size_in_bytes);

/*
* DESTRUCTOR function for GenericArray
* Parameters: @array - IN - pointer to a valid GenericArray
*/
void generic_array_destroy(GenericArray* array);


// GETTER macros
/*
* GETTER macro for GenericArray
* !!!!!!!!! This is an unsafe, but fast, implementation, ONLY to be USED when you are sure that OUT OF BOUND acesses DON"T HAPPEN
* Parameters: - @vector - IN - pointer to a VALID GenericArray structure
*             - @index  - IN - a VALID index the an element
*             - @type   - IN - the type of the element
* Return: Pointer casted to @type of the memory address where the @index element is located
*/
#define generic_array_get_at_index_fast_unsafe(vector, index, type) ((type) ((vector)->data + (vector)->element_size_in_bytes * (index)))

/*
* GETTER macro for GenericArray
* this is a the normal safe implementation, it includes checks for input parameters
* Parametesr: - @vector - IN  - pointer to a GenericArray structure
*			  - @index  - IN  - index of the required element
*			  - @out	- OUT - pointer of type @type where it will be saved the start address of the element with index @index
*			  - @type   - IN  - the type of the element
*/
#define generic_array_get_at_index(vector, index, out, type)	if ((vector) == NULL) {																\
																	log_warning("Received NULL value for @vector");									\
																	(out) = NULL;																	\
																} else if ((index) >= 0 && (index) < (vector)->max_length) {						\
																	(out) = generic_array_get_at_index_fast_unsafe(vector, index, type);			\
																} else {																			\
																	log_warning("Attempt to set out of bound index %d", (int32_t) (index));			\
																	(out) = NULL;																	\
																}


// SETTER macros
/*
* SETTER macro for GenericArray
* !!!!!!! This is an unsafe, but fast implementation, ONLY use it when you are sure that OUT OF BOUND accesses DO NOT OCCUR and the parameters are VALID
* PARAMETERS: - @vector	  - IN - pointer to a VALID GenericArray structure
*             - @index    - IN - a VALID index of an element in the array
*			  - @src_data - IN - pointer to the memory address from which to copy the data for the element
*/
#define generic_array_set_at_index_fast_unsafe(vector, index, src_data) memcpy((vector)->data + (vector)->element_size_in_bytes * (index), (void*) (src_data), (vector)->element_size_in_bytes)

/*
* SETTER macro for GenericArray
* this is the normal safe implementation, it includes checks for the input parameters
* Parameters: - @vector   - IN - pointer to a GenericArray structure
*             - @index	  - IN - the index of the element to be set
*			  - @src_data - IN - pointer to the memory address from which to copy the data for the element
*/
#define generic_array_set_at_index(vector, index, src_data)		if ((vector) == NULL) {															\
																	log_warning("Recieved NULL value for @vector");								\
																} else if ((src_data) == NULL) {												\
																	log_warning("Received NULL value for @src_data");							\
																} else if ((index) >= 0 && (index) < (vector)->max_length) {					\
																	generic_array_set_at_index_fast_unsafe(vector, index, src_data);			\
																} else {																		\
																	log_warning("Attempt to set out of bound index %d", (int32_t) (index));		\
																}

/*
* SETTER macro for GenericArray
* this macro append elements to the end of the array, as long as their is space
* Parameters: - @vector   - IN - pointer to a GenericArray structure
*			  - @src_data - IN - pointer to the memory address from which to copy the data for the element
*/
#define generic_array_append(vector, src_data)	if ((vector) == NULL) {																\
													log_warning("Received NULL value for @vector");									\
												} else if ((src_data) == NULL) {													\
													log_warning("Received NULL value for @src_data");								\
												} else if ((vector)->length != (vector)->max_length) {								\
													generic_array_set_at_index_fast_unsafe(vector, (vector)->length, src_data);		\
													(vector)->length++;																\
												} else {																			\
													log_warning("Array is full");													\
												}


#endif // __GENERIC_ARRAY_H__