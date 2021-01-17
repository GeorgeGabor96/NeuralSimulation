#include "Layer.h"

#include <stdlib.h>


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status layer_is_valid(Layer* layer) {
	check(layer != NULL, null_argument("layer"));
	check(layer->type == LAYER_FULLY_CONNECTED, "@layer->type is %d", layer->type);
	check(neuron_class_is_valid(layer->neuron_class) == TRUE, invalid_argument("layer->neuron_class"));
	check(synapse_class_is_valid(layer->synapse_class) == TRUE, invalid_argument("layer->synapse_class"));
	check(layer->link != NULL, "NULL value for @layer->link");
	check(string_is_valid(layer->name) == TRUE, invalid_argument("layer->name"));
	check(array_is_valid(layer->input_names) == TRUE, invalid_argument("layer->input_names"));
	check(array_is_valid(&(layer->neurons)) == TRUE, invalid_argument("layer->neurons"));

	return TRUE;

error:
	return FALSE;
}


/*************************************************************
* LAYER FUNCTIONALITY
*************************************************************/
Status layer_init(
		Layer* layer, 
		LayerType type, 
		uint32_t n_neurons, 
		NeuronClass* neuron_class, 
		SynapseClass* synapse_class,
		String* name,
		Array* input_names) {

	check(layer != NULL, null_argument("layer"));
	check(n_neurons > 0, "@n_neurons is 0");
	check(type == LAYER_FULLY_CONNECTED, "@type is %d", type);
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));
	check(synapse_class_is_valid(synapse_class) == TRUE, invalid_argument("synapse_class"));
	check(array_is_valid(name) == TRUE, invalid_argument("name"));
	check(array_is_valid(input_names) == TRUE, invalid_argument("input_names"));

	uint32_t i = 0;
	Status status = FAIL;
	status = array_init(&(layer->neurons), n_neurons, 0, sizeof(Neuron));
	check(status == SUCCESS, "Couldn't init @layer->neurons");

	layer->type = type;
	layer->neuron_class = neuron_class;
	layer->synapse_class = synapse_class;
	layer->name = name;
	layer->input_names = input_names;

	for (i = 0; i < n_neurons; ++i) {
		status = neuron_init(array_get(&(layer->neurons), i), neuron_class);
		check(status == SUCCESS, "Failed to init neuron %d", i);
		layer->neurons.length++;
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

error:
	// reset initiliazed neurons
	for (i = 0; i < layer->neurons.length; ++i) neuron_reset(array_get(&(layer->neurons), i));
	array_reset(&(layer->neurons), NULL);

	return FAIL;
}


Status layer_init_fully_connected(
		Layer* layer, 
		uint32_t n_neurons, 
		NeuronClass* neuron_class, 
		SynapseClass* synapse_class, 
		String* name, 
		Array* input_names) {
	return layer_init(layer, LAYER_FULLY_CONNECTED, n_neurons, neuron_class, synapse_class, name, input_names);
}


Layer* layer_create(
		LayerType type, 
		uint32_t n_neurons, 
		NeuronClass* neuron_class, 
		SynapseClass* synapse_class,
		String* name,
		Array* input_names) {
	Status status = FAIL;
	Layer* layer = (Layer*)malloc(sizeof(Layer));
	check_memory(layer);

	status = layer_init(layer, type, n_neurons, neuron_class, synapse_class, name, input_names);
	check(status == SUCCESS, "Could not initialize @layer");

	return layer;

error:
	if (layer != NULL) free(layer);
	return NULL;
}


Layer* layer_create_fully_connected(
		uint32_t n_neurons, 
		NeuronClass* neuron_class, 
		SynapseClass* synapse_class,
		Array* name,
		Array* input_names) {
	return layer_create(LAYER_FULLY_CONNECTED, n_neurons, neuron_class, synapse_class, name, input_names);
}


void layer_reset(Layer* layer) {
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	Array* name = NULL;
	layer->type = LAYER_INVALID;
	layer->neuron_class = NULL;
	layer->synapse_class = NULL;
	layer->link = NULL;

	array_reset(&(layer->neurons), neuron_reset);
	string_destroy(layer->name);
	strings_destroy(layer->input_names);

error:
	return;
}


void layer_destroy(Layer* layer) {
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	layer_reset(layer);
	free(layer);

error:
	return;
}


ArrayBool* layer_get_spikes(Layer* layer) {
	ArrayBool* spikes = NULL;
	Neuron* neuron = NULL;
	uint32_t i = 0;
	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	
	spikes = array_create(layer->neurons.length, layer->neurons.length, sizeof(bool));
	check(array_is_valid(spikes) == TRUE, invalid_argument("spikes"));

	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

		array_set(spikes, i, &(neuron->spike));
	}

error:
	return spikes;
}


ArrayFloat* layer_get_voltages(Layer* layer) {
	ArrayFloat* voltages = NULL;
	Neuron* neuron = NULL;
	uint32_t i = 0;
	check(layer_is_valid(layer) == TRUE, "@layer is not valid");
	
	voltages = array_create(layer->neurons.length, layer->neurons.length, sizeof(float));
	check(array_is_valid(voltages) == TRUE, invalid_argument("voltages"));
	
	for (i = 0; i < layer->neurons.length; ++i) {
		neuron = (Neuron*)array_get(&(layer->neurons), i);
		check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

		array_set(voltages, i, &(neuron->u));
	}

error:
	return voltages;
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

		for (j = 0; j < input_layer->neurons.length; ++j) {
			neuron_input_layer = (Neuron*)array_get(&(input_layer->neurons), j);

			// TODO: POSIBLE OPTIMIZATION: can allocate all in one go, but not now
			// NOTE: the add_in_synapse does copy the synapse into continuous memory
			// so it does not matter the way in which it is initially alocated
			synapse = synapse_create(layer->synapse_class, 1.0f);
			check_memory(synapse);

			// copy the synapse into the @neuron_layer and get it back to have the reference fot the @neuron_input_layer
			neuron_add_in_synapse(neuron_layer, synapse, TRUE);
			synapse = (Synapse*)array_get(&(neuron_layer->in_synapses), neuron_layer->in_synapses.length - 1);
			check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
			neuron_add_out_synapse(neuron_input_layer, synapse);
		}
	}

	return SUCCESS;

error:
	// TODO: currently if a synapse is not well allocated the system goes on
	// need to dealocate the memory -> need to define what happends then

	return FAIL;
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
	return TRUE;

error:
	return FAIL;
}


Status layer_set_spikes(Layer* layer, ArrayBool* spikes, uint32_t time) {
	check(layer_is_valid(layer), invalid_argument("layer"));
	check(array_is_valid(spikes), invalid_argument("spikes"));
	check(layer->neurons.length == spikes->length, "@layer->neurons.length is %u and @spikes->length is %u at time %u", layer->neurons.length, spikes->length, time);

	uint32_t i = 0;
	bool spike = NULL;
	Neuron* neuron = NULL;

	for (i = 0; i < spikes->length; ++i) {
		spike = *((bool*)array_get(spikes, i));
		if (spike == TRUE) {
			neuron = (Neuron*)array_get(&(layer->neurons), i);
			check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
			neuron_force_spike(neuron, time);
		}
	}

	return SUCCESS;
error:
	return FAIL;
}


Status layer_set_currents(Layer* layer, ArrayFloat* currents, uint32_t time) {
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

		neuron_inject_current(neuron, current, time);
	}

	return SUCCESS;
error:
	return FAIL;
}