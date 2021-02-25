#ifndef __DATA_GEN_RANDOM_SPIKES__
#define __DATA_GEN_RANDOM_SPIKES__

#include "config.h"
#include "data/DataGen.h"

DataGenerator* data_generator_random_spikes_create(uint32_t n_examples, Network* net, float spikes_percent, uint32_t duration);

#endif // __DATA_GEN_RANDOM_SPIKES__