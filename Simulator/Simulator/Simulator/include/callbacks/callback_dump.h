#ifndef __CALLBACK_DUMP_LAYER_NEURONS_H__
#define __CALLBACK_DUMP_LAYER_NEURONS_H__


#include "config.h"
#include "callbacks/callback.h"

// this will make a png plot for every neuron | one big plot for all neurons
// will dump voltages and spikes | can I do this in one plot??

/*
General callback that dumps the the PSC, voltage and spike evolution of every neuron in a given layer
Needs a reference to a @layer object, make sure the reference stays valid
If you are using a @network, get the @layer from the @network

Parameters:
@layer - pointer to a @Layer object from which to dump the information
@output_folder - path to a folder where to put the results, it will create a folder with the name of the @layer
@plot - flag that controls if it should call the python plotting script, plotting is usually slow
*/
Callback* callback_dump_layer_neurons_create(Layer* layer, const char* output_folder, BOOL plot);


/*
Similar to @callback_dump_layer_neurons_create, but it will dump every layer in the network
But it will not call any python script

Parameters:
@network - pointer to a @netoerk object from which to take the layers
@output_folder - path to a folder where to put the dumps, it will creat a folder for each layer
*/
Callback* callback_dump_network_create(Network* net, const char* output_folder);

#endif