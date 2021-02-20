#ifndef __CALLBACK_H__
#define __CALLBACK_H__

#include "config.h"
#include "Network.h"

/****************************************************
* INFERFACE FOR CALLBACK
* EACH CALLBACK NEEDS TO FOLLOW THE CALLBACK STRUCT
****************************************************/


struct Callback;
typedef struct Callback Callback;

// need the network to get outputs and save them in the callback structure
// because I need to plot at the end everythong
typedef void(*__callback_update)(Callback* callback, Network* net);
typedef void(*__callback_run)(Callback* callback, Network* net);
typedef void(*__callback_destroy)(Callback* callback);

/*
Callbacks are nice and easy if they are objects because they have a state that is dependent of class
ex: plotting vs evaluation maybe

And they need function, write?

so we need some kind of generic structure that keeps the functions and data for a callback
and every callback needs to respect that interface -> polymorphism

So, not every callback will need a given function, like a model save callback, that only needs the run
1. Do I make like a type for callbacks that like only does a subset of things -> maybe its complicated and confusing + a lot of types
2. Or I can could just leave the pointer NULL for that function??? -> just need to verify and it would work nicely and only run what needs to 
be runned for a callback
3. Or I could force an implementation for every function, but if I do that when a new callback needs some aditional function
i need to add that for every function

I think I will use the second way because its the easiest to mantain

*/

typedef void CallbackData;

struct Callback {
	CallbackData* data;				// callback dependent data
	__callback_update update;		// updates the callback state
	__callback_run run;				// does something based on data gathered by callback
	__callback_destroy destroy;		// knows how to free the information of a given callback
};

#define callback_is_valid(callback) ((callback) == NULL ? FALSE : TRUE)

#define callback_update(callback, net) ((callback)->update((callback), (net)))
#define callback_run(callback, net) ((callback)->run((callback), (net)))
#define callback_destroy(callback) ((callback)->destroy((callback)))

#endif // __CALLBACK_H__
