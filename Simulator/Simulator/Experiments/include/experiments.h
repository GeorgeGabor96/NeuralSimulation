#ifndef __EXPERIMENTS_H__
#define __EXPERIMENTS_H__

#include "config.h"

void constant_current_experiment();
void constant_current_learning_experiment();


void random_spikes_experiment();
void time_between_spikes_experiment();


// synfire chians
void synfire_space_exploration_connectivity_amplitude();


// cortical slabs
void cortical_slabs_exp_run();
void cortical_slabs_exp_growing_net_run();

// embedded synfire chains
void embedded_synfire_chains_exp();

#endif // __EXPERIMENTS_H__