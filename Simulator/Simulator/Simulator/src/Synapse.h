#ifndef __SYNAPSE_H__
#define __SYNAPSE_H__

#include "Containers.h"
#include "debug.h"
// TO DO: a synapse should have a pointer to a header that defines its class properties
// TO DO: a synapse should keep in order the spikes it received


typedef enum { CONDUCTANCE_SYNAPCE, VOLTAGE_DEPENDENT_SYNAPSE } SynapseType;


typedef struct SynapticClass {
	float e; // reversal potential
	float tau_exp; 
	uint32_t delay; // all spikes are delayed
	SynapseType type;
	// integration step -> don't know whats with that yet
} SynapticClass;

/*
Preconditions: @tau_ms > 0
			   @simulation_step_ms > 0
TODO: should simulation_step_ms be a float???
*/
SynapticClass* synaptic_class_create(float rev_potential, float tau_ms, uint32_t delay, SynapseType type, float simulation_step_ms);
void synaptic_class_destroy(SynapticClass* synaptic_class);


typedef struct Synapse {
	SynapticClass* s_class;
	float w; // synaptic wieght
	float g; // synaptic conductance
	Array* spike_times;
} Synapse;

#define SYNAPSE_INITIAL_SPIKE_CAPACITY 10


// s_class != NULL
Synapse* synapse_create(SynapticClass* s_class, float w);

// synapse != NULL, synapse->s_class != NULL, synapse->spike_times != NULL
void synapse_destroy(Synapse* synapse);

// synapse != NULL, synapse->s_class != NULL, synapse->spike_times != NULL
Status synapse_add_spike_time(Synapse* synapse, uint32_t spike_time);

float synapse_compute_psc(float u); // here is I = g * (u - e) * w * A for VOLTAGE dependent
                           // I = g * w * A for CONDUCTANCE only

// in cod Raul g este intre [0, 1] pt un skipe, numulativ e mai mult

// g_syn cu bara cred ca e 1 in cazul lui Raul si daca vrei mai mult ai A ul ala din synaptic class

Status synapse_step(Synapse* synapse, uint32_t simulation_time);


#endif  // __SYNAPSE_H__