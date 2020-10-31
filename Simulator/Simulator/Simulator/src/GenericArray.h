#ifndef __GENERIC_ARRAY_H__
#define __GENERIC_ARRAY_H__

#include <stdint.h>
#include <string.h>

typedef struct GenericArray {
	uint32_t max_length;
	uint32_t length;
	uint32_t element_size_in_bytes;
	void* data;
} GenericArray;


GenericArray* generic_array_create(uint32_t length, uint32_t element_size_in_bytes);
void generic_array_destroy(GenericArray* array);

//  getter macros
#define generic_array_get_at_index(array, index) ((array)->data[(array)->element_size_in_bytes * (index)])
#define generci_array_get_at_index_with_cast(array, index, type) ((type) generic_array_get_at_index(array, index))


// setter macros
#define generic_array_set_at_index(array, index, data) (memcpy(generic_array_get_at_index(array, index), (void*) data, (array)->element_size_in_bytes); ((array)->length)++;)
#define generic_array_append(array, data) (memcpy(generic_array_get_at_index(array, (array)->lenght), (void*) data, (array)->element_size_in_bytes); ((array)->lenght)++;)


#endif // __GENERIC_ARRAY_H__