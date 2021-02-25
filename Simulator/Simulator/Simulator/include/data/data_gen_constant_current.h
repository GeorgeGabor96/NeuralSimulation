#ifndef __DATA_GEN_CONSTANT_CURRENT__
#define __DATA_GEN_CONSTANT_CURRENT__

#include "config.h"
#include "data/DataGen.h"

DataGenerator* data_generator_constant_current_create(uint32_t length, Network* net, float current_value, uint32_t duration);

#endif // __DATA_GEN_CONSTANT_CURRENT__