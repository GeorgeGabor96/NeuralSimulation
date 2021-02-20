#include "callbacks/callback_visualize_layer_neurons.h"

#include "MemoryManagement.h"
#include "Containers.h"
#include "plotting/plotting.h"


void callback_visualize_layer_neurons_update(Callback* callback, Network* net);
void callback_visualize_layer_neurons_run(Callback* callback, Network* net);
void callback_visualize_layer_neurons_destroy(Callback* callback);


// I need to know how many neurons there are in the layer because I need an array of array or 2, one that keeps voltages and one for spikes
typedef struct C_Data {
	String output_folder;		
	Layer* layer;
	Array voltages_per_neuron; // contains an ArrayFloat for every neuron
	Array spikes_per_neuron;   // contains an ArrayBool for every neuron
} C_Data;


BOOL callback_visualize_layer_neurons_is_valid(Callback* callback) {
	check(callback != NULL, null_argument("callback"));
	check(callback->data != NULL, null_argument("callback->data"));
	check(callback->update != NULL, null_argument("callback->update"));
	check(callback->run != NULL, null_argument("callback->run"));
	check(callback->destroy != NULL, null_argument("callback->destroy"));

	C_Data* data = (C_Data*)(callback->data);
	check(string_is_valid(&(data->output_folder)) == TRUE, invalid_argument("data->output_folder"));
	check(layer_is_valid(data->layer) == TRUE, invalid_argument("data->layer"));
	check(array_is_valid(&(data->voltages_per_neuron)) == TRUE, invalid_argument("data->voltages_per_neuron"));
	check(array_is_valid(&(data->spikes_per_neuron)) == TRUE, invalid_argument("data->spikes_per_neuron"));

	return TRUE;
ERROR
	return FALSE;
}



static inline Status __array_of_arrays_init(Array* data, uint32_t length, size_t inner_element_size) {
	uint32_t i = 0;
	Status status = FAIL;
	Status status_data = FAIL;
	
	check(data != NULL, null_argument("data"));

	status_data = array_init(data, length, length, sizeof(Array));
	check(status_data == SUCCESS, "@status is %u", status_data);
	for (i = 0; i < data->length; ++i) {
		status = array_init((Array*)array_get(data, i), 10, 0, inner_element_size);
		check(status == SUCCESS, "@status is %u", status);
	}
	return SUCCESS;

ERROR
	if (status_data == SUCCESS) {
		// I know it failed at i, reset everything under i
		while (i != 0) {
			i--;
			array_reset((Array*)array_get(data, i), NULL);
		}
	}
	return FAIL;
}


static inline Status __array_of_arrays_reset(Array* data) {
	uint32_t i = 0;
	Array* inner_array = NULL;

	check(array_is_valid(data) == TRUE, invalid_argument("data"));
	for (i = 0; i < data->length; ++i) {
		inner_array = (Array*)array_get(data, i);
		check(array_is_valid(inner_array) == TRUE, "invalid @inner_array %u", i);
		array_reset(inner_array, NULL);
	}
	array_reset(data, NULL);
	return SUCCESS;

ERROR
	return FAIL;

}


Callback* callback_visualize_layer_neurons_create(Layer* layer, const char* output_folder) {
	Callback* callback = NULL;
	C_Data* data = NULL;
	Status status_output_folder = FAIL;
	Status status = FAIL;

	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	callback = (Callback*)calloc(1, sizeof(Callback*), "callback_visualize_layer_neurons_create callback");
	check_memory(callback);

	data = (C_Data*)malloc(sizeof(C_Data), "callback_visualize_layer_neurons_create data");
	check_memory(data);

	// init the data
	status = __array_of_arrays_init(&(data->voltages_per_neuron), layer->neurons.length, sizeof(float));
	check(status == SUCCESS, "status is %u", status);
	status = __array_of_arrays_init(&(data->spikes_per_neuron), layer->neurons.length, sizeof(BOOL));
	check(status == SUCCESS, "status is %u", status);
	status_output_folder = string_init(&(data->output_folder), output_folder);
	check(status_output_folder == SUCCESS, "Could not process @output_folder");
	data->layer = layer;

	callback->data = (void*)data;
	callback->update = callback_visualize_layer_neurons_update;
	callback->run = callback_visualize_layer_neurons_run;
	callback->destroy = callback_visualize_layer_neurons_destroy;

	// big question?
	// Do I need to create a folder???? -> or the fopen already will do this?
	// fopen is the answer

	return callback;

ERROR
	// data->voltages_per_neuron && data->spikes_per_neuron are taken cared by __array_of_arrays_init
	// and layer is not ours to free
	if (callback != NULL) free(callback);
	if (status_output_folder == SUCCESS) string_reset(&(data->output_folder));
	if (data != NULL) free(data);
	return NULL;
}

void callback_visualize_layer_neurons_update(Callback* callback, Network* net) {
	(net);
	check(callback_visualize_layer_neurons_is_valid(callback) == TRUE, invalid_argument("callback"));
	C_Data* data = (C_Data*)(callback->data);
	uint32_t i = 0; 

	ArrayFloat* voltages = layer_get_voltages(data->layer);
	ArrayBool* spikes = layer_get_spikes(data->layer);
	float* voltage_p = NULL;
	BOOL* spike_p = NULL;

	// save info about voltages and spikes
	for (i = 0; i < data->layer->neurons.length; ++i) {
		voltage_p = (float*)array_get(voltages, i);
		spike_p = (BOOL*)array_get(spikes, i);
		array_append(&(data->voltages_per_neuron), voltage_p);
		array_append(&(data->spikes_per_neuron), spike_p);
	}
	array_destroy(voltages, NULL);
	array_destroy(spikes, NULL);

ERROR
	return;
}

void callback_visualize_layer_neurons_run(Callback* callback, Network* net) {
	(net);
	check(callback_visualize_layer_neurons_is_valid(callback) == TRUE, invalid_argument("callback"));
	uint32_t i = 0;
	C_Data* data = (C_Data*)(callback->data);
	ArrayFloat* voltages = NULL;
	ArrayBool* spikes = NULL;
	String* output_folder = string_path_join_strings(&(data->output_folder), data->layer->name);
	String* file_path = NULL;
	char file_name[128] = { 0 };
	Array* steps = array_arange_float(data->layer->neurons.length);

	// version 1 - create 2 plots per neuron, one for voltages one for spikes
	for (i = 0; i < data->layer->neurons.length; ++i) {
		voltages = (ArrayFloat*)array_get(&(data->voltages_per_neuron), i);
		sprintf(file_name, "Voltages_N%u.png", i);
		// need some work
		file_path = string_path_join_string_and_C(output_folder, file_name); // need to write this
		plotting_scatter_plot_floats(steps, voltages, 600, 400, string_get_C_string(file_path));
		string_destroy(file_path);

		spikes = (ArrayBool*)array_get(&(data->spikes_per_neuron), i);
		sprintf(file_name, "Spikes_N%u.png", i);
		file_path = string_path_join_string_and_C(output_folder, file_name);
		plotting_scatter_plot_floats(steps, spikes, 600, 400, string_get_C_string(file_path));
		string_destroy(file_path);
	}
	string_destroy(output_folder);
	array_destroy(steps, NULL);

ERROR
	return;
}

void callback_visualize_layer_neurons_destroy(Callback* callback) {
	check(callback_visualize_layer_neurons_is_valid(callback) == TRUE, invalid_argument("callback"));

	// free data
	C_Data* data = (C_Data*)(callback->data);
	string_reset(&(data->output_folder));
	data->layer = NULL;
	__array_of_arrays_reset(&(data->voltages_per_neuron));
	__array_of_arrays_reset(&(data->spikes_per_neuron));
	free(callback->data);

	// clear memory
	callback->data = NULL;
	callback->update = NULL;
	callback->run = NULL;
	callback->destroy = NULL;
	free(callback);

ERROR
	return;
}