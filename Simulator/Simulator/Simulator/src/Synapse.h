#ifndef __SYNAPSE_H__
#define __SYNAPSE_H__

#include "Containers.h"
#include "debug.h"
// TO DO: a synapse should have a pointer to a header that defines its class properties
// TO DO: a synapse should keep in order the spikes it received


/*************************************************************
* SynapseClass Functionality
*************************************************************/

typedef enum { CONDUCTANCE_SYNAPCE, VOLTAGE_DEPENDENT_SYNAPSE } SynapseType;


typedef struct SynapseClass {
	float E; // reversal potential
	float tau_exp; 
	uint32_t delay; // all spikes are delayed
	SynapseType type;
	// integration step -> don't know whats with that yet
} SynapseClass;

/*
Preconditions: @tau_ms > 0
			   @simulation_step_ms > 0
			   @type == CONDUCTANCE_SYNPASE || @type == VOLTAGE_DEPENDENT_SYNAPSE
*/
SynapseClass* synapse_class_create(float rev_potential, float tau_ms, uint32_t delay, SynapseType type, float simulation_step_ms);

/*
Preconditions: @synapse_class != NULL
*/
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
Preconditions: @s_class != NULL
*/
Synapse* synapse_create(SynapseClass* s_class, float w);

/*
Preconditions: @synapse != NULL 
			   @synapse->s_class != NULL
			   @synapse->spike_times != NULL
*/
void synapse_destroy(Synapse* synapse);

/*
Preconditions: @synapse != NULL
			   @synapse->s_class != NULL
			   @synapse->spike_times != NULL
*/
Status synapse_add_spike_time(Synapse* synapse, uint32_t spike_time);

/*
Preconditions: @synapse != NULL
			   @synapse->s_class != NULL
			   @synapse->spike_times != NULL

Maybe add some u checks (if its in a expected range)
*/
float synapse_compute_PSC(Synapse* synapse, float u);

/*
Preconditions: @synapse != NULL
			   @synapse->s_class != NULL
			   @synapse->spike_times != NULL
*/
Status synapse_step(Synapse* synapse, uint32_t simulation_time);

#endif  // __SYNAPSE_H__