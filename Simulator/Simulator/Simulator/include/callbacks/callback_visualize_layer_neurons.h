#ifndef __CALLBACK_VISUALIZE_LAYER_NEURONS_H__
#define __CALLBACK_VISUALIZE_LAYER_NEURONS_H__


#include "config.h"
#include "callbacks/callback.h"

// this will make a png plot for every neuron | one big plot for all neurons
// will dump voltages and spikes | can I do this in one plot??

Callback* callback_visualize_layer_neurons_plot_create(const char* layer_name, const char* output_folder);
Callback* callback_visualize_layer_neurons_txt_create(const char* layer_name, const char* output_folder);


#endif