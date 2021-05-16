#ifndef __EXPERIMENTS_H__
#define __EXPERIMENTS_H__

#include "config.h"

void constant_current_experiment();
void constant_current_learning_experiment();


void random_spikes_experiment();
void time_between_spikes_experiment();


// synfire chians
typedef struct connectivity_amplitude_space_exp {
	float connectivity_start;
	float connectivity_end;
	float connectivity_inc;

	float amplitude_start;
	float amplitude_end;
	float amplitude_inc;
	BOOL use_refract;

	float min_ratio;
	float max_ratio;
}connectivity_amplitude_space_exp;

void synfire_space_exploration_connectivity_amplitude_multiple(uint32_t n_trials);
void synfire_chain_space_mapping_connectivity_and_amplitude(connectivity_amplitude_space_exp* config, const char* result_path);
void synfire_chain_space_mapping_connectivity_and_amplitude_specific(float connectivity, float amplitude, BOOL use_refract);

#endif // __EXPERIMENTS_H__