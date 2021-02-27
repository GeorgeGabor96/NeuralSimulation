#include "callbacks/callback_visualize_layer_neurons.h"

#include "utils/MemoryManagement.h"
#include "Containers.h"
#include "utils/os.h"
#include <time.h>


BOOL callback_visualize_layer_neurons_is_valid(Callback* callback);
void callback_visualize_layer_neurons_update(Callback* callback, Network* net);
void callback_visualize_layer_neurons_run(Callback* callback, Network* net);
void callback_visualize_layer_neurons_destroy(Callback* callback);
void callback_visualize_layer_neurons_reset(Callback* callback);


typedef struct C_Data {
	String output_folder;		
	Layer* layer;				// does not take ownership
	Array voltages_per_neuron;	// contains an ArrayFloat for every neuron
	Array spikes_per_neuron;	// contains an ArrayBool for every neuron
	uint32_t steps;				// stores how many time the update method was called
	uint32_t plot_width;		// the number of pixels for width 
	uint32_t plot_height;		// the number of pixels for heigth
} C_Data;


Callback* callback_visualize_layer_neurons_create(Layer* layer, const char* output_folder, uint32_t plot_width, uint32_t plot_height) {
	Callback* callback = NULL;
	C_Data* data = NULL;
	Status status = FAIL;

	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));
	check(plot_width > 0, "@plot_width == 0");
	check(plot_height > 0, "@plot_height == 0");

	callback = (Callback*)calloc(1, sizeof(Callback), "callback_visualize_layer_neurons_create callback");
	check_memory(callback);

	data = (C_Data*)malloc(sizeof(C_Data), "callback_visualize_layer_neurons_create data");
	check_memory(data);

	// init the data
	status = array_of_arrays_init(&(data->voltages_per_neuron), layer->neurons.length, sizeof(float));
	check(status == SUCCESS, "status is %u", status);
	status = array_of_arrays_init(&(data->spikes_per_neuron), layer->neurons.length, sizeof(BOOL));
	check(status == SUCCESS, "status is %u", status);
	
	// create output folder
	String* output_folder_s = string_path_join_C_and_string(output_folder, layer->name);
	check(string_is_valid(output_folder_s) == TRUE, invalid_argument("output_folder_s"));
	memcpy(&(data->output_folder), output_folder_s, sizeof(String));
	free(output_folder_s);
	os_mkdir(data->output_folder.data);

	data->steps = 0;
	data->layer = layer;
	data->plot_width = plot_width;
	data->plot_height = plot_height;

	callback->data = (void*)data;
	callback->is_valid = callback_visualize_layer_neurons_is_valid;
	callback->update = callback_visualize_layer_neurons_update;
	callback->run = callback_visualize_layer_neurons_run;
	callback->destroy = callback_visualize_layer_neurons_destroy;
	callback->reset = callback_visualize_layer_neurons_reset;
	return callback;

ERROR
	// data->voltages_per_neuron && data->spikes_per_neuron are taken cared by array_of_arrays_init
	// and layer is not ours to free
	if (callback != NULL) free(callback);
	if (data->output_folder.data != NULL) string_reset(&(data->output_folder));
	if (data != NULL) free(data);
	return NULL;
}


void callback_visualize_layer_neurons_reset(Callback* callback) {
	check(callback_visualize_layer_neurons_is_valid(callback) == TRUE, invalid_argument("callback"));
	// free data
	C_Data* data = (C_Data*)(callback->data);
	string_reset(&(data->output_folder));
	data->layer = NULL;
	data->steps = 0;
	data->plot_width = 0;
	data->plot_height = 0;
	array_of_arrays_reset(&(data->voltages_per_neuron));
	array_of_arrays_reset(&(data->spikes_per_neuron));
	free(callback->data);

	// clear memory
	callback->data = NULL;
	callback->is_valid = NULL;
	callback->update = NULL;
	callback->run = NULL;
	callback->destroy = NULL;
	callback->reset = NULL;
ERROR
	return;
}


void callback_visualize_layer_neurons_destroy(Callback* callback) {
	check(callback_visualize_layer_neurons_is_valid(callback) == TRUE, invalid_argument("callback"));
	callback_visualize_layer_neurons_reset(callback);
	free(callback);

ERROR
	return;
}


BOOL callback_visualize_layer_neurons_is_valid(Callback* callback) {
	check(callback != NULL, null_argument("callback"));
	check(callback->data != NULL, null_argument("callback->data"));
	check(callback->is_valid != NULL, null_argument("callback->is_valid"));
	check(callback->update != NULL, null_argument("callback->update"));
	check(callback->run != NULL, null_argument("callback->run"));
	check(callback->destroy != NULL, null_argument("callback->destroy"));
	check(callback->reset != NULL, null_argument("callback->reset"));

	C_Data* data = (C_Data*)(callback->data);
	check(string_is_valid(&(data->output_folder)) == TRUE, invalid_argument("data->output_folder"));
	check(layer_is_valid(data->layer) == TRUE, invalid_argument("data->layer"));
	check(array_is_valid(&(data->voltages_per_neuron)) == TRUE, invalid_argument("data->voltages_per_neuron"));
	check(array_is_valid(&(data->spikes_per_neuron)) == TRUE, invalid_argument("data->spikes_per_neuron"));

	return TRUE;
ERROR
	return FALSE;
}


void callback_visualize_layer_neurons_update(Callback* callback, Network* net) {
	(net);
	check(callback_visualize_layer_neurons_is_valid(callback) == TRUE, invalid_argument("callback"));
	C_Data* data = (C_Data*)(callback->data);
	uint32_t i = 0; 

	ArrayFloat* voltages = layer_get_voltages(data->layer);
	ArrayBool* spikes = layer_get_spikes(data->layer);
	ArrayFloat* inner_voltage = NULL;
	ArrayBool* inner_spike = NULL;
	float* voltage_p = NULL;
	BOOL* spike_p = NULL;

	// save info about voltages and spikes
	for (i = 0; i < data->layer->neurons.length; ++i) {
		// get values for layer
		voltage_p = (float*)array_get(voltages, i);
		spike_p = (BOOL*)array_get(spikes, i);
		
		// add them into their containers
		inner_voltage = (ArrayFloat*)array_get(&(data->voltages_per_neuron), i);
		inner_spike = (ArrayBool*)array_get(&(data->spikes_per_neuron), i);
		array_append(inner_voltage, voltage_p);
		array_append(inner_spike, spike_p);
	}
	array_destroy(voltages, NULL);
	array_destroy(spikes, NULL);
	(data->steps)++;

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
	String* file_path = NULL;
	String* data_name = NULL;
	char file_name[128] = { 0 };
	ArrayFloat* spikes_f = NULL;
	Array* steps = array_arange_float(data->steps);

	// create 2 binaries per neuron, one for voltages one for spikes
	for (i = 0; i < data->layer->neurons.length; ++i) {
		
		voltages = (ArrayFloat*)array_get(&(data->voltages_per_neuron), i);
		sprintf(file_name, "voltages_N%u.bin", i);
		file_path = string_path_join_string_and_C(&(data->output_folder), file_name);
		data_name = string_create("voltage");
		array_float_dump(voltages, file_path, data_name);
		string_destroy(file_path);
		string_destroy(data_name);

		spikes = (ArrayBool*)array_get(&(data->spikes_per_neuron), i);
		sprintf(file_name, "spikes_N%u.bin", i);
		file_path = string_path_join_string_and_C(&(data->output_folder), file_name);
		data_name = string_create("spikes");
		array_bool_dump(spikes, file_path, data_name);
		string_destroy(file_path);
		string_destroy(data_name);
	}
	array_destroy(steps, NULL);

	// call the plotting script
	os_plot_layers(string_get_C_string(&(data->output_folder)));
ERROR
	return;
}