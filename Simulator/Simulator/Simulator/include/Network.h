#ifndef __NETWORK_H__
#define __NETWORK_H__


#include "debug.h"
#include "Layer.h"

#include <stdint.h>

typedef float* InputSpikes;

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
// this will connect he synapses between layers
Status network_compile(Network* network);
Layer* network_get_layer(Network* network, uint32_t layer_idx)
void network_step(Network* network, InputSpikes spikes);

#endif // __NETWORK_H__
