#ifndef __NETWORK_H__
#define __NETWORK_H__


#include "config.h"
#include "Layer.h"
#include "Containers.h"


// TO DO: so if we want non linear networks, need a way to reference a layer by name/index

typedef enum { SPIKES = 0, CURRENT = 1, VOLTAGE = 2 } NetworkValueType;

// Chestia este ca input-ul si output-ul unei retele practic este acelasi structura
// caci ca input trebuie un vector de arrays cu valori si un tip (spike sau current)
// iar la output ai un vector de arrays cu valori si un tip (spike sau voltaj)

// if I add a type to the array and the other structures, i could directly use an array, and i could also verify the type when i receive an array
typedef struct NetworkValues {
	NetworkValueType type;
	Array* values;
} NetworkValues;
typedef Array NetworkInputs;
typedef Array NetworkOutputs;

void network_values_show(Array* values);

typedef struct Network {
	Array layers;			// will be added one by one
	Array output_layers;	// one or more, keeps references to @layers
	Array input_layers;		// one or more, keeps references to @layers
	Array input_names;		// names of the input layers, reference to @layers->name
	Array output_names;		// names of the output layers, reference to @layers->name
	BOOL compiled;			// if the network has been compiled
} Network;


/* 
* Network is valid if:
* 1. @network != NULL
* 2. @network->layers is valid
* 3. @network->output_layers is valid
* 4. @network->input_layers is valid
* 5. @network->input_names is valid
* 6. @network->output_names is valid
*/
BOOL network_is_valid(Network* network);

Network* network_create();
void network_destroy(Network* network);

/*
* This will copy the content of the @layer structure (shallow copy).
* The user need to decide if the memory location that holds the @layer structure is still nedeed
*
* Setting @should_free:
* should_free -> does a free(layer)
* 1. If the layer was on the stack then set @should_free = FALSE
* 2. If the layer was on the heap and the @layer structure is still needed then set @should_free = FALSE
* 3. If the layer was on the heap and the @layer structure is not needed anymore set @should_free = TRUE
*/
Status network_add_layer(Network* network, Layer* layer, BOOL should_free, BOOL is_input, BOOL is_output);

Layer* network_get_layer_by_idx(Network* network, uint32_t layer_idx);
Layer* network_get_layer_by_string(Network* network, String* name);
Layer* network_get_layer_by_name(Network* network, char* name);
uint32_t network_get_layer_idx_by_string(Network* network, String* name);
uint32_t network_get_layer_idx_by_name(Network* network, char* name);

/*
* Check that all the layers are valid and if there are missing layers
* Then it will connect every layer with its inputs
*/
Status network_compile(Network* network);
void network_summary(Network* network);


// forward of the network for an input
// NOTE the inputs vector should have the same length as the number of input layers and the same number of values
void network_step(Network* network, NetworkInputs* inputs, uint32_t time);
NetworkOutputs* network_get_outputs(Network* network, NetworkValueType type);

// TODO: maybe is easier to be specific
// TODO: get output by index???
Array* network_get_output_spikes(Network* network);
Array* network_get_output_voltages(Network* network);
Array* network_get_layer_spikes(Network* network, uint32_t i);
Array* network_get_layer_voltages(Network* network, uint32_t i);


#endif // __NETWORK_H__
