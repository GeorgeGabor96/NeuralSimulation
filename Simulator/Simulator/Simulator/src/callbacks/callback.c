#include "callbacks/callback.h"


BOOL callback_is_valid(Callback* callback) {
	if (callback->is_valid != NULL)
		return callback->is_valid(callback);
	return FALSE;
}

void callback_update(Callback* callback, Network* net) {
	if (callback->update != NULL)
		callback->update(callback, net);
}

void callback_run(Callback* callback, Network* net) {
	if (callback->run != NULL)
		callback->run(callback, net);
}

void callback_destroy(Callback* callback) {
	if (callback->destroy != NULL)
		callback->destroy(callback);
}

void callback_reset(Callback* callback) {
	if (callback->reset != NULL)
		callback->reset(callback);
}
