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

typedef void CallbackData;

typedef BOOL(*__callback_is_valid)(Callback* callback);
typedef void(*__callback_update)(Callback* callback, Network* net);
typedef void(*__callback_run)(Callback* callback, Network* net);
typedef void(*__callback_destroy)(Callback* callback);
typedef void(*__callback_reset)(Callback* callback);

struct Callback {
	CallbackData* data;				// callback dependent data
	__callback_is_valid is_valid;   // checks if callback is valid
	__callback_update update;		// updates the callback state
	__callback_run run;				// does something based on data gathered by callback
	__callback_destroy destroy;		// knows how to free the information of a given callback
	__callback_reset reset;			// knows hot to free the inner state of the callback
};

BOOL callback_is_valid(Callback* callback);
void callback_update(Callback* callback, Network* net);
void callback_run(Callback* callback, Network* net);
void callback_destroy(Callback* callback);
void callback_reset(Callback* callback);

#endif // __CALLBACK_H__
