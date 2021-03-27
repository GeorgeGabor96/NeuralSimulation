#ifndef __DATA_GEN_RANDOM_SPIKES__
#define __DATA_GEN_RANDOM_SPIKES__

#include "config.h"
#include "data/DataGen.h"

DataGenerator* data_generator_random_spikes_create(uint32_t n_examples, Network* net, float spikes_percent, uint32_t duration);


/*
* Every will receive one spike, first one at time 0, second ont at time @step_between_neurons, third at time 2 * @step_between_neurons, and so on
*/
DataGenerator* data_generator_with_step_between_neurons_create(uint32_t n_examples, Network* net, uint32_t step_between_neurons, uint32_t duration);


/*
* This will create a dataset of spikes
* For @between_pulses_duration it will create spikes with frequency @between_pulses_spike_frequency
* For @pulse_duration it will create spikes with frequency @pulse_spike_frequency
* First pulse will begin at timestamp @first_pulse_timestamp
*/
DataGenerator* data_generator_spike_pulses_create(
	uint32_t n_examples,
	Network* net, 
	uint32_t first_pulse_timestamp, 
	uint32_t between_pulses_duration,
	uint32_t pulse_duration,
	float between_pulses_spike_frequency, 
	float pulse_spike_frequency, 
	uint32_t duration);

#endif // __DATA_GEN_RANDOM_SPIKES__