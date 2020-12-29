#include "Network.h"

#include <stdlib.h>
#include <stdbool.h>


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status network_is_valid(Network* network) {
	check(network != NULL, null_argument("network"));
	check(vector_is_valid(network->layers) == TRUE, invalid_argument("network->layers"));
	check(vector_is_valid(network->input_layers) == TRUE, invalid_argument("network->input_layers"));
	check(vector_is_valid(network->output_layers) == TRUE, invalid_argument("network->output_layers"));

	return TRUE;

error:
	return FALSE;
}


/*************************************************************
* NETWORK FUNCTIONALITY
*************************************************************/
Network* network_create() {
	Network* network = (Network*)calloc(1, sizeof(Network));
	check_memory(network);

	network->layers = vector_create(1, sizeof(Layer));
	check_memory(network->layers);
	network->input_layers = vector_create(1, sizeof(Layer*));
	check_memory(network->input_layers);
	network->output_layers = vector_create(1, sizeof(Layer*));
	check_memory(network->output_layers);
	network->compiled = FALSE;

	return network;

error:
	if (network != NULL) {
		if (network->layers != NULL) vector_destroy(network->layers, layer_reset);
		if (network->output_layers != NULL) vector_destroy(network->output_layers, NULL);
		if (network->input_layers != NULL) vector_destroy(network->input_layers, NULL);
		free(network);
	}
	return NULL;
}


void network_destroy(Network* network) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	
	uint32_t i = 0;
	Layer* layer = NULL;

	vector_destroy(network->layers, layer_reset);
	vector_destroy(network->input_layers, NULL);
	vector_destroy(network->output_layers, NULL);
	free(network);

error:
	return;
}


Status network_add_layer(Network* network, Layer* layer, Status should_free, Status is_input, Status is_output) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	Status status = FAIL;
	status = vector_append(network->layers, layer);
	check(status == SUCCESS, "Could not add layer");
	// may need to remove the memory that contain the info about the layer, which we copied
	if (should_free == TRUE) free(layer);

	if (is_input == TRUE) {
		Layer* network_layer = (Layer*)vector_get(network->layers, network->layers->length - 1);
		check(layer_is_valid(network_layer) == TRUE, invalid_argument("network_layer"));
		// save reference to the current layer
		status = vector_append(network->input_layers, &network_layer);
		check(status == SUCCESS, "Could not add input_layer");
	}

	if (is_output == TRUE) {
		Layer* network_layer = (Layer*)vector_get(network->layers, network->layers->length - 1);
		check(layer_is_valid(network_layer) == TRUE, invalid_argument("network_layer"));
		// save reference to the current layer
		status = vector_append(network->output_layers, &network_layer);
		check(status == SUCCESS, "Could not add output_layer");
	}

	return SUCCESS;

error:
	return FAIL;
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


Layer* network_get_layer_by_name(Network* network, Array* name) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(array_is_valid(name) == TRUE, invalid_argument("name"));
	uint32_t i = 0;
	Layer* layer = NULL;

	for (i = 0; i < network->layers->length; ++i) {
		layer = (Layer*)vector_get(network->layers, i);
		if (string_compare(layer->name, name) == 0) {
			return layer;
		}
	}

error:
	return NULL;
}


uint32_t network_get_layer_idx_by_name(Network* network, Array* name) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(array_is_valid(name) == TRUE, invalid_argument("name"));
	uint32_t i = 0;
	Layer* layer = NULL;

	for (i = 0; i < network->layers->length; ++i) {
		layer = (Layer*)vector_get(network->layers, i);
		if (string_compare(layer->name, name) == 0) {
			return i;
		}
	}

error:
	return network->layers->length;
}


Status network_compile(Network* network) {
	check(network_is_valid(network), invalid_argument("network"));

	if (network->compiled == TRUE) {
		return SUCCESS;
	}

	uint32_t i = 0;
	uint32_t j = 0;
	Layer* layer = NULL;
	Layer* input_layer = NULL;
	Array* layer_name = NULL;
	Vector* inputs_layers_names = NULL;

	// check every layer is valid
	for (i = 0; i < network->layers->length; ++i) {
		layer = (Layer*)vector_get(network->layers, i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

		// check that the input layers exist
		for (j = 0; j < layer->input_names->length; ++j) {
			layer_name = *((Array**)vector_get(layer->input_names, j));
			input_layer = network_get_layer_by_name(network, layer_name);
			check(input_layer != NULL, null_argument("input_layer"));
		}
	}

	// sort layers so every layer is after its inputs
	// TODO

	// link the layers
	for (i = 0; i < network->layers->length; ++i) {
		layer = (Layer*)vector_get(network->layers, i);
		
		for (j = 0; j < layer->input_names->length; ++j) {
			layer_name = *((Array**)vector_get(layer->input_names, j));
			input_layer = network_get_layer_by_name(network, layer_name);
			check(layer->link(layer, input_layer) == SUCCESS, "Could not link layers");
		}
		
	}

	network->compiled = TRUE;

	return SUCCESS;

error:
	network->compiled = FALSE;

	return FAIL;
}





// TODO: what if the inputs can be NULL, just to see the evolution
void network_step(Network* network, Vector* inputs, uint32_t time) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(vector_is_valid(inputs) == TRUE, invalid_argument("inputs"));
	check(inputs->length == network->input_layers->length, "@inputs->length %d should equal @network->input_layers->lenght %d", inputs->length, network->input_layers->length);

	uint32_t i = 0;
	Layer* layer = NULL;
	NetworkValues* input = NULL;

	for (i = 0; i < inputs->length; ++i) {
		input = (NetworkValues*)vector_get(inputs, i);
		layer = *(Layer**)vector_get(network->input_layers, i);
		printf("%d\n", input->values->length);
		printf("%d\n", layer->neurons->length);
		check(input->values->length == layer->neurons->length, "for input %i - input lenght %d while layer length %d", i, input->values->length, layer->neurons->length);

		if (input->type == SPIKES) {
			layer_set_spikes(layer, input->values, time);
		}
		else if (input->type == CURRENT) {
			layer_set_currents(layer, input->values, time);
		}
		else {
			log_error("Undefined NETWORK input type %d", input->type);
		}
	}
	return;
	for (i = 0; i < network->layers->length; ++i) {
		printf("Before\n");

		layer = (Layer*)vector_get(network->layers, i);
		check(layer != NULL, null_argument("layer"));
		
		layer_step(layer, time);
		printf("After\n");
	}

error:
	return;
}


Array* network_get_outputs(Network* network, NetworkValueType type) {
	Array* outputs = array_create(network->output_layers->length, sizeof(NetworkValues));
	check_memory(outputs);
	uint32_t i = 0;
	Layer* output_layer = NULL;
	Array* values = NULL;
	NetworkValues net_values;

	for (i = 0; i < network->output_layers->length; ++i) {
		output_layer = *((Layer**)vector_get(network->output_layers, i));
		if (type == SPIKES) {
			values = layer_get_spikes(output_layer);
		}
		else if (type == VOLTAGE) {
			values = layer_get_voltages(output_layer);
		}
		else {
			log_error(invalid_argument("type"));
		}
		net_values.type = type;
		net_values.values = values;
		array_set(outputs, i, &net_values);
	}

	return outputs;

error:
	return NULL;
}


void network_values_show(Array* values) {
	uint32_t i = 0;
	NetworkValues* net_values = NULL;

	log_info("Showing values");

	for (i = 0; i < values->length; ++i) {
		net_values = (NetworkValues*)array_get(values, i);
		printf("\n[%d]-Type: %d\n", i, net_values->type);
		if (net_values->type == SPIKES) {
			array_show(net_values->values, NULL);
		}
		else if (net_values->type == VOLTAGE) {
			array_show(net_values->values, show_float);
		}
	}
}