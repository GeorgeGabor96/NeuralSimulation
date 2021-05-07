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


/*************************************************************
* HELPER FUNCTIONALITY
*************************************************************/
// general function to destroy a @NetworkInputs structure
void data_element_base_remove_values(DataElement* element, NetworkInputs* inputs);


/*************************************************************
* DATA GENERATORS
*************************************************************/
/*
* Callback for generating a constant input current for a network
* 
* Params:
* @length - how many examples to generate
* @net - pointer to the @Network object for which to generate the input
* @current_value - value of the current
* @duration - duration of the example
*/
DataGenerator* data_generator_constant_current_create(uint32_t length, Network* net, float current_value, uint32_t duration);


/*
* Callback for generating random spikes as input for a network
* 
* Params:
* @n_examples - how many examples to generate
* @net - pointer to the @Network object for which to generate the input
* @spike_percent - the chance of generation a spike at any time
* @duration - duration of time for the examples
*/
DataGenerator* data_generator_random_spikes_create(uint32_t n_examples, Network* net, float spikes_percent, uint32_t duration);


/*
* This callback will generate one spike per neuron, sequentially with a step between them
* For example: first neuron will spike at time 0
*			   second neuron will spike at time @step_between_neurons
*			   third neuron will spike at time 2 * @step_between_neurons
*			   and so on
* 
* Params:
* @n_examples - number of examples to be generated
* @net - pointer to the network object for which to generate input
* @step_between_neurons - the interval of time between the spikes of succesive neurons
* @duration - the duration of each example
*/
DataGenerator* data_generator_with_step_between_neurons_create(uint32_t n_examples, Network* net, uint32_t step_between_neurons, uint32_t duration);


/*
* Callback for creating spike 'pulses'
* A pulse is define as a period of time in which the activity increases
* 
* Params:
* @n_examples - number of examples to generate
* @net - pointer to the network object for which to generate input
* @first_pulse_timestamp - the time value at which the first pulse starts
* @between_pulses_duration - the time between pulses
* @pulse_duration - duration of a pulse
* @between_pulses_spike_frequency - the probability for a spike when the pulse is not activ, between [0 - 1]
* @pulse_spike_frequency - the probability for a spike inside a pulse, between [0 - 1]
* @duration - the duration of the examples
*/
DataGenerator* data_generator_spike_pulses_create(
	uint32_t n_examples,
	Network* net,
	uint32_t first_pulse_timestamp,
	uint32_t between_pulses_duration,
	uint32_t pulse_duration,
	float between_pulses_spike_frequency,
	float pulse_spike_frequency,
	uint32_t duration);


#endif // __DATA_GEN_H__