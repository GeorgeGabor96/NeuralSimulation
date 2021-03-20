#ifndef __NETWORK_H__
#define __NETWORK_H__


#include "config.h"
#include "Layer.h"
#include "Containers.h"


typedef enum { SPIKES = 0, CURRENT = 1, VOLTAGE = 2 } NetworkValueType;

// Chestia este ca input-ul si output-ul unei retele practic este acelasi structura
// caci ca input trebuie un vector de arrays cu valori si un tip (spike sau current)
// iar la output ai un vector de arrays cu valori si un tip (spike sau voltaj)

// if I add a type to the array and the other structures, i could directly use an array, and i could also verify the type when i receive an array
typedef struct NetworkValues {
	NetworkValueType type;
	Array values;
} NetworkValues;
typedef Array NetworkInputs; // array of NetworkValues
typedef Array NetworkOutputs; // array of NetworkValues

void network_values_show(Array* values);

typedef struct Network {
	// TODO: layer can't keep the full name with them
	Array layers;			// keeps the layers, will be added one by one, network has ownership
	Array output_layers;	// one or more, keeps references to @layers
	Array input_layers;		// one or more, keeps references to @layers
	Array input_names;		// names of the input layers, reference to @layers->name
	Array output_names;		// names of the output layers, reference to @layers->name
	Array synapse_classes;	// keeps references to synaptic classes that should be used by synapses of this network, should be added one by one, network takes ownership
	Array neuron_classes;	// keeps references to the neuron classes that should be used by neurons of this network, should be added one by one, network takes ownership
	BOOL compiled;			// if the network has been compiled
} Network;


/* 
* Network is valid if:
* 1. @network != NULL
* 2. @network->layers is valid
* 3. @network->output_layers is valid
* 4. @network->input_layers is valid
* 5. @network->input_names is valid
* 6. @network->output_names is valid
*/
BOOL network_is_valid(Network* network);

Network* network_create();
void network_destroy(Network* network);

/*
* This will copy the content of the @layer structure (shallow copy) and if everything works it will free (take ownership) the @layer pointer
* 
* Implemented using @network_add_layer_keep_valid
*/
Status network_add_layer(Network* network, Layer* layer, BOOL is_input, BOOL is_output);
/*
* Alternative to @network_add_layer when user wants to keep the @layer pointer valid
* No free will be performend but the @network will still destroy the layer itself
* 
* Use Cases: when the original @layer was on the stack
*			 if @layer was on the heap, probably its better to used @network_get_layer_... functions to get it back, instead of using this function
*/
Status network_add_layer_keep_valid(Network* network, Layer* layer, BOOL is_input, BOOL is_output);



Layer* network_get_layer_by_idx(Network* network, uint32_t layer_idx);
Layer* network_get_layer_by_string(Network* network, String* name);
Layer* network_get_layer_by_name(Network* network, char* name);
uint32_t network_get_layer_idx_by_string(Network* network, String* name);
uint32_t network_get_layer_idx_by_name(Network* network, char* name);

/*
* Adding and getting back Synapse and Neuron classes
* The network takes ownership by freeing the Synapses and NeuronClasses when it is destroy
* Retrieving is done using the name of the individual object class, the process is done liniary
*/
Status network_add_synapse_class(Network* network, SynapseClass* s_class);
Status network_add_neuron_class(Network* network, NeuronClass* n_class);
SynapseClass* network_get_synapse_class_by_string(Network* network, String* name);
SynapseClass* network_get_synapse_class_by_name(Network* network, const char* s_name);
NeuronClass* network_get_neuron_class_by_string(Network* network, String* name);
NeuronClass* network_get_neuron_class_by_name(Network* network, const char* n_name);


/*
* Check that all the layers are valid and if there are missing layers
* Then it will connect every layer with its inputs
*/
Status network_compile(Network* network);
void network_summary(Network* network);


// forward of the network for an input
// NOTE the inputs vector should have the same length as the number of input layers and the same number of values
void network_step(Network* network, NetworkInputs* inputs, uint32_t time);
NetworkOutputs* network_get_outputs(Network* network, NetworkValueType type);

// Resets all the neurons and synapses to default and initial values also removes all spikes generated
void network_clear_state(Network* network);

// TODO: maybe is easier to be specific
// TODO: get output by index???
Array* network_get_output_spikes(Network* network);
Array* network_get_output_voltages(Network* network);
Array* network_get_layer_spikes(Network* network, uint32_t i);
Array* network_get_layer_voltages(Network* network, uint32_t i);


#endif // __NETWORK_H__
