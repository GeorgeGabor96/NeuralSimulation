#include "callbacks/callback_dump.h"

#include "utils/MemoryManagement.h"
#include "Containers.h"


typedef struct C_Data {
	Array callbacks_layers;
} C_Data;


BOOL callback_dump_network_data_is_valid(C_Data* data);
void callback_dump_network_data_destroy(C_Data* data);
void callback_dump_network_data_update(C_Data* data, Network* net);
void callback_dump_network_data_run(C_Data* data, Network* net);


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
		callback = callback_dump_layer_neurons_create(layer, output_folder);
		array_append(&(data->callbacks_layers), callback);
		free(callback);
	}

	callback = (Callback*)calloc(1, sizeof(Callback), "callback_dump_network_create  callback");
	check_memory(callback);
	callback->data = data;
	callback->data_is_valid = callback_dump_network_data_is_valid;
	callback->data_destroy = callback_dump_network_data_destroy;
	callback->data_update = callback_dump_network_data_update;
	callback->data_run = callback_dump_network_data_run;

	return callback;

ERROR
	if (callback != NULL) free(callback);
	if (data != NULL) {
		for (i = 0; i < data->callbacks_layers.length; ++i) {
			callback = (Callback*)array_get(&(data->callbacks_layers), i);
			callback_reset(callback);
		}
		free(data);
	}

	return NULL;
}


BOOL callback_dump_network_data_is_valid(C_Data* data) {
	check(array_is_valid(&(data->callbacks_layers)) == TRUE, invalid_argument("data->callbacks_layers"));
	Callback* callback = NULL;
	for (uint32_t i = 0; i < data->callbacks_layers.length; ++i) {
		callback = (Callback*)array_get(&(data->callbacks_layers), i);
		check(callback_is_valid(callback) == TRUE, invalid_argument("callback"));
	}
	return TRUE;

ERROR
	return FALSE;
}


void callback_dump_network_data_destroy(C_Data* data) {
	check(callback_dump_network_data_is_valid(data) == TRUE, invalid_argument("data"));
	uint32_t i = 0;
	Callback* callback = NULL;

	array_reset(&(data->callbacks_layers), callback_reset);
	free(data);

ERROR
	return;
}


void callback_dump_network_data_update(C_Data* data, Network* net) {
	check(callback_dump_network_data_is_valid(data) == TRUE, invalid_argument("data"));
	Callback* callback = NULL;
	uint32_t i = 0;

	for (i = 0; i < data->callbacks_layers.length; ++i) {
		callback = (Callback*)array_get(&(data->callbacks_layers), i);
		callback_update(callback, net);
	}

ERROR
	return;
}


void callback_dump_network_data_run(C_Data* data, Network* net) {
	check(callback_dump_network_data_is_valid(data) == TRUE, invalid_argument("data"));
	Callback* callback = NULL;
	uint32_t i = 0;

	for (i = 0; i < data->callbacks_layers.length; ++i) {
		callback = (Callback*)array_get(&(data->callbacks_layers), i);
		callback_run(callback, net);
	}

ERROR
	return;
}


