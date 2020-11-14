#ifndef __NEURON_H__
#define __NEURON_H__

#include "Containers.h"
#include "Synapse.h"


/*************************************************************
* NeuronClass Functionality
*************************************************************/

typedef enum { LIF_NEURON = 0 } NeuronType;

typedef struct NeuronClass {
	NeuronType type;
	
	// TODO: Union if considering multiple types of neurons
	// TODO: do we need a u_reset for when we have spikes??? ask Raul, watch video
	// LIF neuron
	float u_th;		// membrane threshold potential
	float u_rest;	// membrane reset potential
	float tau;		// membrane time constant = r * c
	float u_factor;		// = 1 - 1 / (r * c)
	float i_factor;		// = 1 / c
	float free_factor;	// = u_rest / (r * c)
} NeuronClass;

#define LIF_U_TH	-45.0f
#define LIF_U_REST	-65.0f 
#define LIF_R		10.0f
#define LIF_C		1.0f

/*
Preconditions:	@type = LIF_NEURON
*/
NeuronClass* neuron_class_create(NeuronType type);

/*
Preconditions:	@neuron_class != NULL
*/
void neuron_class_destroy(NeuronClass* neuron_class);

/*
Preconditions:	@neuron_class != NULL
*/
Status neuron_class_set_LIF_parameters(NeuronClass* neuron_class, float u_th, float u_rest, float r, float c);


/*************************************************************
* Neuron Functionality
*************************************************************/

typedef struct Neuron {
	NeuronClass* n_class;
	float u;
	uint32_t spike;
	Vector* in_synapses_refs; // pointers to the INPUT synapses
	Vector* out_synapses_refs; // pointers to the OUTPUT synapses
} Neuron;

#define NEURON_INITIAL_SYNAPSE_LENGHT 10u

/*
Preconditions:	@neuron_class != NULL
*/
Neuron* neuron_create(NeuronClass* neuron_class);

/*
Preconditions:	@neuron != NULL
				@neuron->in_synapses != NULL
				@neuron->out_synapses != NULL
*/
void neuron_destroy(Neuron* neuron);

/*
Preconditions:	@neuron != NULL
				@neuron->in_synapses != NULL
				@neuron->out_synapses != NULL
				@synapse != NULL
*/
Status neuron_add_in_synapse(Neuron* neuron, Synapse* synapse);

/*
Preconditions:	@neuron != NULL
				@neuron->in_synapses != NULL
				@neuron->out_synapses != NULL
				@synapse != NULL
*/
Status neuron_add_out_synapse(Neuron* neuron, Synapse* out_synapse);

/*
Preconditions:	@neuron != NULL
				@neuron->in_synapses != NULL
				@neuron->out_synapses != NULL
*/
Status neuron_step(Neuron* neuron, uint32_t simulation_time);


#endif // __NEURON_H__
