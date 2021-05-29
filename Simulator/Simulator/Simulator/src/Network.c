#include "Network.h"
#include "utils/MemoryManagement.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
BOOL network_is_valid(Network* network) {
	check(network != NULL, null_argument("network"));
	check(array_is_valid(&(network->layers)) == TRUE, invalid_argument("network->layers"));
	check(array_is_valid(&(network->input_layers)) == TRUE, invalid_argument("network->input_layers"));
	check(array_is_valid(&(network->output_layers)) == TRUE, invalid_argument("network->output_layers"));
	check(array_is_valid(&(network->input_names)) == TRUE, invalid_argument("network->input_names"));
	check(array_is_valid(&(network->output_names)) == TRUE, invalid_argument("network->output_names"));
	check(array_is_valid(&(network->synapse_classes)) == TRUE, invalid_argument("netwokr->synapse_classes"));
	check(array_is_valid(&(network->neuron_classes)) == TRUE, invalid_argument("network->neuron_classes"));

	uint32_t i = 0;
	uint32_t j = 0;
	Layer* layer = NULL;
	Layer* layer_p = NULL;
	SynapseClass* s_class = NULL;
	NeuronClass* n_class = NULL;
	BOOL ok = FALSE;

	// check each layer
	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	}

	if (network->compiled) {
		for (i = 0; i < network->input_layers.length; ++i) {
			layer = *((Layer**)array_get(&(network->input_layers), i));
			// the address of an input layer should be found in @network->layers
			ok = FALSE;
			for (j = 0; j < network->layers.length; ++j) {
				layer_p = (Layer*)array_get(&(network->layers), j);
				if (layer == layer_p) {
					ok = TRUE;
					break;
				}
			}
			check(ok == TRUE, "Couldn't find input layer with adress %p in @network->layers", layer);
		}

		for (i = 0; i < network->output_layers.length; ++i) {
			layer = *((Layer**)array_get(&(network->output_layers), i));
			// the address of an input layer should be found in @network->layers
			ok = FALSE;
			for (j = 0; j < network->layers.length; ++j) {
				layer_p = (Layer*)array_get(&(network->layers), j);
				if (layer == layer_p) {
					ok = TRUE;
					break;
				}
			}
			check(ok == TRUE, "Couldn't find output layer with adress %p in @network->layers", layer);
		}
	}

	// check each synaptic and neuron class
	for (i = 0; i < network->synapse_classes.length; ++i) {
		s_class = (*(SynapseClass**)array_get(&(network->synapse_classes), i));
		check(synapse_class_is_valid(s_class) == TRUE, invalid_argument("s_class"));
	}
	for (i = 0; i < network->neuron_classes.length; ++i) {
		n_class = (*(NeuronClass**)array_get(&(network->neuron_classes), i));
		check(neuron_class_is_valid(n_class) == TRUE, invalid_argument("n_class"));
	}
		
	return TRUE;

ERROR
	return FALSE;
}


/*************************************************************
* Network Valute Type Functionality
*************************************************************/
const char* network_value_type_C_string(NetworkValueType type) {
	const char* name = NULL;

	if (type == INVALID_NETWORK_VALUE) name = "INVALID_NETWORK_VALUE";
	else if (type == SPIKES) name = "SPIKES";
	else if (type == CURRENT) name = "CURRENT";
	else if (type == VOLTAGE) name = "VOLTAGE";
	else name = "UNKNOWN_NETWORK_VALUE";
	return name;
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
	status = array_init(&(network->input_names), 1, 0, sizeof(String*));
	check(status == SUCCESS, "Couldn't init @network->input_names");
	status = array_init(&(network->output_names), 1, 0, sizeof(String*));
	check(status == SUCCESS, "Couldn't init @network->output_names");
	status = array_init(&(network->synapse_classes), 1, 0, sizeof(SynapseClass*));
	check(status == SUCCESS, "Couldn't init @network->synapse_classes");
	status = array_init(&(network->neuron_classes), 1, 0, sizeof(NeuronClass*));
	check(status == SUCCESS, "Couldn't init @network->neuron_classes");
	network->compiled = FALSE;

	return network;

ERROR
	if (network != NULL) {
		if (array_is_valid(&(network->layers)) == TRUE) array_reset(&(network->layers), NULL);
		if (array_is_valid(&(network->output_layers)) == TRUE) array_reset(&(network->output_layers), NULL);
		if (array_is_valid(&(network->input_layers)) == TRUE) array_reset(&(network->input_layers), NULL);
		if (array_is_valid(&(network->input_names)) == TRUE) array_reset(&(network->input_names), NULL);
		if (array_is_valid(&(network->output_names)) == TRUE) array_reset(&(network->output_names), NULL);
		if (array_is_valid(&(network->synapse_classes)) == TRUE) array_reset(&(network->synapse_classes), NULL);
		if (array_is_valid(&(network->neuron_classes)) == TRUE) array_reset(&(network->neuron_classes), NULL);
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
	array_reset(&(network->input_names), NULL);
	array_reset(&(network->output_names), NULL);
	array_reset(&(network->synapse_classes), synapse_class_ref_destroy);
	array_reset(&(network->neuron_classes), neuron_class_ref_destroy);
	free(network);

ERROR
	return;
}


// layer functionality
Status network_add_layer_keep_valid(Network* network, Layer* layer, BOOL is_input, BOOL is_output) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	Layer* network_layer = NULL;
	Status status = FAIL;
	String* layer_name = NULL;
	layer->is_input = FALSE;

	status = array_append(&(network->layers), layer);
	check(status == SUCCESS, "Could not add layer");

	if (is_input == TRUE) {
		network_layer = (Layer*)array_get(&(network->layers), network->layers.length - 1);
		check(layer_is_valid(network_layer) == TRUE, invalid_argument("network_layer"));
		status = array_append(&(network->input_names), &(network_layer->name));
		check(status == SUCCESS, "Could not add input_layer");
		network_layer->is_input = TRUE;
	}

	if (is_output == TRUE) {
		network_layer = (Layer*)array_get(&(network->layers), network->layers.length - 1);
		check(layer_is_valid(network_layer) == TRUE, invalid_argument("network_layer"));
		status = array_append(&(network->output_names), &(network_layer->name));
		check(status == SUCCESS, "Could not add output_layer");
	}
	return SUCCESS;

ERROR
	return FAIL;
}

Status network_add_layer(Network* network, Layer* layer, BOOL is_input, BOOL is_output) {
	Status status = network_add_layer_keep_valid(network, layer, is_input, is_output);
	if (status == SUCCESS) free(layer);
	return status;
}


Layer* network_get_layer_by_idx(Network* network, uint32_t layer_idx) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(layer_idx < network->layers.length, "@layer_idx is too big");

	Layer* layer = array_get(&(network->layers), layer_idx);
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	return layer;

ERROR
	return NULL;
}

Layer* network_get_layer_by_string(Network* network, String* name) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(string_is_valid(name) == TRUE, invalid_argument("name"));
	uint32_t i = 0;
	Layer* layer = NULL;

	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
		if (string_equal(layer->name, name) == TRUE) {
			return layer;
		}
	}
ERROR
	return NULL;
}


Layer* network_get_layer_by_name(Network* network, char* name) {
	String* string = string_create(name);
	Layer* layer = network_get_layer_by_string(network, string);
	string_destroy(string);
	return layer;
}


uint32_t network_get_layer_idx_by_string(Network* network, String* name) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(string_is_valid(name) == TRUE, invalid_argument("name"));
	uint32_t i = 0;
	Layer* layer = NULL;

	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
		if (string_equal(layer->name, name) == TRUE) {
			return i;
		}
	}

ERROR
	return UINT32_MAX;
}


uint32_t network_get_layer_idx_by_name(Network* network, char* name) {
	String* string = string_create(name);
	uint32_t idx = network_get_layer_idx_by_string(network, string);
	string_destroy(string);
	return idx;
}


// SynapseClass and NeuronClass functionality
Status network_add_synapse_class(Network* network, SynapseClass* s_class) {
	uint32_t i = 0;
	SynapseClass* inner_s_class = NULL;
	Status status = FAIL;

	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(synapse_class_is_valid(s_class) == TRUE, invalid_argument("s_class"));

	for (i = 0; i < network->synapse_classes.length; ++i) {
		inner_s_class = (*(SynapseClass**)array_get(&(network->synapse_classes), i));
		if (string_equal(inner_s_class->name, s_class->name) == TRUE) {
			log_warning("Already a synapse class with name %s", string_get_C_string(s_class->name));
		}
	}
	status = array_append(&(network->synapse_classes), &s_class);
	check(status == SUCCESS, "Couldn't add synapse class %s", string_get_C_string(s_class->name));
	return SUCCESS;
ERROR
	return FAIL;
}


Status network_add_neuron_class(Network* network, NeuronClass* n_class) {
	uint32_t i = 0;
	NeuronClass* inner_n_class = NULL;
	Status status = FAIL;

	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(neuron_class_is_valid(n_class) == TRUE, invalid_argument("n_class"));

	for (i = 0; i < network->neuron_classes.length; ++i) {
		inner_n_class = (*(NeuronClass**)array_get(&(network->neuron_classes), i));
		if (string_equal(inner_n_class->name, n_class->name) == TRUE) {
			log_warning("Already a neuron class with name %s", string_get_C_string(n_class->name));
			return FAIL;
		}
	}
	status = array_append(&(network->neuron_classes), &n_class);
	check(status == SUCCESS, "Couldn't add neuron class %s", string_get_C_string(n_class->name));

	return SUCCESS;
ERROR
	return FAIL;
}


SynapseClass* network_get_synapse_class_by_string(Network* network, String* name) {
	uint32_t i = 0;
	SynapseClass* s_class = NULL;

	check(string_is_valid(name) == TRUE, invalid_argument("name"));
	check(network_is_valid(network) == TRUE, invalid_argument("network"));

	for (i = 0; i < network->synapse_classes.length; ++i) {
		s_class = (*(SynapseClass**)array_get(&(network->synapse_classes), i));
		if (string_equal(s_class->name, name) == TRUE) {
			return s_class;
		}
	}

ERROR
	return NULL;
}


SynapseClass* network_get_synapse_class_by_name(Network* network, const char* s_name) {
	String name;
	string_init(&name, s_name);
	SynapseClass* s_class = network_get_synapse_class_by_string(network, &name);
	string_reset(&name);
	return s_class;
}


NeuronClass* network_get_neuron_class_by_string(Network* network, String* name) {
	uint32_t i = 0;
	NeuronClass* n_class = NULL;

	check(string_is_valid(name) == TRUE, invalid_argument("name"));
	check(network_is_valid(network) == TRUE, invalid_argument("network"));

	for (i = 0; i < network->neuron_classes.length; ++i) {
		n_class = (*(NeuronClass**)array_get(&(network->neuron_classes), i));
		if (string_equal(n_class->name, name) == TRUE) {
			return n_class;
		}
	}

ERROR
	return NULL;
}


NeuronClass* network_get_neuron_class_by_name(Network* network, const char* n_name) {
	String name;
	string_init(&name, n_name);
	NeuronClass* n_class = network_get_neuron_class_by_string(network, &name);
	string_reset(&name);
	return n_class;
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
	LayerInputData* input_data = NULL;
	SynapseClass* s_class = NULL;
	String* layer_name = NULL;
	LayerInputDataLink linking_data = { 0 };

	// check every layer is valid
	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

		// check that the input layers and synaptic classes exist
		for (j = 0; j < layer->inputs_data.length; ++j) {
			input_data = (LayerInputData*)array_get(&(layer->inputs_data), j);
			check(input_data != NULL, null_argument("input_data"));
			input_layer = network_get_layer_by_string(network, &(input_data->layer_name));
			check(layer_is_valid(input_layer) == TRUE, invalid_argument("input_layer"));
			s_class = network_get_synapse_class_by_string(network, &(input_data->syanpse_class_name));
			check(synapse_class_is_valid(s_class) == TRUE, invalid_argument("s_class"));
		}
	}
	// sort layers so every layer is after its inputs -> similar to a topological sort
	// i is incremented only after layer i is in a valid order in the network->layers
	// NOTE: multiple solutions for the same network
	/*
	for (i = 0; i < network->layers.length - 1; i++) {
loop1:
		layer = (Layer*)array_get(&(network->layers), i);

		for (j = 0; j < layer->inputs_data.length; j++) {
			input_data = (LayerInputData*)array_get(&(layer->inputs_data), j);

			for (k = i + 1; k < network->layers.length; k++) {
				layer_k = (Layer*)array_get(&(network->layers), k);

				// check if layer k is input for layer i, where i < k
				if (string_equal(layer_k->name, &(input_data->layer_name)) == TRUE) {
					array_swap(&(network->layers), i, k);
					// start over from the same index layer
					goto loop1; // know is bad practice, but its easy
				}
			}
		}
	}
	*/
	// link the layers
	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

		for (j = 0; j < layer->inputs_data.length; ++j) {
			input_data = (LayerInputData*)array_get(&(layer->inputs_data), j);
			linking_data.input_layer = network_get_layer_by_string(network, &(input_data->layer_name));
			linking_data.s_class = network_get_synapse_class_by_string(network, &(input_data->syanpse_class_name));
			linking_data.connectivity = input_data->connectivity;
			linking_data.synaptic_strength = input_data->synaptic_strength;

			status = layer->link(layer, &linking_data);
			check(status == SUCCESS, "Could not link layers");
		}
	}

	// save references of input layers in @network->input_layers
	for (i = 0; i < network->input_names.length; ++i) {
		layer_name = *((String**)array_get(&(network->input_names), i));
		layer = network_get_layer_by_string(network, layer_name);
		array_append(&(network->input_layers), &layer);
	}

	// save references of output layers in @network->output_layers
	for (i = 0; i < network->output_names.length; ++i) {
		layer_name = *((String**)array_get(&(network->output_names), i));
		layer = network_get_layer_by_string(network, layer_name);
		array_append(&(network->output_layers), &layer);
	}

	network->compiled = TRUE;
	status = SUCCESS;

ERROR
	return status;
}


void network_summary(Network* network) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	uint32_t i = 0;
	String* name = NULL;
	Layer* layer = NULL;
	size_t n_weights = 0;

	printf("-----------------NETWORK SUMMARY----------------\n");
	printf("[LAYERS]\n");
	for (i = 0; i < network->layers.length; i++) {
		layer = (Layer*)array_get(&(network->layers), i);
		printf("------------------------------------------------\n");
		layer_summary(layer);
		n_weights += layer_get_weights_number(layer);
	}
	printf("------------------------------------------------\n");

	printf("Compiled: ");
	if (network->compiled == TRUE) printf("TRUE\n");
	else printf("FALSE\n");

	printf("Number of layers: %u\n", network->layers.length);
	printf("Input layers:");
	for (i = 0; i < network->input_names.length; i++) {
		name = *((String**)array_get(&(network->input_names), i));
		printf(" %s", string_get_C_string(name));
	}
	printf("\n");
	printf("Output layers:");
	for (i = 0; i < network->output_names.length; i++) {
		name = *((String**)array_get(&(network->output_names), i));
		printf(" %s", string_get_C_string(name));
	}
	printf("\n");
	printf("Number of parameters: %llu\n", n_weights);
	printf("------------------------------------------------\n");

ERROR
	return;
}


// TODO: what if the inputs can be NULL, just to see the evolution
void network_step(Network* network, NetworkInputs* inputs, uint32_t time) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
	check(inputs->length == network->input_layers.length, "@inputs->length %u should equal @network->input_layers.length %u", inputs->length, network->input_layers.length);
	check(network->compiled == TRUE, invalid_argument("network->compiled"));

	uint32_t i = 0;
	Layer* layer = NULL;
	NetworkValues* input = NULL;

	// run on input layers
	for (i = 0; i < inputs->length; ++i) {
		input = (NetworkValues*)array_get(inputs, i);
		check(array_is_valid(&(input->values)) == TRUE, invalid_argument("input->values"));

		layer = *((Layer**)array_get(&(network->input_layers), i));
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

		check(input->values.length == layer->neurons.length, "for input %u - input lenght %u while layer length %u", i, input->values.length, layer->neurons.length);
		if (input->type == SPIKES) {
			layer_step_force_spikes(layer, &(input->values), time);
		}
		else if (input->type == CURRENT) {
			layer_step_inject_currents(layer, &(input->values), time);
		}
		else {
			log_error("Undefined NETWORK input type %d", input->type);
		}
	}

	// run on the rest of layers
	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
		if (layer->is_input == FALSE) {
			layer_step(layer, time);
		}
	}

ERROR
	return;
}


NetworkOutputs* network_get_outputs(Network* network, NetworkValueType type) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	check(type == SPIKES || type == VOLTAGE, invalid_argument("type"));

	NetworkOutputs* outputs = array_create(network->output_layers.length, network->output_layers.length, sizeof(NetworkValues));
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
		memcpy(&(net_values.values), values, sizeof(Array));
		free(values);
		array_set(outputs, i, &net_values);
	}
	return outputs;

ERROR
	return NULL;
}


void network_clear_state(Network* network) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	uint32_t i = 0;
	Layer* layer = NULL;

	for (i = 0; i < network->layers.length; ++i) {
		layer = (Layer*)array_get(&(network->layers), i);
		layer_clear_state(layer);
	}

ERROR
	return;
}


Array* network_get_output_spikes(Network* network) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	Array* outputs = array_create(network->output_layers.length, 0, sizeof(ArrayBool));
	check(array_is_valid(outputs) == TRUE, invalid_argument("outputs"));

	uint32_t i = 0;
	Layer* output_layer = NULL;
	Array* spikes = NULL;

	for (i = 0; i < network->output_layers.length; ++i) {
		output_layer = *((Layer**)array_get(&(network->output_layers), i));
		check(layer_is_valid(output_layer) == TRUE, invalid_argument("output_layer"));

		spikes = layer_get_spikes(output_layer);
		array_append(outputs, spikes);
		free(spikes);
	}
	return outputs;
ERROR
	return NULL;
}

Array* network_get_output_voltages(Network* network) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	Array* outputs = array_create(network->output_layers.length, 0, sizeof(Array));
	check(array_is_valid(outputs) == TRUE, invalid_argument("outputs"));

	uint32_t i = 0;
	Layer* output_layer = NULL;
	Array* voltages = NULL;

	for (i = 0; i < network->output_layers.length; ++i) {
		output_layer = *((Layer**)array_get(&(network->output_layers), i));
		check(layer_is_valid(output_layer) == TRUE, invalid_argument("output_layer"));

		voltages = layer_get_voltages(output_layer);
		array_append(outputs, voltages);
	}
	return outputs;
ERROR
	return NULL;
}

Array* network_get_layer_spikes(Network* network, uint32_t i) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	Layer* layer = network_get_layer_by_idx(network, i);
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	return layer_get_spikes(layer);
ERROR
	return NULL;
}

Array* network_get_layer_voltages(Network* network, uint32_t i) {
	check(network_is_valid(network) == TRUE, invalid_argument("network"));
	Layer* layer = network_get_layer_by_idx(network, i);
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	return layer_get_voltages(layer);
ERROR
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
			array_show(&(net_values->values), show_bool);
		}
		else if (net_values->type == VOLTAGE) {
			array_show(&(net_values->values), show_float);
		}
	}
}


void network_values_destroy(Array* values) {
	check(array_is_valid(values) == TRUE, invalid_argument("values"));
	uint32_t i = 0;
	NetworkValues* net_vals = NULL;

	for (i = 0; i < values->length; ++i) {
		net_vals = (NetworkValues*)array_get(values, i);
		net_vals->type = INVALID_NETWORK_VALUE;
		array_reset(&(net_vals->values), NULL);
	}
	array_destroy(values, NULL);

ERROR
	return;
}
