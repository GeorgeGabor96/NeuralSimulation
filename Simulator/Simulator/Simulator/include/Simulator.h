#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "Network.h"


struct Simulator {
	// DataGenerator* data_gen; // will generate data
	Network* network;   // will process data
	// Callbacks* callbacks; // some postprocessing on data
};

#endif // __SIMULATOR_H__
