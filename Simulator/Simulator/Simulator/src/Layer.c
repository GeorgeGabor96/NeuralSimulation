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
