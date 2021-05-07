#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "config.h"
#include "Network.h"


/****************************************************
* INFERFACE FOR CALLBACK
*
* EACH CALLBACK NEEDS TO FOLLOW THE CALLBACK STRUCT
* TO ALLOW POLYMORPHISM
****************************************************/

struct Callback;
typedef struct Callback Callback;
typedef Array ArrayCallback;


// each callback needs an internal state, which can be anything
typedef void __CallbackData;

// check that the state variables are valid
typedef BOOL(*__callback_data_is_valid)(__CallbackData* data);

// clean the state variables specific to a callback
typedef void(*__callback_data_destroy)(__CallbackData* data);

// updates the internal state based on the current state of a network
// this should be used when the network suffers somekind of change, like every each timestamp
typedef void(*__callback_data_update)(__CallbackData* data, Network* net);

// does something meaningfull with the network or the interval state gathered so far, or with both
// like: saving the state of the newtwork to disk, or computing some kind of property of the network (ex: average activation)
typedef void(*__callback_data_run)(__CallbackData* data, Network* net);


struct Callback {
	__CallbackData* data;						// callback dependent data
	__callback_data_is_valid data_is_valid;		// checks if callback is valid
	__callback_data_destroy data_destroy;		// knows how to free the information of a given callback
	__callback_data_update data_update;			// updates the callback state
	__callback_data_run data_run;				// does something based on data gathered by callback
};


// the callback behave in the same way, so the general boiler plate code is inside of this functions
BOOL callback_is_valid(Callback* callback);
void callback_reset(Callback* callback);
void callback_destroy(Callback* callback);
void callback_update(Callback* callback, Network* net);
void callback_run(Callback* callback, Network* net);



/****************************************************
* EXISTING CALLBACKS
****************************************************/
/*
General callback that dumps the the PSC, voltage and spike evolution of every neuron in a given layer
Needs a reference to a @layer object, make sure the reference stays valid
If you are using a @network, get the @layer from the @network

Parameters:
@layer - pointer to a @Layer object from which to dump the information
@output_folder - path to a folder where to put the results, it will create a folder with the name of the @layer
*/
Callback* callback_dump_layer_neurons_create(Layer* layer, const char* output_folder);


/*
Similar to @callback_dump_layer_neurons_create, but it will dump every layer in the network

Parameters:
@network - pointer to a @network object from which to take the layers
@output_folder - path to a folder where to put the dumps, it will create a folder for each layer with the layer's name
*/
Callback* callback_dump_network_create(Network* net, const char* output_folder);


/*
Callback that detects if the activity of the network respects a synfire chain protocol, meaning that the spike are organized in
syncronous fashion and are propagating from layer to layer consistently

The network is given in the @callback_run method

Parameters:
@net - pointer the the network object

@max_sync_act_duration - the maximum "pulse" or "wave" of activity that the syncronous activity should last
	The callback computes for each neuron a duration of a pulse by finding the start of the activity and searching for the last spike

@min_ratio - the minimum ratio between the duration of activity for the second layer and the last layer
	if the mean_act_second_layer / mean_act_last_layer < @min_ratio -> the activity is considered to be dying

@max_ratio - the maximum ratio between the duration of activity for the second layer and the last layer
	if the mean_act_second_layer / mean_act_last_layer > @min_ratio -> the activity is considered to become epileptic

@output_file_path - path to the file where to write the state of the network

!!! NOTE: Currently on the ratio between the second and last layer is considered, but a more complex analysis can be conducted
	for example considering all the layers, or more complex structures than a sequential network
*/
Callback* callback_detect_synfire_activity_create(
	Network* net,
	uint32_t max_sync_act_duration,
	float min_ratio,
	float max_ratio,
	const char* output_file_path
);

#endif // __CALLBACK_H__
