#ifndef __NETWORK_H__
#define __NETWORK_H__


#include "config.h"
#include "Layer.h"
#include "Containers.h"


// TO DO: need a structure for input/output spikes, input Current, output voltages per neuron???
typedef float Spike;

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


void network_values_show(Array* values);

typedef struct Network {
	/*
	A layer in @layers can simultaneously be an input oand an output layer (usefull for debuging)
	*/
	Vector* layers; // will be added one by one
	Vector* output_layers; // one or more, keeps references to @layers
	Vector* input_layers; // one or more, keeps references to @layers
	Status compiled; // if the network has been compiled TODO: integrate this
} Network;


/* 
Network is valid if:
1. @network != NULL
2. @network->layers is valid
3. @network->output_layers is valid
4. @network->input_layers is valid
*/
Status network_is_valid(Network* network);

// this will only allocate the layers
Network* network_create();
void network_destroy(Network* network);

Layer* network_get_layer_by_idx(Network* network, uint32_t layer_idx);
Layer* network_get_layer_by_name(Network* network, Array* name);
uint32_t network_get_layer_idx_by_name(Network* network, Array* name);

/*
This will copy the content of the @layer structure (shallow copy).
The user need to decide if the memory location that holds the @layer structure is still nedeed

Setting @should_free:
should_free -> does a free(layer)
1. If the layer was on the stack then set @should_free = FALSE
2. If the layer was on the heap and the @layer structure is still needed then set @should_free = FALSE
3. If the layer was on the heap and the @layer structure is not needed anymore set @should_free = TRUE
*/
Status network_add_layer(Network* netowrk, Layer* layer, Status should_free, Status is_input, Status is_output);

/*
Check that all the layers are valid and if there are missing layers
Then it will connect every layer with its inputs
*/
Status network_compile(Network* network);

// forward of the network for an input
// NOTE the inputs vector should have the same length as the number of input layers and the same number of values
void network_step(Network* network, Vector* inputs, uint32_t time);

Array* network_get_outputs(Network* network, NetworkValueType type);


// Need to get the output of the network and of individual layers somehow, AGAIN not sure if that is the spikes?, the voltages?
// Spike* network_get_output(Network* network);
// Spike* network_get_layer_output(Network* network, uint32_t layer_idx);

// Need to set somehow the input to the network
// void network_set_input(Network* networ, Spike* spikes);


#endif // __NETWORK_H__
