#include "Network.h"
#include "MemoryManagement.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
bool network_is_valid(Network* network) {
	check(network != NULL, null_argument("network"));
	check(array_is_valid(&(network->layers)) == TRUE, invalid_argument("network->layers"));
	check(array_is_valid(&(network->input_layers)) == TRUE, invalid_argument("network->input_layers"));
	check(array_is_valid(&(network->output_layers)) == TRUE, invalid_argument("network->output_layers"));

	return TRUE;

error:
	return FALSE;
}


/*************************************************************
* NETWORK FUNCTIONALITY
*************************************************************/
Network* network_create() {
	Status status = FAIL;
	Network* network = (Network*)calloc(1, sizeof(Network), "network_create");
	check_memory(network);

	status = array_init(&(network->layers), 1, 0, sizeof(Layer));
	check(status == SUCCESS, "Couldn't init @network->layers");
	status = array_init(&(network->input_layers), 1, 0, sizeof(Layer*));
	check(status == SUCCESS, "Couldn't init @network->input_layers");
	status = array_init(&(network->output_layers), 1, 0, sizeof(Layer*));
	check(status == SUCCESS, "Couldn't init @network->output_layers");
	network->compiled = FALSE;

	return network;

error:
	if (network != NULL) {
		if (network->layers.data != NULL) array_reset(&(network->layers), NULL);
		if (network->output_layers.data != NULL) array_destroy(&(network->output_layers), NULL);
		if (network->input_layers.data != NULL) array_destroy(&(network->input_layers), NULL);
		free(network);
	}
	return NULL;
}


void network_destroy(Network* network) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	
	uint32_t i = 0;
	Layer* layer = NULL;

	array_reset(&(network->layers), layer_reset);
	array_reset(&(network->input_layers), NULL);
	array_reset(&(network->output_layers), NULL);
	free(network);

error:
	return;
}


Status network_add_layer(Network* network, Layer* layer, bool should_free, bool is_input, bool is_output) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	Layer* network_layer = NULL;
	Status status = FAIL;
	status = array_append(&(network->layers), layer);
	check(status == SUCCESS, "Could not add layer");
	// may need to remove the memory that contain the info about the layer, which we copied
	if (should_free == TRUE) free(layer);

	layer->is_input = FALSE;

	if (is_input == TRUE) {
		network_layer = (Layer*)array_get(&(network->layers), network->layers.length - 1);
		check(layer_is_valid(network_layer) == TRUE, invalid_argument("network_layer"));
		// save reference to the current layer
		status = array_append(&(network->input_layers), &network_layer);
		check(status == SUCCESS, "Could not add input_layer");
		network_layer->is_input = TRUE;
	}

	if (is_output == TRUE) {
		network_layer = (Layer*)array_get(&(network->layers), network->layers.length - 1);
		check(layer_is_valid(network_layer) == TRUE, invalid_argument("network_layer"));
		// save reference to the current layer
		status = array_append(&(network->output_layers), &network_layer);
		check(status == SUCCESS, "Could not add output_layer");
	}

	return SUCCESS;

error:
	return FAIL;
}


Layer* network_get_layer_by_idx(Network* network, uint32_t layer_idx) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(layer_idx < network->layers.length, "@layer_idx is too big");

	Layer* layer = array_get(&(network->layers), layer_idx);
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	return layer;

error:
	return NULL;
}


Layer* network_get_layer_by_name(Network* network, String* name) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(string_is_valid(name) == TRUE, invalid_argument("name"));
	uint32_t i = 0;
	Layer* layer = NULL;

	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
		if (string_compare(layer->name, name) == 0) {
			return layer;
		}
	}

error:
	return NULL;
}


uint32_t network_get_layer_idx_by_name(Network* network, String* name) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(string_is_valid(name) == TRUE, invalid_argument("name"));
	uint32_t i = 0;
	Layer* layer = NULL;

	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
		if (string_compare(layer->name, name) == 0) {
			return i;
		}
	}

error:
	return UINT32_MAX;
}


Status network_compile(Network* network) {
	check(network_is_valid(network), invalid_argument("network"));

	// do nothing if already compiled
	if (network->compiled == TRUE) {
		return SUCCESS;
	}

	Status status = FAIL;
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	Layer* layer = NULL;
	Layer* layer_k = NULL;
	Layer* input_layer = NULL;
	String* layer_name = NULL;
	String* input_name = NULL;
	Array* inputs_layers_names = NULL;

	// check every layer is valid
	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

		// check that the input layers exist
		for (j = 0; j < layer->input_names->length; ++j) {
			layer_name = *((String**)array_get(layer->input_names, j));
			check(string_is_valid(layer_name) == TRUE, invalid_argument("layer_name"));
			input_layer = network_get_layer_by_name(network, layer_name);
			check(layer_is_valid(input_layer) == TRUE, invalid_argument("input_layer"));
		}
	}

	// sort layers so every layer is after its inputs -> similar to a topological sort
	// i is incremented only after layer i is in a valid order in the network->layers
	// NOTE: multiple solutions for the same network
	for (i = 0; i < network->layers.length - 1; i++) {
loop1:
		layer = (Layer*)array_get(&(network->layers), i);

		for (j = 0; j < layer->input_names->length; j++) {
			input_name = *((String**)array_get(layer->input_names, j));

			for (k = i + 1; k < network->layers.length; k++) {
				layer_k = (Layer*)array_get(&(network->layers), k);

				// check if layer k is input for layer i, where i < k
				if (string_compare(layer_k->name, input_name) == 0) {
					array_swap(&(network->layers), i, k);
					// start over from the same index layer
					goto loop1; // know is bad practice, but its easy
				}
			}
		}
	}

	// link the layers
	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

		for (j = 0; j < layer->input_names->length; ++j) {
			layer_name = *((String**)array_get(layer->input_names, j));
			input_layer = network_get_layer_by_name(network, layer_name);
			status = layer->link(layer, input_layer);
			check(status == SUCCESS, "Could not link layers");
		}
		
	}
	network->compiled = TRUE;
	status = SUCCESS;

error:
	return status;
}



// TODO: what if the inputs can be NULL, just to see the evolution
void network_step(Network* network, NetworkInputs* inputs, uint32_t time) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
	check(inputs->length == network->input_layers.length, "@inputs->length %u should equal @network->input_layers.lenght %u", inputs->length, network->input_layers.length);
	check(network->compiled == TRUE, invalid_argument("network->compiled"));

	uint32_t i = 0;
	Layer* layer = NULL;
	NetworkValues* input = NULL;

	for (i = 0; i < inputs->length; ++i) {
		input = (NetworkValues*)array_get(inputs, i);
		check(array_is_valid(input->values) == TRUE, invalid_argument("input->values"));

		layer = *((Layer**)array_get(&(network->input_layers), i));
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

		check(input->values->length == layer->neurons.length, "for input %u - input lenght %u while layer length %u", i, input->values->length, layer->neurons.length);
		if (input->type == SPIKES) {
			layer_force_spikes(layer, input->values, time);
		}
		else if (input->type == CURRENT) {
			layer_inject_currents(layer, input->values, time);
		}
		else {
			log_error("Undefined NETWORK input type %d", input->type);
		}
	}

	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
		if (layer->is_input == FALSE) {
			layer_step(layer, time);
		}
	}

error:
	return;
}


NetworkOutputs* network_get_outputs(Network* network, NetworkValueType type) {
	NetworkOutputs* outputs = array_create(network->output_layers.length, 0, sizeof(NetworkValues));
	check(array_is_valid(outputs) == TRUE, invalid_argument("outputs"));

	uint32_t i = 0;
	Layer* output_layer = NULL;
	Array* values = NULL;
	NetworkValues net_values;

	for (i = 0; i < network->output_layers.length; ++i) {
		output_layer = *((Layer**)array_get(&(network->output_layers), i));
		check(layer_is_valid(output_layer) == TRUE, invalid_argument("output_layer"));

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
			// to do
			array_show(net_values->values, NULL);
		}
		else if (net_values->type == VOLTAGE) {
			array_show(net_values->values, show_float);
		}
	}
}