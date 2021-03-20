#ifndef __DATA_GEN_RANDOM_SPIKES__
#define __DATA_GEN_RANDOM_SPIKES__

#include "config.h"
#include "data/DataGen.h"

DataGenerator* data_generator_random_spikes_create(uint32_t n_examples, Network* net, float spikes_percent, uint32_t duration);


/*
* Every will receive one spike, first one at time 0, second ont at time @step_between_neurons, third at time 2 * @step_between_neurons, and so on
*/
DataGenerator* data_generator_with_step_between_neurons_create(uint32_t n_examples, Network* net, uint32_t step_between_neurons, uint32_t duration);

#endif // __DATA_GEN_RANDOM_SPIKES__