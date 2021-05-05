#ifndef __CALLBACK_DUMP_LAYER_NEURONS_H__
#define __CALLBACK_DUMP_LAYER_NEURONS_H__


#include "config.h"
#include "callbacks/callback.h"


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

#endif