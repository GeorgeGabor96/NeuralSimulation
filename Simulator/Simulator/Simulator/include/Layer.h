#ifndef __LAYER_H__
#define __LAYER_H__

#include "debug.h"
#include "Neuron.h"
#include "Synapse.h"
#include "Containers.h"

#include <stdint.h>

struct Layer;
typedef struct Layer Layer;

typedef Status (*Layer_step)(Layer*, uint32_t);
typedef Status(*Layer_backward)(Layer*, uint32_t);


typedef enum { LAYER_INPUT = 0, LAYER_FULLY_CONNECTED = 1 } LayerType;


struct Layer {
	LayerType type;
	Neuron* neurons; // will be allocated in one malloc
	uint32_t n_neurons;
	Layer_step* step; // pointer to forward function
	// Layer_backward* backward;
};


Status layer_init(Layer* layer, uint32_t n_neurons, NeuronClass* neuron_class, SynapseClass* synapse_class);
void layer_reset(Layer* layer);
Layer layer_create(uint32_t n_neurons, NeuronClass* neuron_class, Synapse* synapse_class);
void layer_destroy(Layer* layer);
Status layer_is_valid(Layer* layer);

// the synapses are always kept in the effent_layer
Status layer_link(Layer* afferent_layer, Layer* efferent_layer);



#endif // __LAYER_H__