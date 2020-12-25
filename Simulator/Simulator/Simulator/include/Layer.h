#ifndef __LAYER_H__
#define __LAYER_H__

#include "debug.h"
#include "Neuron.h"
#include "Synapse.h"
#include "Containers.h"

#include <stdint.h>

struct Layer;
typedef struct Layer Layer;

// TODO is it posible for a layer to not succed on the forward, the memory is allocated, its just calculations
typedef Status (*Layer_step)(Layer*, uint32_t);
typedef Status(*Layer_backward)(Layer*, uint32_t);
typedef Status(*Layer_link)(Layer* layer, Layer* input_layer);

// Need a type of the layer, this will define how synapses are connected between neurons
typedef enum { LAYER_INPUT = 0, LAYER_FULLY_CONNECTED = 1 } LayerType;


struct Layer {
	LayerType type;
	Array* neurons;				 // managed by the layer
	Layer_link link;			 // pointer to link function
	NeuronClass* neuron_class;   // managed by the simulator
	SynapseClass* synapse_class; // managed by the simulator
	char* name;
	char** input_names;
};

/*
* A @layer is valid if:
* 1. @layer != NULL
* 2. @layer->neuron_class != NULL
* 3. @layer->synapse_class != NULL
* 4. @layer->neurons != NULL
*/
Status layer_is_valid(Layer* layer);


// if you have the memory allocated
Status layer_init(Layer* layer, LayerType type, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class);
Status layer_init_fully_connected(Layer* layer, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class);
Status layer_init_input(Layer* layer, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class);

Layer* layer_create(LayerType type, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class);
Layer* layer_create_fully_connected(uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class);
Layer* layer_create_input(uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class);


/*
* Preconditions: @layer is valid
*/
void layer_reset(Layer* layer);

void layer_destroy(Layer* layer);
Status layer_is_valid(Layer* layer);

// the synapses are always kept in the effent_layer
Status layer_step(Layer* layer, uint32_t time);

// link functions
/*
* Preconditions: - @layer is valid
*				 - @input_layer is valid
* 
* NOTE: this can be used to link multiple 
* 
*/
Status layer_link_fc(Layer* layer, Layer* input_layer);

// get layer simulation information
Array* layer_get_spikes(Layer* layer);
Array* layer_get_voltages(Layer* layer);

#endif // __LAYER_H__