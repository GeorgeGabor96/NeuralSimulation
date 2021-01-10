#ifndef __LAYER_H__
#define __LAYER_H__

#include "config.h"
#include "debug.h"
#include "Neuron.h"
#include "Synapse.h"
#include "Containers.h"


struct Layer;
typedef struct Layer Layer;

typedef Status (*Layer_step)(Layer*, uint32_t);
typedef Status(*Layer_backward)(Layer*, uint32_t);
typedef Status(*Layer_link)(Layer* layer, Layer* input_layer);

// Need a type of the layer, this will define how synapses are connected between neurons
typedef enum { LAYER_INVALID = 0, LAYER_FULLY_CONNECTED = 1 } LayerType;


// Possible optimmization of memory: DO I NEED A NEURON TO HOLD INFORMATION ABOUT ITS CLASS?, In sensul ca am neuroni pe layer de acelasi tip, nu pot tine asta in layer?? direct

/*-------------------NOTE---------------------------
After succesfull creation the layer has ownership over:
1. @neurons
2. @name
3. @input_names
and it will delete them in the destroy function

The caller needs to handle to deallocation of @neuron_class and @synapse_class
--------------------------------------------------*/
struct Layer {
	LayerType type;
	NeuronClass* neuron_class;   // managed by the simulator
	SynapseClass* synapse_class; // managed by the simulator
	Array* neurons;				 // managed by the layer
	Layer_link link;			 // pointer to link function
	Array* name;				 // array of char's
	Array* input_names;		 // vector of array's of char's
};

/*
* A @layer is valid if:
* 1. @layer != NULL
* 2. @layer->type is one of the supported ones
* 2. @layer->neuron_class is valid
* 3. @layer->synapse_class is valid
* 4. @layer->neurons is valid
* 5. @layer->name is valid
* 6. @layer->input_names is valid
*/
Status layer_is_valid(Layer* layer);

Status layer_init(
	Layer* layer,					// != NULL
	LayerType type,					
	uint32_t n_neurons,				// > 0
	NeuronClass* neuron_class,		// valid
	SynapseClass* synapse_class,	// valid
	Array* name,					// valid
	Array* input_names);			// valid

Status layer_init_fully_connected(
	Layer* layer, 
	uint32_t n_neurons, 
	NeuronClass* neuron_class, 
	SynapseClass* synapse_class, 
	Array* name, 
	Array* input_names);

Layer* layer_create(
	LayerType type, 
	uint32_t n_neurons, 
	NeuronClass* neuron_class, 
	SynapseClass* synapse_class,
	Array* name,
	Array* input_names);

Layer* layer_create_fully_connected(
	uint32_t n_neurons, 
	NeuronClass* neuron_class, 
	SynapseClass* synapse_class,
	Array* name,
	Array* input_names);

void layer_reset(Layer* layer);

void layer_destroy(Layer* layer);

Status layer_step(Layer* layer, uint32_t time);

// get outputs
Array* layer_get_spikes(Layer* layer);
Array* layer_get_voltages(Layer* layer);

// set inputs
Status layer_set_spikes(Layer* layer, Array* spikes, uint32_t time);
Status layer_set_currents(Layer* layer, Array* currents, uint32_t time);

// LINK functions
Status layer_link_fc(Layer* layer, Layer* input_layer);

#endif // __LAYER_H__