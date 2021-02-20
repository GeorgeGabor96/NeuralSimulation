#include "callbacks/callback.h"


BOOL callback_is_valid(Callback* callback) {
	check(callback != NULL, null_argument("callback"));
	check(callback->destroy != NULL, null_argument("callback"));
	return TRUE;
ERROR
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
