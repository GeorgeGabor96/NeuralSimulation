#ifndef __DATA_GEN_H__
#define __DATA_GEN_H__

#include "config.h"
#include "Network.h"


typedef struct DataGenerator {
	void* nothing; 
	uint32_t length;
}DataGenerator;

typedef struct DataElement {
	uint32_t length;
}DataElement;


BOOL data_generator_is_valid(DataGenerator* data);
void data_generator_destroy(DataGenerator* data);
DataElement* data_generator_get_element(DataGenerator* data, uint32_t idx);


BOOL data_element_is_valid(DataElement* elem);
NetworkInputs* data_element_get_values(DataElement* elem, uint32_t time);

#endif // __DATA_GEN_H__