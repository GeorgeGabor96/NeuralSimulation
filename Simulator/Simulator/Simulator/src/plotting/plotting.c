#include "plotting/plotting.h"

// DO NOT TEST THIS MODULE UNTIL YOU CAN DO THE FULL PIPELINE (load image, plots, save image)
// I am sure there are a lot of memory leaks


void plotting_scatter_plot_floats(ArrayFloat* xs, ArrayFloat* ys, size_t width, size_t height, char* path) {
	ArrayDouble* d_xs = array_float_to_double(xs, FALSE);
	ArrayDouble* d_ys = array_float_to_double(ys, FALSE);

	plotting_scatter_plot_doubles(d_xs, d_ys, width, height, path);

	array_destroy(d_xs, NULL);
	array_destroy(d_ys, NULL);
}


void plotting_scatter_plot_doubles(ArrayDouble* xs, ArrayDouble* ys, size_t width, size_t height, char* path) {
	RGBABitmapImageReference* image_reference = CreateRGBABitmapImageReference();
	DrawScatterPlot(image_reference, 
					(double)width, 
					(double)height, 
					(double*)xs->data, 
					(size_t)(xs->length), 
					(double*)ys->data, 
					(size_t)(xs->length));

	size_t length;
	double* pngdata = ConvertToPNG(&length, image_reference->image);
	WriteToFile(pngdata, length, path);
	DeleteImage(image_reference->image);
}
