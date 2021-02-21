#include "Layer.h"
#include "MemoryManagement.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
BOOL layer_is_valid(Layer* layer) {
	check(layer != NULL, null_argument("layer"));
	check(layer_type_is_valid(layer->type) == TRUE, invalid_argument("layer->type"));
	check(neuron_class_is_valid(layer->neuron_class) == TRUE, invalid_argument("layer->neuron_class"));
	check(synapse_class_is_valid(layer->synapse_class) == TRUE, invalid_argument("layer->synapse_class"));
	check(layer->link != NULL, "NULL value for @layer->link");
	check(string_is_valid(layer->name) == TRUE, invalid_argument("layer->name"));
	check(array_is_valid(layer->input_names) == TRUE, invalid_argument("layer->input_names"));
	check(array_is_valid(&(layer->neurons)) == TRUE, invalid_argument("layer->neurons"));

	// check the neurons
	for (uint32_t i = 0; i < layer->neurons.length; ++i) {
		Neuron* neuron = (Neuron*)array_get(&(layer->neurons), i);
		check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
	}

	return TRUE;

ERROR
	return FALSE;
}


/*************************************************************
* LAYER TYPE FUNCTIONALITY
*************************************************************/
const char* layer_type_C_string(LayerType type) {
	const char* name = NULL;
	if (type == LAYER_INVALID) name = "LAYER_INVALID";
	else if (type == LAYER_FULLY_CONNECTED) name = "LAYER_FULLY_CONNECTED";
	else name = "LAYER_UNKNOWN";
	return name;
}


/*************************************************************
* LAYER FUNCTIONALITY
*************************************************************/

// may add multiple layer types in the future
BOOL layer_type_is_valid(LayerType type) {
	check(type == LAYER_FULLY_CONNECTED, "@type is %d", type);

	return TRUE;
ERROR
	return FALSE;
}


// NEW LAYER INTERFACE
Status layer_init(
		Layer* layer,
		LayerType type,
		uint32_t n_neurons,
		NeuronClass* neuron_class,
		SynapseClass* synapse_class,
		char* name) {

	Status status = FAIL;
	uint32_t i = 0;

	check(layer != NULL, null_argument("layer"));
	check(layer_type_is_valid(type) == TRUE, invalid_argument("type"));
	check(n_neurons > 0, "@n_neurons is 0");
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));
	check(synapse_class_is_valid(synapse_class) == TRUE, invalid_argument("synapse_class"));
	check(name != NULL, null_argument("name"));
	
	status = array_init(&(layer->neurons), n_neurons, n_neurons, sizeof(Neuron));
	check(status == SUCCESS, "Couldn't init @layer->neurons");
	for (i = 0; i < n_neurons; ++i) {
		status = neuron_init(array_get(&(layer->neurons), i), neuron_class);
		check(status == SUCCESS, "Failed to init neuron %d", i);
	}

	String* name_s = string_create(name);
	check(string_is_valid(name_s) == TRUE, invalid_argument("name"));

	layer->type = type;
	layer->neuron_class = neuron_class;
	layer->synapse_class = synapse_class;
	layer->name = name_s;
	layer->input_names = array_create(1, 0, sizeof(String*));
	check(array_is_valid(layer->input_names) == TRUE, invalid_argument("layer->input_names"));

	switch (type)
	{
	case LAYER_FULLY_CONNECTED:
		layer->link = layer_link_fc;
		break;
	default:
		break;
	}

	return SUCCESS;

ERROR
	// reset neurons, working with unsigned, need underflow
	if (i < layer->neurons.length) {
		--i;
		while (i < layer->neurons.length) {
			neuron_reset(array_get(&(layer->neurons), i));
			--i;
		}
		array_reset(&(layer->neurons), NULL);
	}
	return FAIL;
}


Status layer_init_fully_connected(
		Layer* layer, 
		uint32_t n_neurons,
		NeuronClass* neuron_class,
		SynapseClass* synapse_class,
		char* name) {
	return layer_init(layer, LAYER_FULLY_CONNECTED, n_neurons, neuron_class, synapse_class, name);
}


Layer* layer_create(
		LayerType type,
		uint32_t n_neurons,
		NeuronClass* neuron_class,
		SynapseClass* synapse_class,
		char* name) {
	Status status = FAIL;
	Layer* layer = (Layer*)malloc(sizeof(Layer), "layer_create");
	check_memory(layer);

	status = layer_init(layer, type, n_neurons, neuron_class, synapse_class, name);
	check(status == SUCCESS, "Could not initialize @layer");

	return layer;

ERROR
	if (layer != NULL) free(layer);
	return NULL;
}


Layer* layer_create_fully_connected(
		uint32_t n_neurons,
		NeuronClass* neuron_class,
		SynapseClass* synapse_class,
		char* name) {
	return layer_create(LAYER_FULLY_CONNECTED, n_neurons, neuron_class, synapse_class, name);
}


Status layer_add_input_layer(Layer* layer, Layer* input) {
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	check(layer_is_valid(input) == TRUE, invalid_argument("input"));
	String* input_name = string_copy(input->name);
	return array_append(layer->input_names, &input_name);
ERROR
	return FAIL;
}


Status layer_link_input_layer(Layer* layer, Layer* input) {
	Status status = FAIL;
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	check(layer_is_valid(input) == TRUE, invalid_argument("input"));
	status = layer_add_input_layer(layer, input);
	check(status == FALSE, "@layer_add_input_layer failed");
	status = layer->link(layer, input);
ERROR
	return status;
}


const char* layer_get_name(Layer* layer) {
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	return string_get_C_string(layer->name);
ERROR
	return NULL;
}


void layer_reset(Layer* layer) {
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	layer->type = LAYER_INVALID;
	layer->neuron_class = NULL;
	layer->synapse_class = NULL;
	layer->link = NULL;
	string_destroy(layer->name);
	strings_destroy(layer->input_names);
	array_reset(&(layer->neurons), neuron_reset);

ERROR
	return;
}


void layer_destroy(Layer* layer) {
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	layer_reset(layer);
	free(layer);

ERROR
	return;
}


Status layer_step(Layer* layer, uint32_t time) {
	// NOTE: every neuron is updating the state of its input synapse
	// => only need to care about neurons state here
	uint32_t i = 0;
	Neuron* neuron = NULL;

	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
		neuron_step(neuron, time);
	}
	return SUCCESS;

ERROR
	return FAIL;
}


void layer_clear_state(Layer* layer) {
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	uint32_t i = 0;
	Neuron* neuron = NULL;
	
	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		neuron_clear_state(neuron);
	}

ERROR
	return;
}



Status layer_step_force_spikes(Layer* layer, ArrayBool* spikes, uint32_t time) {
	check(layer_is_valid(layer), invalid_argument("layer"));
	check(array_is_valid(spikes), invalid_argument("spikes"));
	check(layer->neurons.length == spikes->length, "@layer->neurons.length is %u and @spikes->length is %u at time %u", layer->neurons.length, spikes->length, time);

	uint32_t i = 0;
	BOOL spike = FALSE;
	Neuron* neuron = NULL;

	for (i = 0; i < spikes->length; ++i) {
		spike = *((BOOL*)array_get(spikes, i));
		if (spike == TRUE) {
			neuron = (Neuron*)array_get(&(layer->neurons), i);
			check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
			neuron_step_force_spike(neuron, time);
		}
	}

	return SUCCESS;
ERROR
	return FAIL;
}


Status layer_step_inject_currents(Layer* layer, ArrayFloat* currents, uint32_t time) {
	check(layer_is_valid(layer), invalid_argument("layer"));
	check(array_is_valid(currents), invalid_argument("currents"));
	check(layer->neurons.length == currents->length, "@layer->neurons.length is %u and @currents->length is %u at time %u", layer->neurons.length, currents->length, time);

	uint32_t i = 0;
	float current = 0.0f;
	Neuron* neuron = NULL;

	for (i = 0; i < currents->length; ++i) {
		current = *((float*)array_get(currents, i));
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
		neuron_step_inject_current(neuron, current, time);
	}

	return SUCCESS;
ERROR
	return FAIL;
}


ArrayBool* layer_get_spikes(Layer* layer) {
	ArrayBool* spikes = NULL;
	Neuron* neuron = NULL;
	uint32_t i = 0;
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	spikes = array_create(layer->neurons.length, 0, sizeof(BOOL));
	check(array_is_valid(spikes) == TRUE, invalid_argument("spikes"));

	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

		array_append(spikes, &(neuron->spike));
	}

	return spikes;
ERROR
	if (spikes != NULL) array_destroy(spikes, NULL);
	return NULL;
}


ArrayFloat* layer_get_voltages(Layer* layer) {
	ArrayFloat* voltages = NULL;
	Neuron* neuron = NULL;
	uint32_t i = 0;
	check(layer_is_valid(layer) == TRUE, "@layer is not valid");

	voltages = array_create(layer->neurons.length, 0, sizeof(float));
	check(array_is_valid(voltages) == TRUE, invalid_argument("voltages"));

	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

		array_append(voltages, &(neuron->u));
	}

	return voltages;
ERROR
	if (voltages != NULL) array_destroy(voltages, NULL);
	return NULL;
}


Status layer_link_fc(Layer* layer, Layer* input_layer) {
	uint32_t i = 0;
	uint32_t j = 0;
	Neuron* neuron_layer = NULL;
	Neuron* neuron_input_layer = NULL;
	Synapse* synapse = NULL;

	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	check(layer_is_valid(input_layer) == TRUE, invalid_argument("input_layer"));

	for (i = 0; i < layer->neurons.length; ++i) {
		neuron_layer = (Neuron*)array_get(&(layer->neurons), i);
		check(neuron_is_valid(neuron_layer) == TRUE, invalid_argument("neuron_layer"));

		for (j = 0; j < input_layer->neurons.length; ++j) {
			neuron_input_layer = (Neuron*)array_get(&(input_layer->neurons), j);
			check(neuron_is_valid(neuron_input_layer) == TRUE, invalid_argument("neuron_input_layer"));

			synapse = synapse_create(layer->synapse_class, 1.0f);
			check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
			
			// copy the synapse into the @neuron_layer and get it back to have the reference for the @neuron_input_layer
			neuron_add_in_synapse(neuron_layer, synapse);
			synapse = *((Synapse**)array_get(&(neuron_layer->in_synapses_refs), neuron_layer->in_synapses_refs.length - 1));
			check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
			neuron_add_out_synapse(neuron_input_layer, synapse);
		}
	}

	return SUCCESS;

ERROR
	// TODO: currently if a synapse is not well allocated the system goes on
	// need to dealocate the memory -> need to define what happends then
	// PROBABLY not a problem

	return FAIL;
}


void layer_summary(Layer* layer) {
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	
	uint32_t n_synapses = 0;
	uint32_t i = 0;
	Neuron* neuron = NULL;
	String* name = NULL;

	printf("Name: %s\n", string_get_C_string(layer->name));
	printf("Type: %s\n", layer_type_C_string(layer->type));
	printf("Neurons %u of Type: %s\n", layer->neurons.length, neuron_type_C_string(layer->neuron_class->type));
	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		n_synapses += neuron->in_synapses_refs.length;
	}
	printf("Synapse %u of Type: %s\n", n_synapses, synapse_type_C_string(layer->synapse_class->type));
	printf("Input layers:");
	for (i = 0; i < layer->input_names->length; ++i) {
		name = *((String**)array_get(layer->input_names, i));
		printf(" %s", string_get_C_string(name));
	}
	printf("\n");

ERROR
	return;
}


size_t layer_get_weights_number(Layer* layer) {
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	size_t n_weights = 0;
	uint32_t i = 0;
	Neuron* neuron = NULL;

	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		n_weights += (size_t)neuron->in_synapses_refs.length;
	}
	return n_weights;

ERROR
	return 0;
}

// old way of doing stuff
Status layer_init_with_input_names(
	Layer* layer,
	LayerType type,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	String* name,
	Array* input_names) {

	uint32_t i = 0;
	Status status = FAIL;

	check(layer != NULL, null_argument("layer"));
	check(layer_type_is_valid(type) == TRUE, invalid_argument("type"));
	check(n_neurons > 0, "@n_neurons is 0");
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));
	check(synapse_class_is_valid(synapse_class) == TRUE, invalid_argument("synapse_class"));
	check(string_is_valid(name) == TRUE, invalid_argument("name"));
	check(array_is_valid(input_names) == TRUE, invalid_argument("input_names"));

	status = array_init(&(layer->neurons), n_neurons, n_neurons, sizeof(Neuron));
	check(status == SUCCESS, "Couldn't init @layer->neurons");

	layer->type = type;
	layer->neuron_class = neuron_class;
	layer->synapse_class = synapse_class;
	layer->name = name;
	layer->input_names = input_names;

	for (i = 0; i < n_neurons; ++i) {
		status = neuron_init(array_get(&(layer->neurons), i), neuron_class);
		check(status == SUCCESS, "Failed to init neuron %d", i);
	}

	switch (type)
	{
	case LAYER_FULLY_CONNECTED:
		layer->link = layer_link_fc;
		break;
	default:
		break;
	}

	return SUCCESS;

	ERROR
		// reset initiliazed neurons
		if (i > 0) {
			--i;
			while (i < layer->neurons.length) {
				neuron_reset(array_get(&(layer->neurons), i));
				--i;
			}
			array_reset(&(layer->neurons), NULL);
		}
	return FAIL;
}


Status layer_init_fully_connected_with_input_names(
	Layer* layer,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	String* name,
	Array* input_names) {
	return layer_init_with_input_names(layer, LAYER_FULLY_CONNECTED, n_neurons, neuron_class, synapse_class, name, input_names);
}


Layer* layer_create_with_input_names(
	LayerType type,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	String* name,
	Array* input_names) {
	Status status = FAIL;
	Layer* layer = (Layer*)malloc(sizeof(Layer), "layer_create");
	check_memory(layer);

	status = layer_init_with_input_names(layer, type, n_neurons, neuron_class, synapse_class, name, input_names);
	check(status == SUCCESS, "Could not initialize @layer");

	return layer;

	ERROR
		if (layer != NULL) free(layer);
	return NULL;
}


Layer* layer_create_fully_connected_with_input_names(
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	String* name,
	Array* input_names) {
	return layer_create_with_input_names(LAYER_FULLY_CONNECTED, n_neurons, neuron_class, synapse_class, name, input_names);
}
