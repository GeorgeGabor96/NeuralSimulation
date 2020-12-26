#ifndef __NETWORK_H__
#define __NETWORK_H__


#include "debug.h"
#include "Layer.h"
#include "Containers.h"

#include <stdint.h>

// Config Files:- este mai greu de lucrat cu ele, imi trebuie parsare, definirea unui format, etc
// Keras Approach - add layers one by one, input/output by name

// TO DO: need a structure for input/output spikes, input Current, output voltages per neuron???
typedef float Spike;

// TO DO: so if we want non linear networks, need a way to reference a layer by name/index

typedef struct Network {
	/*
	A layer in @layers can simultaneously be an input oand an output layer (usefull for debuging)
	*/
	Vector* layers; // will be added one by one
	Vector* output_layers; // one or more, keeps references to @layers
	Vector* input_layers; // one or more, keeps references to @layers
	Status compiled; // if the network has been compiled TODO: integrate this
} Network;

// this will only allocate the layers
Network* network_create();
void network_destroy(Network* network);

// this will ensure that the network structure is valid = all layers have a valid type
Status network_is_valid(Network* network);

// if every layer is valid
Status network_is_ready(Network* network);

// this will connect the synapses between layers
Status network_compile(Network* network);

Layer* network_get_layer_by_idx(Network* network, uint32_t layer_idx);

// this will copy the content of the @layer into the network (shallow copy), and the network will use that copied information
// user needs to manage the memory that contains the @layer information
// NOTE: Recomend to alocate the layer on the stack, and pass that here to avoid wierd 'free' calls
// TODO: free_layer?? parameter?
Status network_add_layer(Network* netowrk, Layer* layer, Status is_input, Status is_output);

// forward of the network for an input
// NOTE the inputs vector should have the same length as the number of input layers and the same number of values
void network_step(Network* network, Vector* inputs, uint32_t time);


// Need to get the output of the network and of individual layers somehow, AGAIN not sure if that is the spikes?, the voltages?
// Spike* network_get_output(Network* network);
// Spike* network_get_layer_output(Network* network, uint32_t layer_idx);

// Need to set somehow the input to the network
// void network_set_input(Network* networ, Spike* spikes);


#endif // __NETWORK_H__
