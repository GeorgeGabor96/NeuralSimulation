#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "config.h"
#include "Network.h"

/****************************************************
* INFERFACE FOR CALLBACK
* 
* EACH CALLBACK NEEDS TO FOLLOW THE CALLBACK STRUCT
* TO ALLOW POLYMORPHISM
****************************************************/

struct Callback;
typedef struct Callback Callback;
typedef Array ArrayCallback;

typedef void __CallbackData;

typedef BOOL(*__callback_data_is_valid)(__CallbackData* data);
typedef void(*__callback_data_destroy)(__CallbackData* data);
typedef void(*__callback_data_update)(__CallbackData* data, Network* net);
typedef void(*__callback_data_run)(__CallbackData* data, Network* net);

struct Callback {
	__CallbackData* data;				// callback dependent data
	__callback_data_is_valid data_is_valid;   // checks if callback is valid
	__callback_data_destroy data_destroy;		// knows how to free the information of a given callback
	__callback_data_update data_update;		// updates the callback state
	__callback_data_run data_run;				// does something based on data gathered by callback
};


BOOL callback_is_valid(Callback* callback);
void callback_reset(Callback* callback);
void callback_destroy(Callback* callback);
void callback_update(Callback* callback, Network* net);
void callback_run(Callback* callback, Network* net);

#endif // __CALLBACK_H__
