#ifndef __SYNAPSE_H__
#define __SYNAPSE_H__

#include "Containers.h"
#include "debug.h"

// TODO: Update documentation


/*************************************************************
* SynapseClass Functionality
*************************************************************/

typedef enum { CONDUCTANCE_SYNAPCE = 0, VOLTAGE_DEPENDENT_SYNAPSE = 1 } SynapseType;


typedef struct SynapseClass {
	float E; // reversal potential
	float tau_exp; 
	uint32_t delay; // all spikes are delayed
	SynapseType type;
	// integration step -> don't know whats with that yet
} SynapseClass;


#define SYNAPSE_REV_POTENTIAL_DF 0.0f
#define SYNAPSE_TAU_MS_DF -1.0f / (float)log(0.5f)
#define SYNAPSE_DELAY_DF 1
#define SYNAPSE_TYPE_DF CONDUCTANCE_SYNAPCE
#define SYNAPSE_SIMULATION_TIME_MS_DF 1.0f

/*
* General function to verify that a synapse class is valid
* A @synapse_class is valid if:
* 1. @synapse_class != NULL
* 2. @synapse_class->type = CONDUCTANCE_SYNAPCE or @synapse_class->type = VOLTAGE_DEPENDENT_SYNAPSE
*/
Status synapse_class_is_valid(SynapseClass* synapse_class);

/*
Preconditions: @tau_ms > 0
			   @simulation_step_ms > 0
			   @type == CONDUCTANCE_SYNPASE || @type == VOLTAGE_DEPENDENT_SYNAPSE
*/
SynapseClass* synapse_class_create(float rev_potential, float tau_ms, uint32_t delay, SynapseType type, float simulation_step_ms);

SynapseClass* synapse_class_create_default(); 

void synapse_class_destroy(SynapseClass* synapse_class);


/*************************************************************
* Synapse Functionality
*************************************************************/

typedef struct Synapse {
	SynapseClass* s_class;
	float w; // synaptic weight
	float g; // synaptic conductance
	Queue* spike_times;
} Synapse;

#define SYNAPSE_INITIAL_SPIKE_CAPACITY 10

/*
* General function to verify that a synapse is valid
* A @synapse is valid if:
* 1. @synapse != NULL
* 2. @synapse->s_class != NULL
* 3. @synapse->spike_times != NULL
*/
Status synapse_is_valid(Synapse* synapse);

Status synapse_init(Synapse* synapse, SynapseClass* s_class, float w);

void synapse_reset(Synapse* synapse);

Synapse* synapse_create(SynapseClass* s_class, float w);

void synapse_destroy(Synapse* synapse);

/*
Preconditions: @spike_time > queue_head(synapse->spike_times) - should never receive a spike older that I already have
*/
Status synapse_add_spike_time(Synapse* synapse, uint32_t spike_time);

float synapse_compute_PSC(Synapse* synapse, float u);

Status synapse_step(Synapse* synapse, uint32_t simulation_time);

#endif  // __SYNAPSE_H__