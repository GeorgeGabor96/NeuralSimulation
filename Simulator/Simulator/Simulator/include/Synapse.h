#ifndef __SYNAPSE_H__
#define __SYNAPSE_H__


#include "config.h"
#include "Containers.h"

// TODO: Update documentation


/*************************************************************
* SynapseClass Functionality
*************************************************************/

typedef enum { INVALID_SYNAPSE = 0, CONDUCTANCE_SYNAPSE = 1, VOLTAGE_DEPENDENT_SYNAPSE = 2 } SynapseType;
const char* synapse_type_C_string(SynapseType type);


typedef struct SynapseClass {
	String* name;
	float E; // reversal potential
	float tau_exp; 
	uint32_t delay; // all spikes are delayed
	SynapseType type;
	// integration step -> don't know whats with that yet
} SynapseClass;


#define SYNAPSE_REV_POTENTIAL_DF 0.0f
#define SYNAPSE_TAU_MS_DF -1.0f / (float)log(0.5f)
#define SYNAPSE_DELAY_DF 1
#define SYNAPSE_TYPE_DF CONDUCTANCE_SYNAPSE
#define SYNAPSE_SIMULATION_TIME_MS_DF 1.0f

/*
* General function to verify that a synapse class is valid
* A @synapse_class is valid if:
* 1. @synapse_class != NULL
* 2. @synapse_class->type = CONDUCTANCE_SYNAPCE or @synapse_class->type = VOLTAGE_DEPENDENT_SYNAPSE
*/
Status synapse_class_is_valid(SynapseClass* synapse_class);

/*
* Parameters: 
* ----------
* rev_potential -
* tau_ms - time constant of the synapse (bigger will make the reduction in current slower)
* delay - difference between time when a spike is processed and when it arrives
* type - type of synapse, controls how current is computed
* simulation_step_ms - the number of miliseconds the integration is made, cumulated that many miliseconds in the upate
* 
* 
* Preconditions: @tau_ms > 0
* 			   @simulation_step_ms > 0
* 			   @type == CONDUCTANCE_SYNPASE || @type == VOLTAGE_DEPENDENT_SYNAPSE
*/
SynapseClass* synapse_class_create(const char* name, float rev_potential, float tau_ms, uint32_t delay, SynapseType type, float simulation_step_ms);

SynapseClass* synapse_class_create_default(const char* name);
void synapse_class_reset(SynapseClass* synapse_class);
void synapse_class_destroy(SynapseClass* synapse_class);


/*************************************************************
* Synapse Functionality
*************************************************************/

typedef struct Synapse {
	SynapseClass* s_class;
	float w; // synaptic weight
	float g; // synaptic conductance
	Queue spike_times;
} Synapse;

#define SYNAPSE_INITIAL_SPIKE_CAPACITY 10

/*
* General function to verify that a synapse is valid
* A @synapse is valid if:
* 1. @synapse != NULL
* 2. @synapse->s_class != NULL
* 3. @synapse->spike_times is valid
*/
Status synapse_is_valid(Synapse* synapse);
Status synapse_init(Synapse* synapse, SynapseClass* s_class, float w);
void synapse_reset(Synapse* synapse);
Synapse* synapse_create(SynapseClass* s_class, float w);
void synapse_destroy(Synapse* synapse);
void synapse_destroy_2p(Synapse** synapse);

/*
Preconditions: @spike_time > queue_head(synapse->spike_times) - should never receive a spike older that I already have
*/
Status synapse_add_spike_time(Synapse* synapse, uint32_t spike_time);

float synapse_compute_PSC(Synapse* synapse, float u);

Status synapse_step(Synapse* synapse, uint32_t simulation_time);

// empties the queue of spikes and resets the conductance
void synapse_clear_state(Synapse* synapse);


#endif  // __SYNAPSE_H__