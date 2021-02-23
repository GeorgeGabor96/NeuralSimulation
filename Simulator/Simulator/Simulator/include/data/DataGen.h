#ifndef __DATA_GEN_H__
#define __DATA_GEN_H__

#include "config.h"
#include "Network.h"

struct DataGenerator;
typedef struct DataGenerator DataGenerator;

struct DataElement;
typedef struct DataElement DataElement;


typedef void __DataGeneratorData;
typedef BOOL(*__data_generator_is_valid)(DataGenerator* data);
typedef void(*__data_generator_destroy)(DataGenerator* data);
typedef DataElement* (*__data_generator_get_elem)(DataGenerator* data, uint32_t idx);

// MAY NOT NEED THIS TO BE POLYMORPHIC
struct DataGenerator {
	uint32_t length;							// need to know the number of elements
	__DataGeneratorData* data;					// for aditional information
	__data_generator_is_valid is_valid;
	__data_generator_destroy destroy;
	__data_generator_get_elem get_elem;
};

#define data_generator_is_valid(data) ((data) != NULL && (data)->is_valid != NULL ? (data)->is_valid(data) : FALSE)
#define data_generator_destroy(data) ((data) != NULL && (data)->destroy != NULL ? (data)->destroy(data) : 0)
#define data_generator_get_element(data, idx) ((data) != NULL && (data)->get_elem != NULL ? (data)->get_elem(data, idx) : NULL)


typedef void __DataElementData;
typedef BOOL(*__data_element_is_valid)(DataElement* element);
typedef void(*__data_element_destroy)(DataElement* element);

// get the values for the network input
typedef NetworkInputs* (*__data_element_get_values)(DataElement* element, uint32_t time);
typedef void(*__date_element_remove_values)(DataElement* element, NetworkInputs* inputs);  // knows how to destroy what __data_element_get_values created

typedef struct DataElement {
	uint32_t duration;						// how much ms
	__DataElementData* data;
	__data_element_is_valid is_valid;
	__data_element_destroy destroy;
	__data_element_get_values get_values;
	__date_element_remove_values remove_values;
}DataElement;

#define data_element_is_valid(element) ((element) != NULL && (element)->is_valid != NULL ? (element)->is_valid(element) : FALSE)
#define data_element_destroy(element) ((element) != NULL && (element)->destroy != NULL ? (element)->destroy(element) : FALSE)
#define data_element_get_values(element, time) ((element) != NULL && (element)->get_values != NULL ? (element)->get_values(element, time) : NULL)

#endif // __DATA_GEN_H__