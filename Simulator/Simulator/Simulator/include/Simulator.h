#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__


#include "config.h"
#include "Network.h"
#include "data/DataGen.h"
#include "callbacks/callback.h"


typedef struct Simulator {
	DataGenerator* data;	 // takes ownership, will generate data
	Network* network;		 // takes ownership?? -> Currently YES
	ArrayCallback callbacks; // takes ownershp over every callback, Are added one by one
} Simulator;


BOOL simulator_is_valid(Simulator* sim);
Simulator* simulator_create(DataGenerator* data, Network* net);
void simulator_destroy(Simulator* sim);
Status simulator_add_callback(Simulator* sim, Callback* call);
void simulator_infer(Simulator* sim);
void simulator_train(Simulator* sim);
void simulator_test(Simulator* sim);


#endif // __SIMULATOR_H__
