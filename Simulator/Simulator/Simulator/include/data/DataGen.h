#ifndef __DATA_GEN_H__
#define __DATA_GEN_H__

#include "config.h"
#include "Network.h"


/*************************************************************
* STRUCTS
*************************************************************/
struct DataGenerator;
typedef struct DataGenerator DataGenerator;

struct DataElement;
typedef struct DataElement DataElement;


/*************************************************************
* DATA GENERATOR FUNCTIONALITY
*************************************************************/
typedef void __DataGeneratorData;
typedef BOOL(*__data_generator_data_is_valid)(__DataGeneratorData* data);
typedef void(*__data_generator_data_destroy)(__DataGeneratorData* data);
typedef DataElement* (*__data_generator_get_elem)(__DataGeneratorData* data, uint32_t idx);

// MAY NOT NEED THIS TO BE POLYMORPHIC
struct DataGenerator {
	uint32_t length;							// need to know the number of elements
	__DataGeneratorData* data;					// for aditional information
	__data_generator_data_is_valid data_is_valid;
	__data_generator_data_destroy data_destroy;
	__data_generator_get_elem get_elem;
};

// common functionality
BOOL data_generator_is_valid(DataGenerator* generator);
void data_generator_destroy(DataGenerator* generator);
DataElement* data_generator_get_element(DataGenerator* generator, uint32_t idx);


/*************************************************************
* DATA ELEMENT FUNCTIONALITY
*************************************************************/
typedef void __DataElementData;
typedef BOOL(*__data_element_data_is_valid)(__DataElementData* data);
typedef void(*__data_element_data_destroy)(__DataElementData* element);

// get the values for the network input
typedef NetworkInputs* (*__data_element_get_values)(DataElement* element, uint32_t time);
typedef void(*__date_element_remove_values)(DataElement* element, NetworkInputs* values);  // knows how to destroy what __data_element_get_values created

typedef struct DataElement {
	uint32_t duration;						// how much ms
	__DataElementData* data;
	__data_element_data_is_valid data_is_valid;
	__data_element_data_destroy data_destroy;
	__data_element_get_values get_values;
	__date_element_remove_values remove_values;
}DataElement;

#define data_element_get_values(element, t) ((element) != NULL && (element)->get_values != NULL ? (element)->get_values(element, t) : NULL)
#define data_element_remove_values(element, values) ((element) != NULL && (element->remove_values) != NULL ? (element)->remove_values(element, values) : 0)


// common functionality
BOOL data_element_is_valid(DataElement* element);
void data_element_destroy(DataElement* element);

void data_element_base_remove_values(DataElement* element, NetworkInputs* inputs);



#endif // __DATA_GEN_H__