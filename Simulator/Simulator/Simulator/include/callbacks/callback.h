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

typedef BOOL(*__callback_is_valid)(Callback* callback);
typedef void(*__callback_update)(Callback* callback, Network* net);
typedef void(*__callback_run)(Callback* callback, Network* net);
typedef void(*__callback_destroy)(Callback* callback);
typedef void(*__callback_reset)(Callback* callback);

struct Callback {
	__CallbackData* data;				// callback dependent data
	__callback_is_valid is_valid;   // checks if callback is valid
	__callback_update update;		// updates the callback state
	__callback_run run;				// does something based on data gathered by callback
	__callback_destroy destroy;		// knows how to free the information of a given callback
	__callback_reset reset;			// knows hot to free the inner state of the callback
};


#define callback_is_valid(callback) ((callback) != NULL && (callback)->is_valid != NULL ? (callback)->is_valid(callback) : FALSE)
#define callback_update(callback, net) ((callback) != NULL && (callback)->update != NULL ? (callback)->update(callback, net) : 0)
#define callback_run(callback, net) ((callback) != NULL && (callback)->run != NULL ? (callback)->run(callback, net) : 0)
#define callback_destroy(callback) ((callback) != NULL && (callback)->destroy != NULL ? (callback)->destroy(callback) : 0)
#define callback_reset(callback) ((callback) != NULL && (callback)->reset != NULL ? (callback)->reset(callback) : 0)

#endif // __CALLBACK_H__
