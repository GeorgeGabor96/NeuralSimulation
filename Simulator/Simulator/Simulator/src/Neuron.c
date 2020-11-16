#include "Neuron.h"

#include <stdlib.h>


/*************************************************************
* NeuronClass Functionality
*************************************************************/

NeuronClass* neuron_class_create(NeuronType type) {
	NeuronClass* neuron_class = NULL;
	check(type == LIF_NEURON, "@type has invalid value");
	
	neuron_class = (NeuronClass*)malloc(sizeof(NeuronClass));
	check_memory(neuron_class);

	neuron_class->type = type;

	switch (neuron_class->type)
	{
	case LIF_NEURON:
		neuron_class_set_LIF_parameters(neuron_class, LIF_U_TH, LIF_U_REST, LIF_R, LIF_C);
		break;
	default:
		log_error("Unkown neuron type");
		break;
	}

	return neuron_class;

error:
	if (neuron_class != NULL) {
		free(neuron_class);
	}
	return NULL;
}


void neuron_class_destroy(NeuronClass* neuron_class) {
	check(neuron_class != NULL, "NULL value for @neuron_class");

	free(neuron_class);
error:
	return;
}


Status neuron_class_set_LIF_parameters(NeuronClass* neuron_class, float u_th, float u_rest, float r, float c) {
	Status status = FAIL;
	check(neuron_class != NULL, "NULL value for @neuron_class");

	neuron_class->tau = r * c;
	neuron_class->u_factor = 1.0f - 1.0f / (r * c);
	neuron_class->i_factor = 1.0f / c;
	neuron_class->free_factor = u_rest / (r * c);
	neuron_class->u_th = u_th;
	neuron_class->u_rest = u_rest;

	status = SUCCESS;

error:
	return status;
}


/*************************************************************
* Neuron Functionality
*************************************************************/

Neuron* neuron_create(NeuronClass* neuron_class) {
	Neuron* neuron = NULL;
	check(neuron_class != NULL, "NULL value for @neuron_class");

	neuron = (Neuron*)malloc(sizeof(Neuron));
	check_memory(neuron);

	neuron->in_synapses_refs = vector_create(NEURON_INITIAL_SYNAPSE_LENGHT, sizeof(Synapse*));
	check_memory(neuron->in_synapses_refs);

	neuron->out_synapses_refs = vector_create(NEURON_INITIAL_SYNAPSE_LENGHT, sizeof(Synapse*));
	check_memory(neuron->out_synapses_refs);

	neuron->n_class = neuron_class;
	neuron->u = neuron_class->u_rest;

	return neuron;

error:
	// @neuron->in_synapses FAIL
	if (neuron != NULL) {
		// @neuron->out_synapses FAIL
		if (neuron->in_synapses_refs != NULL) {
			vector_destroy(neuron->in_synapses_refs);
		}
		free(neuron);
	}
	return NULL;
}


void neuron_destroy(Neuron* neuron) {
	check(neuron != NULL, "NULL value for @neuron");
	check(neuron->in_synapses_refs != NULL, "NULL value for @neuron->in_synapses");
	check(neuron->out_synapses_refs != NULL, "NULL value for @neuron->out_synapses");

	vector_destroy(neuron->in_synapses_refs);
	vector_destroy(neuron->out_synapses_refs);
	free(neuron);

error:
	return;
}


Status neuron_add_in_synapse(Neuron* neuron, Synapse* synapse) {
	Status status = FAIL;
	check(neuron != NULL, "NULL value for @neuron");
	check(neuron->in_synapses_refs != NULL, "NULL value for @neuron->in_synapses");
	check(neuron->out_synapses_refs != NULL, "NULL value for @neuron->out_synapses");
	check(synapse != NULL, "NULL value for @synapse");
	
	// copy the address for the synapse
	check(vector_append(neuron->in_synapses_refs, &synapse) == SUCCESS, "Could not add new INPUT synapse");

	status = SUCCESS;

error:
	return status;
}


Status neuron_add_out_synapse(Neuron* neuron, Synapse* synapse) {
	Status status = FAIL;
	check(neuron != NULL, "NULL value for @neuron");
	check(neuron->in_synapses_refs != NULL, "NULL value for @neuron->in_synapses");
	check(neuron->out_synapses_refs != NULL, "NULL value for @neuron->out_synapses");
	check(synapse != NULL, "NULL value for @synapse");

	// copy the address for the synapse
	check(vector_append(neuron->out_synapses_refs, &synapse) == SUCCESS, "Could not add new OUTPU synapse");

	status = SUCCESS;

error:
	return status;
}


Status neuron_step(Neuron* neuron, uint32_t simulation_time) {
	Status status = FAIL;
	check(neuron != NULL, "NULL value for @neuron");
	check(neuron->in_synapses_refs != NULL, "NULL value for @neuron->in_synapses");
	check(neuron->out_synapses_refs != NULL, "NULL value for @neuron->out_synapses");

	// gather PSC
	float PSC = 0.0f;
	uint32_t i = 0u;
	uint8_t spike = 0u;
	Synapse* synapse = NULL;
	for (i = 0u; i < neuron->in_synapses_refs->length; i++) {
		synapse = *(Synapse**)vector_get(neuron->in_synapses_refs, i);
		PSC += synapse_compute_PSC(synapse, neuron->u);
		synapse_step(synapse, simulation_time);
	}

	// update neuron
	switch (neuron->n_class->type)
	{
	case LIF_NEURON:
		neuron->u = neuron->n_class->u_factor * neuron->u + neuron->n_class->free_factor + neuron->n_class->i_factor * PSC;
		// check for spike
		if (neuron->u >= neuron->n_class->u_th) {
			neuron->u = neuron->n_class->u_rest;
			spike = 1u;
		}
	default:
		break;
	}

	// if there was a spike update OUTPUT synapses
	for (i = 0u; i < neuron->out_synapses_refs->length; i++) {
		synapse = *(Synapse**)vector_get(neuron->out_synapses_refs, i);
		synapse_add_spike_time(synapse, simulation_time);
	}

	status = SUCCESS;

error:
	return status;
}

