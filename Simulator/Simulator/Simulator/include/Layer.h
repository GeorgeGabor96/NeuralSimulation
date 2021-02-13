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

const char* layer_type_C_string(LayerType type);

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
	Layer_link link;			 // pointer to link function
	String* name;				 // array of char's
	Array* input_names;			 // vector of array's of char's
	Array neurons;				 // managed by the layer
	bool is_input;				 // if the step function was called for this layer
};

/*
* A @layer is valid if:
* 1. @layer != NULL
* 2. @layer->type is one of the supported ones
* 3. @layer->neuron_class is valid
* 4. @layer->synapse_class is valid
* 5. @layer->link != NULL
* 6. @layer->name is valid
* 7. @layer->input_names is valid
* 8. @layer->neurons is valid
*/
bool layer_is_valid(Layer* layer);
bool layer_type_is_valid(LayerType type);

// NEW CREATION API
Status layer_init(
	Layer* layer,
	LayerType type,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	char* name);

Status layer_init_fully_connected(
	Layer* layer,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	char* name);

Layer* layer_create(
	LayerType type,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	char* name);

Layer* layer_create_fully_connected(
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	char* name);

// stores that @input is an input layer for @layer, does not call @link
Status layer_add_input_layer(Layer* layer, Layer* input);

// stores that @input is an input layer for @layer, does call @link
Status layer_link_input_layer(Layer* layer, Layer* input);

const char* layer_get_name(Layer* layer);

// NORMAL FUNCTIONALITY
void layer_reset(Layer* layer);
void layer_destroy(Layer* layer);

Status layer_step(Layer* layer, uint32_t time);
/*
Use these to set the input of the network
*/
Status layer_step_force_spikes(Layer* layer, ArrayBool* spikes, uint32_t time);
Status layer_step_inject_currents(Layer* layer, ArrayFloat* currents, uint32_t time);

/*
use these to get the output of the network
*/
ArrayBool* layer_get_spikes(Layer* layer);
ArrayFloat* layer_get_voltages(Layer* layer);

void layer_summary(Layer* layer);
size_t layer_get_weights_number(Layer* layer);

// LINK functions
Status layer_link_fc(Layer* layer, Layer* input_layer);


// OLD CREATION API
Status layer_init_with_input_names(
	Layer* layer,
	LayerType type,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	String* name,
	Array* input_names);

Status layer_init_fully_connected_with_input_names(
	Layer* layer,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	String* name,
	Array* input_names);

Layer* layer_create_with_input_names(
	LayerType type,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	SynapseClass* synapse_class,
	String* name,
	Array* input_names);

Layer* layer_create_fully_connected_with_input_names(
	uint32_t n_neurons, 
	NeuronClass* neuron_class, 
	SynapseClass* synapse_class,
	String* name,
	Array* input_names);

#endif // __LAYER_H__