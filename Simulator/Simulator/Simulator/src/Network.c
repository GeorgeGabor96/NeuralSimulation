#include "Network.h"

#include <stdlib.h>
#include <stdbool.h>


Network* network_create() {
	Network* network = (Network*)calloc(1, sizeof(Network));
	check_memory(network);

	// Consider ca o retea are cel putin un layer si se mai pot adauga
	network->layers = vector_create(1, sizeof(Layer));
	check_memory(network->layers);
	network->input_layers = vector_create(1, sizeof(Layer*));
	check_memory(network->input_layers);
	network->output_layers = vector_create(1, sizeof(Layer*));
	check_memory(network->output_layers);

	return network;

error:
	if (network != NULL) {
		if (network->output_layers != NULL) vector_destroy(network->output_layers, NULL);
		if (network->input_layers != NULL) vector_destroy(network->input_layers, NULL);
		if (network->layers != NULL) vector_destroy(network->layers, layer_reset); 
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

	vector_destroy(network->layers, layer_reset);
	vector_destroy(network->input_layers, NULL);
	vector_destroy(network->output_layers, NULL);
	free(network);

error:
	return;
}


Status network_is_valid(Network* network) {
	check(network != NULL, null_argument("network"));
	check(network->layers != NULL, null_argument("network->layers"));
	check(network->input_layers != NULL, null_argument("network->input_layers"));
	check(network->output_layers != NULL, null_argument("network->output_layers"));

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
	check(network_is_ready(network) != FALSE, "Cannot compile the @network because is not ready");
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


Status network_add_layer(Network* network, Layer* layer, Status is_input, Status is_output) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	Status status = FAIL;
	status = vector_append(network->layers, layer);
	check(status == SUCCESS, "Could not add layer");
	
	if (is_input == TRUE) {
		Layer* network_layer = (Layer*)vector_get(network->layers, network->layers->length - 1);
		check(network_layer != NULL, null_argument("network_layer"));
		// save reference to the current layer
		status = vector_append(network->input_layers, &network_layer);
		check(status == SUCCESS, "Could not add input_layer");
	}

	if (is_output == TRUE) {
		Layer* network_layer = (Layer*)vector_get(network->layers, network->layers->length - 1);
		check(network_layer != NULL, null_argument("network_layer"));
		// save reference to the current layer
		status = vector_append(network->output_layers, &network_layer);
		check(status == SUCCESS, "Could not add output_layer");
	}

	return SUCCESS;

error:
	return FAIL;
}


// TODO: what if the inputs can be NULL, just to see the evolution
void network_step(Network* network, Vector* inputs, uint32_t time) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(vector_is_valid(inputs) == TRUE, invalid_argument("inputs"));
	check(inputs->length == network->input_layers->length, "@inputs->length %d should equal @network->input_layers->lenght %d", inputs->length, network->input_layers->length);

	uint32_t i = 0;
	Array* input = NULL;
	Layer* layer = NULL;

	// set inputs
	for (i = 0; i < inputs->length; ++i) {
		input = (Array*)vector_get(inputs, i);
		check(input != NULL, null_argument("input"));

		layer = *(Layer**)vector_get(network->input_layers, i);
		check(layer != NULL, null_argument("layer"));

		check(input->length == layer->neurons->length, "for input %i - input lenght %d while layer length %d", i, input->length, layer->neurons->length);
		// how to set the input if it can be spikes or current?????
		/// probably some structure
	}

	for (i = 0; i < network->layers->length; ++i) {
		layer = (Layer*)vector_get(network->layers, i);
		check(layer != NULL, null_argument("layer"));
		
		layer_step(layer, time);
	}

error:
	return;
}
