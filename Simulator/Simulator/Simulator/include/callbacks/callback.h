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


// each callback needs an internal state, which can be anything
typedef void __CallbackData;

// check that the state variables are valid
typedef BOOL(*__callback_data_is_valid)(__CallbackData* data);

// clean the state variables specific to a callback
typedef void(*__callback_data_destroy)(__CallbackData* data);

// updates the internal state based on the current state of a network
// this should be used when the network suffers somekind of change, like every each timestamp
typedef void(*__callback_data_update)(__CallbackData* data, Network* net);

// does something meaningfull with the network or the interval state gathered so far, or with both
// like: saving the state of the newtwork to disk, or computing some kind of property of the network (ex: average activation)
typedef void(*__callback_data_run)(__CallbackData* data, Network* net);


struct Callback {
	__CallbackData* data;						// callback dependent data
	__callback_data_is_valid data_is_valid;		// checks if callback is valid
	__callback_data_destroy data_destroy;		// knows how to free the information of a given callback
	__callback_data_update data_update;			// updates the callback state
	__callback_data_run data_run;				// does something based on data gathered by callback
};


// the callback behave in the same way, so the general boiler plate code is inside of this functions
BOOL callback_is_valid(Callback* callback);
void callback_reset(Callback* callback);
void callback_destroy(Callback* callback);
void callback_update(Callback* callback, Network* net);
void callback_run(Callback* callback, Network* net);

#endif // __CALLBACK_H__
