#include "Network.h"

#include <stdlib.h>
#include <stdbool.h>


Network* network_create(uint32_t n_layers) {
	Network* network = (Network*)malloc(sizeof(Network));
	check_memory(network);

	network->layers = calloc(n_layers, sizeof(Layer));
	check_memory(network->layers);
	network->n_layers = n_layers;

	return network;

error:
	// @network->layers == NULL
	if (network != NULL) {
		free(network);
	}
	return NULL;
}


void network_destroy(Network* network) {
	check(network != NULL, "NULL value for @network");
	check(network->layers != NULL, "NULL value for @network->layers");
	uint32_t i = 0;

	for (i = 0; i < network->n_layers; ++i) {
		layer_reset((network->layers) + i);
	}

	free(network->layers);
	free(network);

error:
	return;
}


Status network_is_valid(Network* network) {
	check(network != NULL, "NULL value for @network");
	check(network->layers != NULL, "NULL value for @network->layers");
	check(network->n_layers != 0, "@network->n_layers is 0");

	return TRUE;

error:
	return FALSE;
}


// init a layer on stack and init it and then copy it here
Status network_overwrite_layer(Network* network, Layer* layer, uint32_t layer_idx) {
	Status status = FAIL;
	check(network_is_valid(network), "Invalid @network");
	check(layer_is_valid(layer), "Invalid @layer");
	check(layer_idx < network->n_layers, "@layer_idx is too big");

	status = SUCCESS;

	memcpy(network->layers + layer_idx, layer, sizeof(Layer));

	// TO DO: I may need to free(layer), not sure, depends on how i create the network

error:
	return status;
}

Layer* network_get_layer(Network* network, uint32_t layer_idx) {
	check(network_is_valid(network), "Invalid @network");
	check(layer_idx < network->n_layers, "@layer_idx is too big");

	return network->layers + layer_idx;

error:
	return NULL;
}


Status network_is_ready(Network* network) {
	check(network_is_valid(network), "Invalid @network");
	uint32_t i = 0;


	for (i = 0; i < network->n_layers; ++i) {
		check(layer_is_valid(network->layers + i), "Layer %d is not valid", i);
	}

	return TRUE;

error:
	return FALSE;
}


Status network_compile(Network* network) {
	check(network_is_ready(network) != FALSE, "@network is not ready");
	uint32_t i = 0;

	// currently only sequential networks
	for (i = 0; i < network->n_layers; ++i) {

	}

error:
}


Spike* network_step(Network* network, Spike* spikes, uint32_t time) {

	network_set_input(network, spikes);

	uint32_t i = 0;
	for (i = 0; i < network->n_layers; ++i) {
		layer_step(network->layers + i, time);
	}

	Spike* out_spikes = network_get_output(network);

	return out_spikes;
}
