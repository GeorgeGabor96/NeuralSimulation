#include "Layer.h"

#include <stdlib.h>

// DO I NEED A NEURON TO HOLD INFORMATION ABOUT ITS CLASS?, In sensul ca am neuroni pe layer de acelasi tip, nu pot tine asta in layer?? direct

Status layer_init(Layer* layer, LayerType type, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class) {
	check(neuron_class != NULL, "NULL value for @neuron_class");
	check(synapse_class != NULL, "NULL value for @synapse_class");

	layer->neurons = (Neuron*)malloc(n_neurons * sizeof(Neuron));
	check_memory(layer->neurons);

	layer->type = type;
	layer->n_neurons = n_neurons;
	layer->neuron_class = neuron_class;
	layer->synapse_class = synapse_class;

	uint32_t i = 0;
	for (i = 0; i < n_neurons; ++i) {
		check(SUCCESS == neuron_init(layer->neurons + i, neuron_class), "Failed to init neuron");
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
		neuron_reset(layer->neurons + i);
		--i;
	}
	free(layer->neurons);

	return FAIL;
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
	check(layer_is_valid(layer) == TRUE, "@layer is not valid");



error:
	return;
}
