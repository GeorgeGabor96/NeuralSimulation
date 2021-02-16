#ifndef __PLOTTING_PLOTING__
#define __PLOTTING_PLOTING__

#include "plotting/pbPlots.h"
#include "plotting/supportLib.h"
#include "config.h"
#include "Containers.h"


void plotting_scatter_plot_floats(ArrayFloat* xs, ArrayFloat* ys, size_t width, size_t height, char* path);
void plotting_scatter_plot_doubles(ArrayDouble* xs, ArrayDouble* ys, size_t width, size_t height, char* path) {

#endif