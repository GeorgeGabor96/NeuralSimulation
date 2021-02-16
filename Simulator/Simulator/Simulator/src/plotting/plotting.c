#include "plotting/plotting.h"

// DO NOT TEST THIS MODULE UNTIL YOU CAN DO THE FULL PIPELINE (load image, plots, save image)
static inline array_float_to_double(ArrayFloat* floats) {
	ArrayDouble* doubles = array_create(floats->length, floats->length, sizeof(double));
	float f_value = 0.0f;
	double d_value = 0.0;

	for (uint32_t i = 0; i < floats->length; ++i) {
		f_value = *((float*)array_get(floats, i));
		d_value = (double)f_value;
		array_set(doubles, i, &d_value);
	}

	return doubles;
}


void plotting_scatter_plot_floats(ArrayFloat* xs, ArrayFloat* ys, size_t width, size_t height, char* path) {
	ArrayDouble* d_xs = array_float_to_double(xs);
	ArrayDouble* d_ys = array_float_to_double(ys);

	plotting_scatter_plot_doubles(d_xs, d_ys, width, height, path);

	array_destroy(d_xs, NULL);
	array_destroy(d_ys, NULL);
}


void plotting_scatter_plot_doubles(ArrayDouble* xs, ArrayDouble* ys, size_t width, size_t height, char* path) {
	RGBABitmapImageReference* image_reference = CreateRGBABitmapImageReference();
	DrawScatterPlot(image_reference, (double)width, (double)height, xs->data, (size_t)(xs->length), ys->data, (size_t)(xs->length));

	size_t length;
	double* pngdata = ConvertToPNG(&length, image_reference->image);
	WriteToFile(pngdata, length, path);
	DeleteImage(image_reference->image);
}
