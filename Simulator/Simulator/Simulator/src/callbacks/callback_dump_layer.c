#include "Callback.h"

#include "utils/MemoryManagement.h"
#include "Containers.h"
#include "utils/os.h"
#include <time.h>


typedef struct C_Data {
	String output_folder;
	Layer* layer;				// does not take ownership
	Array voltages_per_neuron;	// contains an ArrayFloat for every neuron
	Array spikes_per_neuron;	// contains an ArrayBool for every neuron
	Array psc_per_neuron;		// contains an ArrayFloat for every neuron
	Array epsc_per_neuron;		// contains an ArrayFloat for every neuron
	Array ipsc_per_neuron;		// contains an ArrayFloat for every neuron
} C_Data;


BOOL callback_dump_layer_neurons_data_is_valid(C_Data* data);
void callback_dump_layer_neurons_data_destroy(C_Data* data);
void callback_dump_layer_neurons_data_update(C_Data* data, Network* net);
void callback_dump_layer_neurons_data_run(C_Data* data, Network* net);


Callback* callback_dump_layer_neurons_create(Layer* layer, const char* output_folder) {
	Callback* callback = NULL;
	C_Data* data = NULL;
	Status status = FAIL;

	check(layer_is_valid(layer) == TRUE, invalid_argument("layer"));

	callback = (Callback*)calloc(1, sizeof(Callback), "callback_dump_layer_neurons_create callback");
	check_memory(callback);

	data = (C_Data*)malloc(sizeof(C_Data), "callback_dump_layer_neurons_create data");
	check_memory(data);

	// init the data
	status = array_of_arrays_init(&(data->voltages_per_neuron), layer->neurons.length, sizeof(float));
	check(status == SUCCESS, "status is %u", status);
	status = array_of_arrays_init(&(data->spikes_per_neuron), layer->neurons.length, sizeof(BOOL));
	check(status == SUCCESS, "status is %u", status);
	status = array_of_arrays_init(&(data->psc_per_neuron), layer->neurons.length, sizeof(float));
	check(status == SUCCESS, "status is %u", status);
	status = array_of_arrays_init(&(data->epsc_per_neuron), layer->neurons.length, sizeof(float));
	check(status == SUCCESS, "status is %u", status);
	status = array_of_arrays_init(&(data->ipsc_per_neuron), layer->neurons.length, sizeof(float));
	
	// create output folder
	String* output_folder_s = string_path_join_C_and_string(output_folder, layer->name);
	check(string_is_valid(output_folder_s) == TRUE, invalid_argument("output_folder_s"));
	memcpy(&(data->output_folder), output_folder_s, sizeof(String));
	free(output_folder_s);
	os_mkdir(data->output_folder.data);

	data->layer = layer;

	callback->data = (void*)data;
	callback->data_is_valid = callback_dump_layer_neurons_data_is_valid;
	callback->data_destroy = callback_dump_layer_neurons_data_destroy;
	callback->data_update = callback_dump_layer_neurons_data_update;
	callback->data_run = callback_dump_layer_neurons_data_run;
	return callback;

ERROR
	// data->voltages_per_neuron && data->spikes_per_neuron are taken cared by array_of_arrays_init
	// and layer is not ours to free
	if (callback != NULL) free(callback);
	if (data->output_folder.data != NULL) string_reset(&(data->output_folder));
	if (data != NULL) free(data);
	return NULL;
}


BOOL callback_dump_layer_neurons_data_is_valid(C_Data* data) {
	check(data != NULL, null_argument("data"));
	check(string_is_valid(&(data->output_folder)) == TRUE, invalid_argument("data->output_folder"));
	check(layer_is_valid(data->layer) == TRUE, invalid_argument("data->layer"));
	check(array_is_valid(&(data->voltages_per_neuron)) == TRUE, invalid_argument("data->voltages_per_neuron"));
	check(array_is_valid(&(data->spikes_per_neuron)) == TRUE, invalid_argument("data->spikes_per_neuron"));
	check(array_is_valid(&(data->psc_per_neuron)) == TRUE, invalid_argument("data->PSC_per_neuron"));
	check(array_is_valid(&(data->epsc_per_neuron)) == TRUE, invalid_argument("data->EPSC_per_neuron"));
	check(array_is_valid(&(data->ipsc_per_neuron)) == TRUE, invalid_argument("data->IPSC_per_neuron"));

	return TRUE;
ERROR
	return FALSE;
}


void callback_dump_layer_neurons_data_destroy(C_Data* data) {
	check(callback_dump_layer_neurons_data_is_valid(data) == TRUE, invalid_argument("data"));
	string_reset(&(data->output_folder));
	data->layer = NULL;
	array_of_arrays_reset(&(data->voltages_per_neuron));
	array_of_arrays_reset(&(data->spikes_per_neuron));
	array_of_arrays_reset(&(data->psc_per_neuron));
	array_of_arrays_reset(&(data->epsc_per_neuron));
	array_of_arrays_reset(&(data->ipsc_per_neuron));
	free(data);

ERROR
	return;
}


void callback_dump_layer_neurons_data_update(C_Data* data, Network* net) {
	(net);
	check(callback_dump_layer_neurons_data_is_valid(data) == TRUE, invalid_argument("data"));
	ArrayFloat* voltages = layer_get_voltages(data->layer);
	ArrayFloat* pscs = layer_get_psc(data->layer);
	ArrayFloat* epscs = layer_get_epsc(data->layer);
	ArrayFloat* ipscs = layer_get_ipsc(data->layer);
	ArrayBool* spikes = layer_get_spikes(data->layer);
	ArrayFloat* inner_voltage = NULL;
	ArrayFloat* inner_psc = NULL;
	ArrayFloat* inner_epsc = NULL;
	ArrayFloat* inner_ipsc = NULL;
	ArrayBool* inner_spike = NULL;
	float* voltage_p = NULL;
	float* psc_p = NULL;
	float* epsc_p = NULL;
	float* ipsc_p = NULL;
	BOOL* spike_p = NULL;
	uint32_t i = 0;

	// save info about voltages and spikes
	for (i = 0; i < data->layer->neurons.length; ++i) {
		// get values for layer
		voltage_p = (float*)array_get(voltages, i);
		psc_p = (float*)array_get(pscs, i);
		epsc_p = (float*)array_get(epscs, i);
		ipsc_p = (float*)array_get(ipscs, i);
		spike_p = (BOOL*)array_get(spikes, i);

		// add them into their containers
		inner_voltage = (ArrayFloat*)array_get(&(data->voltages_per_neuron), i);
		inner_psc = (ArrayFloat*)array_get(&(data->psc_per_neuron), i);
		inner_epsc = (ArrayFloat*)array_get(&(data->epsc_per_neuron), i);
		inner_ipsc = (ArrayFloat*)array_get(&(data->ipsc_per_neuron), i);
		inner_spike = (ArrayBool*)array_get(&(data->spikes_per_neuron), i);
		
		array_append(inner_voltage, voltage_p);
		array_append(inner_psc, psc_p);
		array_append(inner_epsc, epsc_p);
		array_append(inner_ipsc, ipsc_p);
		array_append(inner_spike, spike_p);
	}
	array_destroy(voltages, NULL);
	array_destroy(pscs, NULL);
	array_destroy(epscs, NULL);
	array_destroy(ipscs, NULL);
	array_destroy(spikes, NULL);

ERROR
	return;
}


void callback_dump_layer_neurons_data_run(C_Data* data, Network* net) {
	(net);
	check(callback_dump_layer_neurons_data_is_valid(data) == TRUE, invalid_argument("data"));
	ArrayFloat* voltages = NULL;
	ArrayFloat* pscs = NULL;
	ArrayFloat* epscs = NULL;
	ArrayFloat* ipscs = NULL;
	ArrayBool* spikes = NULL;
	String* file_path = NULL;
	String* data_name = NULL;
	char file_name[128] = { 0 };
	uint32_t i = 0;

	// create 3 binaries per neuron, one for voltages one for currents one for spikes
	for (i = 0; i < data->layer->neurons.length; ++i) {
		
		pscs = (ArrayFloat*)array_get(&(data->psc_per_neuron), i);
		sprintf(file_name, "PSC_N%u.bin", i);
		file_path = string_path_join_string_and_C(&(data->output_folder), file_name);
		data_name = string_create("PSC");
		array_float_dump(pscs, file_path, data_name);
		string_destroy(file_path);
		string_destroy(data_name);

		epscs = (ArrayFloat*)array_get(&(data->epsc_per_neuron), i);
		sprintf(file_name, "EPSC_N%u.bin", i);
		file_path = string_path_join_string_and_C(&(data->output_folder), file_name);
		data_name = string_create("EPSC");
		array_float_dump(epscs, file_path, data_name);
		string_destroy(file_path);
		string_destroy(data_name);

		ipscs = (ArrayFloat*)array_get(&(data->ipsc_per_neuron), i);
		sprintf(file_name, "IPSC_N%u.bin", i);
		file_path = string_path_join_string_and_C(&(data->output_folder), file_name);
		data_name = string_create("IPSC");
		array_float_dump(ipscs, file_path, data_name);
		string_destroy(file_path);
		string_destroy(data_name);

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

ERROR
	return;
}