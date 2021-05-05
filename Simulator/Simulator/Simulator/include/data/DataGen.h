#ifndef __DATA_GEN_H__
#define __DATA_GEN_H__

#include "config.h"
#include "Network.h"


/*************************************************************
* INTERFACE FOR DATA GENERATION
* 
* EACH GENERATOR NEEDS TO FOLLOW THIS INTERFACE TO ALLOW
* POLYMORPHISM
*************************************************************/

// The @DataGenerator creates @DataElements, for example it can read a set of images from disk and give the spike train encoding of every image 
struct DataGenerator;
typedef struct DataGenerator DataGenerator;

// The @DataElement is a spike train, like random spikes, or spikes that encode something. It has a @time component
struct DataElement;
typedef struct DataElement DataElement;


/*************************************************************
* DATA GENERATOR FUNCTIONALITY
*************************************************************/
// specific data, can be anything
typedef void __DataGeneratorData;

// check that the specific internal state is correct
typedef BOOL(*__data_generator_data_is_valid)(__DataGeneratorData* data);

// destroy the internal state
typedef void(*__data_generator_data_destroy)(__DataGeneratorData* data);

// get the @idx'th @DataElement, like the @idx'th image
typedef DataElement* (*__data_generator_get_elem)(__DataGeneratorData* data, uint32_t idx);


struct DataGenerator {
	uint32_t length;							// number of @DataElement's that can be generated
	__DataGeneratorData* data;					// specific internal state
	__data_generator_data_is_valid data_is_valid;
	__data_generator_data_destroy data_destroy;
	__data_generator_get_elem get_elem;
};

// Functionality that is common for each @DataGenerator, those will call the specific functions internally
BOOL data_generator_is_valid(DataGenerator* generator);
void data_generator_destroy(DataGenerator* generator);
DataElement* data_generator_get_element(DataGenerator* generator, uint32_t idx);


/*************************************************************
* DATA ELEMENT FUNCTIONALITY
*************************************************************/

// specific data, can be anything
typedef void __DataElementData;

// check if the internal state is ok
typedef BOOL(*__data_element_data_is_valid)(__DataElementData* data);

// destroys the internal state
typedef void(*__data_element_data_destroy)(__DataElementData* element);

// The @DataElement has a @time component to it, this will return the values for a given @time value
// WARNING: Here I assume that the network is stored in the @__DataElementData structure
typedef NetworkInputs* (*__data_element_get_values)(DataElement* element, uint32_t time);

// knows how to destroy what @__data_element_get_values created
typedef void(*__date_element_remove_values)(DataElement* element, NetworkInputs* values);  


typedef struct DataElement {
	uint32_t duration;						// time duration
	__DataElementData* data;
	__data_element_data_is_valid data_is_valid;
	__data_element_data_destroy data_destroy;
	__data_element_get_values get_values;
	__date_element_remove_values remove_values;
}DataElement;


// common functionality, these functions contain the boilerplate code that it is use for every @DataElement
BOOL data_element_is_valid(DataElement* element);
void data_element_destroy(DataElement* element);
NetworkInputs* data_element_get_values(DataElement* element, uint32_t time);
void data_element_remove_values(DataElement* element, NetworkInputs* values);


// helper functions

// general function to destroy a @NetworkInputs structure
void data_element_base_remove_values(DataElement* element, NetworkInputs* inputs);


#endif // __DATA_GEN_H__