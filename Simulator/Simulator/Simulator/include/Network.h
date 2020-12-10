#ifndef __NETWORK_H__
#define __NETWORK_H__


#include "debug.h"
#include "Layer.h"

#include <stdint.h>

// Config Files:- este mai greu de lucrat cu ele, imi trebuie parsare, definirea unui format, etc
// Keras Approach - add layers one by one, input/output by name

// TO DO: need a structure for input/output spikes, input Current, output voltages per neuron???
typedef float Spike;

// TO DO: so if we want non linear networks, need a way to reference a layer by name/index

typedef struct Network {
	Layer* layers; // will be allocated with one alloc
	uint32_t n_layers;
	Layer input;
} Network;

// this will only allocate the layers
Network* network_create(uint32_t n_layers);
void network_destroy(Network* network);

// this should be called for every layer
Status network_overwrite_layer(Network* network, Layer* layer, uint32_t layer_idx);

// this will ensure that the network structure is valid = all layers have a valid type
Status network_is_valid(Network* network);
Status network_is_ready(Network* network);

// this will connect the synapses between layers
Status network_compile(Network* network);

Layer* network_get_layer(Network* network, uint32_t layer_idx);

// forward of the network for an input
Spike* network_step(Network* network, Spike* spikes, uint32_t time);


// Need to get the output of the network and of individual layers somehow, AGAIN not sure if that is the spikes?, the voltages?
Spike* network_get_output(Network* network);
Spike* network_get_layer_output(Network* network, uint32_t layer_idx);

// Need to set somehow the input to the network
void network_set_input(Network* networ, Spike* spikes);


#endif // __NETWORK_H__
