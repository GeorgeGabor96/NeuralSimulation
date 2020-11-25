#include "Layer.h"

#include <stdlib.h>

// DO I NEED A NEURON TO HOLD INFORMATION ABOUT ITS CLASS?, In sensul ca am neuroni pe layer de acelasi tip, nu pot tine asta in layer?? direct

Status layer_init(Layer* layer, LayerType type, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class) {
	check(layer != NULL, "NULL value for @layer");
	check(neuron_class != NULL, "NULL value for @neuron_class");
	check(synapse_class != NULL, "NULL value for @synapse_class");

	layer->neurons = array_create(n_neurons, sizeof(Neuron));
	check_memory(layer->neurons);

	layer->type = type;
	layer->neuron_class = neuron_class;
	layer->synapse_class = synapse_class;

	uint32_t i = 0;
	for (i = 0; i < n_neurons; ++i) {
		check(SUCCESS == neuron_init(array_get(layer->neurons, i), neuron_class), "Failed to init neuron");
	}

	switch (type)
	{
	case LAYER_FULLY_CONNECTED:
		layer->link = layer_link_fc;
		break;
	case LAYER_INPUT:
		// not sure yet;
		break;
	default:
		break;
	}

	return SUCCESS;

error:
	// reset initiliazed neurons
	while (i >= 0) {
		neuron_reset(array_get(layer->neurons, i));
		--i;
	}
	free(layer->neurons);

	return FAIL;
}


Layer* layer_create(LayerType type, uint32_t n_neurons, NeuronClass* neuron_class, Synapse* synapse_class) {
	Layer* layer = (Layer*)malloc(sizeof(Layer));
	check_memory(layer);

	check(layer_init(layer, type, n_neurons, neuron_class, synapse_class) == TRUE, "Could not initialize @layer");

	return layer;

error:
	// layer_init FAILED
	if (layer != NULL) {
		free(layer);
	}
	return NULL;
}


Status layer_is_valid(Layer* layer) {
	check(layer != NULL, "NULL value for @layer");
	check(layer->link != NULL, "NULL value for @layer->link");
	check(layer->neurons != NULL, "NULL value for @layer->neurons");
	check(layer->neuron_class != NULL, "NULL value for @layer->neuron_class");
	check(layer->synapse_class != NULL, "NULL value for @layer->synapse_class");

	return TRUE;

error:
	return FALSE;
}


void layer_reset(Layer* layer) {
	uint32_t i = 0;

	check(layer_is_valid(layer) == TRUE, "@layer is not valid");

	// need to reset every neuron
	for (i = 0; i < layer->neurons->length; ++i) {
		neuron_reset(array_get(layer->neurons, i));
	}

	array_destroy(layer->neurons);

error:
	return;
}


void layer_destroy(Layer* layer) {
	check(layer_is_valid(layer) == TRUE, "@layer is not valid");

	layer_reset(layer);
	free(layer);

error:
	return;
}




// this can be tested
Array* layer_get_spikes(Layer* layer) {
	Array* spikes = NULL;
	uint32_t i = 0;
	check(layer_is_valid(layer) == TRUE, "@layer is not valid");

	spikes = array_create(layer->neurons->length, sizeof(uint8_t));
	check(spikes != NULL, "@spikes is NULL");

	for (i = 0; i < layer->neurons->length; ++i) {
		array_set(spikes, i, &(array_get_cast(layer->neurons, i, Neuron*)->spike));
	}

error:
	return spikes;
}


// this can be tested
Array* layer_get_voltages(Layer* layer) {
	Array* voltages = NULL;
	uint32_t i = 0;
	check(layer_is_valid(layer) == TRUE, "@layer is not valid");

	voltages = array_create(layer->neurons->length, sizeof(float));

	for (i = 0; i < layer->neurons->length; ++i) {
		array_set(voltages, i, &(array_get_cast(layer->neurons, i, Neuron*)->u));
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

	check(layer_is_valid(layer) == TRUE, "@layer is not valid");
	check(layer_is_valid(input_layer) == TRUE < "@input_layer is not valid");

	for (i = 0; i < layer->neurons->length; ++i) {
		neuron_layer = array_get_cast(layer->neurons, i, Neuron*);

		for (j = 0; j < input_layer->neurons->length; ++j) {
			neuron_input_layer = array_get_cast(input_layer->neurons, j, Neuron*);

			// POSIBLE OPTIMIZATION:

			synapse = synapse_create(layer->synapse_class, 1.0f);
			check(synapse != NULL, "Could not allocate")
			// I can do one big alloc
		}
	}

error:
	return FALSE;
}
