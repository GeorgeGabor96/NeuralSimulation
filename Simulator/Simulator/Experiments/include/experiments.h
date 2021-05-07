#ifndef __EXPERIMENTS_H__
#define __EXPERIMENTS_H__

#include "config.h"

void constant_current_experiment();
void constant_current_learning_experiment();


void random_spikes_experiment();
void time_between_spikes_experiment();


// synfire chians
void synfire_chain_space_mapping_connectivity_and_amplitude();
void synfire_chain_space_mapping_connectivity_and_amplitude_specific(float connectivity, float amplitude, BOOL use_refract);

#endif // __EXPERIMENTS_H__