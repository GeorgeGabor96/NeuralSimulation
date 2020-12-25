#include "Layer.h"

#include <stdlib.h>


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status layer_is_valid(Layer* layer) {
	check(layer != NULL, null_argument("layer"));
	check(layer->link != NULL, "NULL value for @layer->link");
	check(layer->neuron_class != NULL, null_argument("layer->neuron_class"));
	check(layer->synapse_class != NULL, null_argument("layer->synapse_class"));
	check(layer->neurons != NULL, null_argument("layer->neurons"));

	return TRUE;

error:
	return FALSE;
}

// DO I NEED A NEURON TO HOLD INFORMATION ABOUT ITS CLASS?, In sensul ca am neuroni pe layer de acelasi tip, nu pot tine asta in layer?? direct

Status layer_init(Layer* layer, LayerType type, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class) {
	check(layer != NULL, null_argument("layer"));
	check(neuron_class != NULL, null_argument("neuron_class"));
	check(synapse_class != NULL, null_argument("synapse_class"));

	layer->neurons = array_create(n_neurons, sizeof(Neuron));
	check_memory(layer->neurons);

	layer->type = type;
	layer->neuron_class = neuron_class;
	layer->synapse_class = synapse_class;

	uint32_t i = 0;
	for (i = 0; i < n_neurons; ++i) {
		check(SUCCESS == neuron_init(array_get(layer->neurons, i), neuron_class), "Failed to init neuron %d", i);
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


Status layer_init_fully_connected(Layer* layer, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class) {
	return layer_init(layer, LAYER_FULLY_CONNECTED, n_neurons, neuron_class, synapse_class);
}


Status layer_init_input(Layer* layer, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class) {
	return layer_init(layer, LAYER_INPUT, n_neurons, neuron_class, synapse_class);
}


Layer* layer_create(LayerType type, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class) {
	Layer* layer = (Layer*)malloc(sizeof(Layer));
	check_memory(layer);

	check(layer_init(layer, type, n_neurons, neuron_class, synapse_class) == SUCCESS, "Could not initialize @layer");

	return layer;

error:
	// layer_init FAILED
	if (layer != NULL) {
		free(layer);
	}
	return NULL;
}


Layer* layer_create_fully_connected(uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class) {
	return layer_create(LAYER_FULLY_CONNECTED, n_neurons, neuron_class, synapse_class);
}


Layer* layer_create_input(uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class) {
	return layer_create(LAYER_INPUT, n_neurons, neuron_class, synapse_class);
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

	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	check(layer_is_valid(input_layer) == TRUE, invalid_argument("input_layer"));

	for (i = 0; i < layer->neurons->length; ++i) {
		neuron_layer = (Neuron*)array_get(layer->neurons, i);

		for (j = 0; j < input_layer->neurons->length; ++j) {
			neuron_input_layer = (Neuron*)array_get(input_layer->neurons, j);

			// TODO: POSIBLE OPTIMIZATION: can allocate all in one go, but not now
			// NOTE: the add_in_synapse does copy the synapse into continuous memory
			// so it does not matter the way in which it is initially alocated
			synapse = synapse_create(layer->synapse_class, 1.0f);
			check(synapse != NULL, "Could not allocate");

			// copy the synapse into the @neuron_layer and get it back to have the reference fot he @neuron_input_layer
			neuron_add_in_synapse(neuron_layer, synapse);
			synapse = (Synapse*)vector_get(neuron_layer->in_synapses, neuron_layer->in_synapses->length - 1);
			check(synapse != NULL, null_argument("synapse"));
			neuron_add_out_synapse(neuron_input_layer, synapse);
		}
	}

error:
	// TODO: currently if a synapse is not well allocated the system goes on
	// need to dealocate the memory

	return FALSE;
}


Status layer_step(Layer* layer, uint32_t time) {
	// NOTE: every neuron is updating the state of its input synapse
	// => only need to care about neurons state here
	// TODO: maybe if this layer is the input???

	uint32_t i = 0;
	Neuron* neuron = NULL;

	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	for (i = 0; i < layer->neurons->length; ++i) {
		neuron = (Neuron*)array_get(layer->neurons, i);
		check(neuron != NULL, null_argument("neuron"));
		neuron_step(neuron, time);
	}

error:
	return FAIL;
}