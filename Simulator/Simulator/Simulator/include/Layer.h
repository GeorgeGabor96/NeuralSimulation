#ifndef __LAYER_H__
#define __LAYER_H__

#include "config.h"
#include "utils/debug.h"
#include "Neuron.h"
#include "Synapse.h"
#include "Containers.h"


struct Layer;
typedef struct Layer Layer;

typedef Status (*Layer_step)(Layer*, uint32_t);
typedef Status(*Layer_backward)(Layer*, uint32_t);
typedef Status(*Layer_link)(Layer* layer, Layer* input_layer, SynapseClass* s_class);

// Need a type of the layer, this will define how synapses are connected between neurons
typedef enum { LAYER_INVALID = 0, LAYER_FULLY_CONNECTED = 1 } LayerType;
const char* layer_type_C_string(LayerType type);


// used by the network to know to link layers
typedef struct LayerInputData {
	String* layer_name;
	String* syanpse_class_name;
}LayerInputData;


/*-------------------NOTE---------------------------
After succesfull creation the layer has ownership over:
1. @neurons
2. @name
and it will delete them in the destroy function

The caller needs to handle to deallocation of @neuron_class and @synapse_class
--------------------------------------------------*/
struct Layer {
	LayerType type;
	Layer_link link;			 // pointer to link function
	String name;				 // array of char's
	Array inputs_data;			 // vector of LayerInputData
	Array neurons;				 // managed by the layer
	BOOL is_input;				 // if the step function was called for this layer
};

/*
* A @layer is valid if:
* 1. @layer != NULL
* 2. @layer->type is one of the supported ones
* 5. @layer->link != NULL
* 6. @layer->name is valid
* 7. @layer->inputs_data is valid
* 8. @layer->neurons is valid
*/
BOOL layer_is_valid(Layer* layer);
BOOL layer_type_is_valid(LayerType type);

Status layer_init(
	Layer* layer,
	LayerType type,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	char* name);

Status layer_init_fully_connected(
	Layer* layer,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	char* name);

Layer* layer_create(
	LayerType type,
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	char* name);

Layer* layer_create_fully_connected(
	uint32_t n_neurons,
	NeuronClass* neuron_class,
	char* name);

// stores that @input is an input layer for @layer, does not call @link
Status layer_add_input_layer(Layer* layer, Layer* input, SynapseClass* s_class);

// stores that @input is an input layer for @layer, does call @link
Status layer_link_input_layer(Layer* layer, Layer* input, SynapseClass* s_class);

const char* layer_get_name(Layer* layer);

// NORMAL FUNCTIONALITY
void layer_reset(Layer* layer);
void layer_destroy(Layer* layer);

Status layer_step(Layer* layer, uint32_t time);

// clear the state of neurons
void layer_clear_state(Layer* layer);

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
ArrayFloat* layer_get_psc(Layer* layer);

// utilizty functions
void layer_summary(Layer* layer);
size_t layer_get_weights_number(Layer* layer);

// LINK functions
Status layer_link_fc(Layer* layer, Layer* input_layer, SynapseClass* s_class);

#endif // __LAYER_H__