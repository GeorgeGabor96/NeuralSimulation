#include "Network.h"

#include <stdlib.h>
#include <stdbool.h>


Network* network_create() {
	Network* network = (Network*)malloc(sizeof(Network));
	check_memory(network);

	// Consider ca o retea are cel putin un layer si se mai pot adauga
	network->layers = vector_create(1, sizeof(Layer));
	check_memory(network->layers);

	return network;

error:
	// @network->layers == NULL
	if (network != NULL) {
		free(network);
	}
	return NULL;
}


void network_destroy(Network* network) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	
	uint32_t i = 0;
	Layer* layer = NULL;
	for (i = 0; i < network->layers->length; ++i) {
		layer = (Layer*)vector_get(network->layers, i);
		check(layer != NULL, null_argument("layer"));
		layer_reset(layer);
	}

	vector_destroy(network->layers);
	free(network);

error:
	return;
}


Status network_is_valid(Network* network) {
	check(network != NULL, "NULL value for @network");
	check(network->layers != NULL, "NULL value for @network->layers");

	return TRUE;

error:
	return FALSE;
}


Layer* network_get_layer_by_idx(Network* network, uint32_t layer_idx) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(layer_idx < network->layers->length, "@layer_idx is too big");

	Layer* layer = vector_get(network->layers, layer_idx);
	check(layer != NULL, null_argument("layer"));
	return layer;

error:
	return NULL;
}


Status network_is_ready(Network* network) {
	check(network_is_valid(network), invalid_argument("network"));
	uint32_t i = 0;


	for (i = 0; i < network->layers->length; ++i) {
		check(layer_is_valid(vector_get(network->layers, i)) == TRUE, "Layer %d is not valid", i);
	}

	return TRUE;

error:
	return FALSE;
}


Status network_compile(Network* network) {
	check(network_is_ready(network) != FALSE, "@network is not ready");
	uint32_t i = 0;
	Layer* layer = NULL;
	Layer* input_layer = NULL;

	// currently only sequential networks
	// TODO: use name or something to make it more interesting --> multiple input layers
	for (i = 1; i < network->layers->length; ++i) {
		// TODO: poate trebuie o ordonare a layerelor pentru a se execute intr-o ordine valida (ex: TOPO sort)
		layer = (Layer*)vector_get(network->layers, i);
		input_layer = (Layer*)vector_get(network->layers, i - 1);

		// link layers
		check(layer->link(layer, input_layer) == SUCCESS, "Could not link layers");
	}

	return SUCCESS;

error:
	return FAIL;
}


Status network_add_layer(Network* network, Layer* layer) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	check(vector_append(network->layers, layer) == SUCCESS, "Could not add layer");
	free(layer); // need to free this because all info is copied in @network->layers

	return SUCCESS;

error:
	return FAIL;
}



void network_step(Network* network, uint32_t time) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));

	// TODO: unde pun input??? -> SImulators job?

	uint32_t i = 0;
	Layer* layer = NULL;
	for (i = 0; i < network->layers->length; ++i) {
		layer = (Layer*)vector_get(network->layers, i);
		check(layer != NULL, null_argument("layer"));
		
		layer_step(layer, time);
	}

error:
	return;
}
