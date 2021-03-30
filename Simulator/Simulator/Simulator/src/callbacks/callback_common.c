#include "callbacks/callback.h"
#include "utils/MemoryManagement.h"


BOOL callback_is_valid(Callback* callback) {
	check(callback != NULL, null_argument("callback"));
	check(callback->data != NULL, null_argument("callback->data"));
	check(callback->data_is_valid != NULL, null_argument("callback->data_is_valid"));
	check(callback->data_destroy != NULL, null_argument("callback->data_destroy"));
	check(callback->data_update != NULL, null_argument("callback->data_update"));
	check(callback->data_run != NULL, null_argument("callback->data_run"));

	__CallbackData* data = (__CallbackData*)callback->data;
	check(callback->data_is_valid(data) == TRUE, invalid_argument("callback->data"));

	return TRUE;
ERROR
	return FALSE;
}


void callback_reset(Callback* callback) {
	check(callback_is_valid(callback) == TRUE, invalid_argument("callback"));
	__CallbackData* data = (__CallbackData*)callback->data;
	callback->data_destroy(data);

	callback->data = NULL;
	callback->data_is_valid = NULL;
	callback->data_destroy = NULL;
	callback->data_update = NULL;
	callback->data_run = NULL;

ERROR
	return;
}

void callback_destroy(Callback* callback) {
	check(callback_is_valid(callback) == TRUE, invalid_argument("callback"));
	callback_reset(callback);
	free(callback);

ERROR
	return;
}


void callback_update(Callback* callback, Network* net) {
	check(callback_is_valid(callback) == TRUE, invalid_argument("callback"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	__CallbackData* data = (__CallbackData*)callback->data;
	callback->data_update(data, net);

ERROR
	return;
}


void callback_run(Callback* callback, Network* net) {
	check(callback_is_valid(callback) == TRUE, invalid_argument("callback"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	__CallbackData* data = (__CallbackData*)callback->data;
	callback->data_run(data, net);

ERROR
	return;
}