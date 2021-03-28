#include "callbacks/callback_dump.h"

#include "utils/MemoryManagement.h"
#include "Containers.h"


BOOL callback_dump_network_is_valid(Callback* callback);
void callback_dump_network_update(Callback* callback, Network* net);
void callback_dump_network_run(Callback* callback, Network* net);
void callback_dump_network_reset(Callback* callback);
void callback_dump_network_destroy(Callback* callback);


typedef struct C_Data {
	Array callbacks_layers;
} C_Data;


Callback* callback_dump_network_create(Network* net, const char* output_folder) {
	Callback* callback = NULL;
	C_Data* data = NULL;
	Layer* layer = NULL;
	Status status = FAIL;
	uint32_t i = 0;

	check(network_is_valid(net) == TRUE, invalid_argument("net"));

	data = (C_Data*)malloc(sizeof(C_Data), "callback_dump_network_create data");
	check_memory(data);

	// init the data
	status = array_init(&(data->callbacks_layers), net->layers.length, 0, sizeof(Callback));
	check(status == SUCCESS, init_argument("data->callbacks_layers"));
	for (i = 0; i < net->layers.length; ++i) {
		layer = network_get_layer_by_idx(net, i);
		callback = callback_dump_layer_neurons_create(layer, output_folder, FALSE);
		array_append(&(data->callbacks_layers), callback);
		free(callback);
	}

	callback = (Callback*)calloc(1, sizeof(Callback), "callback_dump_network_create  callback");
	check_memory(callback);
	callback->data = data;
	callback->is_valid = callback_dump_network_is_valid;
	callback->update = callback_dump_network_update;
	callback->run = callback_dump_network_run;
	callback->reset = callback_dump_network_reset;
	callback->destroy = callback_dump_network_destroy;

	return callback;

ERROR
	if (callback != NULL) free(callback);
	if (data != NULL) {
		for (i = 0; i < data->callbacks_layers.length; ++i) {
			callback = (Callback*)array_get(&(data->callbacks_layers), i);
			callback->reset(callback);
		}
		free(data);
	}

	return NULL;
}


BOOL callback_dump_network_is_valid(Callback* callback) {
	// TO DO: refactor
	Callback* layer_callback = NULL;
	C_Data* data = (C_Data*)(callback->data);
	check(array_is_valid(&(data->callbacks_layers)) == TRUE, invalid_argument("data->callbacks_layers"));
	for (uint32_t i = 0; i < data->callbacks_layers.length; ++i) {
		layer_callback = (Callback*)array_get(&(data->callbacks_layers), i);
		check(layer_callback->is_valid(layer_callback) == TRUE, invalid_argument("layer_callback"));
	}
	return TRUE;

ERROR
	return FALSE;
}


void callback_dump_network_update(Callback* callback, Network* net) {
	check(callback_dump_network_is_valid(callback) == TRUE, invalid_argument("callback"));
	C_Data* data = (C_Data*)(callback->data);
	Callback* layer_callback = NULL;
	uint32_t i = 0;

	for (i = 0; i < data->callbacks_layers.length; ++i) {
		layer_callback = (Callback*)array_get(&(data->callbacks_layers), i);
		layer_callback->update(layer_callback, net);
	}

ERROR
	return;
}


void callback_dump_network_run(Callback* callback, Network* net) {
	check(callback_dump_network_is_valid(callback) == TRUE, invalid_argument("callback"));
	C_Data* data = (C_Data*)(callback->data);
	Callback* layer_callback = NULL;
	uint32_t i = 0;

	for (i = 0; i < data->callbacks_layers.length; ++i) {
		layer_callback = (Callback*)array_get(&(data->callbacks_layers), i);
		layer_callback->run(layer_callback, net);
	}

ERROR
	return;
}


void callback_dump_network_reset(Callback* callback) {
	check(callback_dump_network_is_valid(callback) == TRUE, invalid_argument("callback"));
	C_Data* data = (C_Data*)(callback->data);
	uint32_t i = 0;
	Callback* layer_callback = NULL;
	
	for (i = 0; i < data->callbacks_layers.length; ++i) {
		layer_callback = (Callback*)array_get(&(data->callbacks_layers), i);
		layer_callback->reset(layer_callback);
	}
	array_reset(&(data->callbacks_layers), NULL);
	free(data);

	// clera memory refactor

ERROR
	return;
}


void callback_dump_network_destroy(Callback* callback) {
	check(callback_dump_network_is_valid(callback) == TRUE, invalid_argument("callback"));
	callback_dump_network_reset(callback);
	free(callback);

ERROR
	return;
}