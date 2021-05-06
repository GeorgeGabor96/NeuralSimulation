#ifndef __CALLBACK_DUMP_LAYER_NEURONS_H__
#define __CALLBACK_DUMP_LAYER_NEURONS_H__


#include "config.h"
#include "callbacks/callback.h"

// TODO rename this file into something else, or move those directly into callback.h
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


#endif