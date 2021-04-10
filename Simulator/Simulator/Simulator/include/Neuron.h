#ifndef __NEURON_H__
#define __NEURON_H__


#include "config.h"
#include "Containers.h"
#include "Synapse.h"


/*************************************************************
* NeuronClass Functionality
*************************************************************/

typedef enum { INVALID_NEURON = 0, LIF_NEURON = 1, LIF_REFRACTORY_NEURON = 2} NeuronType;
const char* neuron_type_C_string(NeuronType type);


typedef struct NeuronClass {
	String* name;
	NeuronType type;
	
	// TODO: do we need a u_reset for when we have spikes??? ask Raul, watch video
	//			because we reset at u_rest

	// NOTE: if you add more types of neurons consider using a UNION
	// LIF neuron
	float u_th;			// membrane threshold potential
	float u_rest;		// membrane reset potential
	float tau;			// membrane time constant = r * c
	float u_factor;		// = 1 - 1 / (r * c)
	float i_factor;		// = 1 / c
	float free_factor;	// = u_rest / (r * c)

	// LIF neuron with Refractory
	uint32_t refractory_time;	
} NeuronClass;

// default values
#define LIF_U_TH	-45.0f
#define LIF_U_REST	-65.0f 
#define LIF_R		10.0f
#define LIF_C		1.0f
#define LIF_REFRACT (uint32_t)5

/*
1. neuron_class != NULL
2. neuron_class->type is a valid one
*/
Status neuron_class_is_valid(NeuronClass* neuron_class);
NeuronClass* neuron_class_create(const char* name, NeuronType type);
void neuron_class_reset(NeuronClass* neuron_class);
void neuron_class_destroy(NeuronClass* neuron_class);
void neuron_class_ref_destroy(NeuronClass** neuron_class);
Status neuron_class_set_LIF_parameters(NeuronClass* neuron_class, float u_th, float u_rest, float r, float c);
Status neuron_class_set_LIF_refractor_parameters(NeuronClass* neuron_class, float u_th, float u_rest, float r, float c, uint32_t refract_time);


/*************************************************************
* Neuron Functionality
*************************************************************/

typedef struct Neuron {
	NeuronClass* n_class;
	Array in_synapses_refs;	  // references to input synapses, has ownership
	Array out_synapses_refs;  // references to output synapses
	float u;
	float PSC;		 		  // the PSC value that determined the current @u and @spike values
	uint32_t last_spike_time;	// for neurons with refractory period
	BOOL spike;
} Neuron;


#define NEURON_INITIAL_SYNAPSE_LENGTH 10u

/*
General function to verify that a neuron is valid
A @neuron is valid if:
1. @neuron != NULL
2. @neuron->n_class is valid
3. @neuron->in_synapses_refs is valid
4. @neuron->out_synapses_refs is valid
*/
Status neuron_is_valid(Neuron* neuron);
Status neuron_init(Neuron* neuron, NeuronClass* neuron_class);
void neuron_reset(Neuron* neuron);
Neuron* neuron_create(NeuronClass* neuron_class);
void neuron_destroy(Neuron* neuron);

// The input synapses are only kept as references, but the neuron is responsible for freeing them
Status neuron_add_in_synapse(Neuron* neuron, Synapse* synapse);

// The ouput synapses are only kept as references
Status neuron_add_out_synapse(Neuron* neuron, Synapse* synapse);

// remove the spike of the neuron, resets its voltage and clears the state of its input synapses
void neuron_clear_state(Neuron* neuron);

/*
This will collect the PSC from the input synpases, update the neuron state,
and if a spike is generated, it will be propagated to the output synapses
*/
Status neuron_step(Neuron* neuron, uint32_t simulation_time);

/*
* Similar to @neuron_step, but it only propagates a spike through the output synapses
*/
Status neuron_step_force_spike(Neuron* neuron, uint32_t simulation_time);


/*
* Similar to @neuron_step, but usses received PSC instead of collecting it from the 
* input synapses
*/
Status neuron_step_inject_current(Neuron* neuron, float PSC, uint32_t simulation_time);

size_t neuron_get_min_byte_size(Neuron* neuron);

#endif // __NEURON_H__
