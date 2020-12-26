#include "Neuron.h"

#include <stdlib.h>


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status neuron_class_is_valid(NeuronClass* neuron_class) {
	check(neuron_class != NULL, null_argument("neuron_class"));
	// NOTE: if you add more types this check needs to be updated
	check(neuron_class->type == LIF_NEURON, invalid_argument("type"));

	return TRUE;

error:
	return FALSE;
}


Status neuron_is_valid(Neuron* neuron) {
	check(neuron != NULL, null_argument("neuron"));
	check(neuron->in_synapses != NULL, null_argument("neuron->in_synapses"));
	check(neuron->out_synapses_refs != NULL, null_argument("neuron_>out_synapses"));

	return TRUE;

error:
	return FALSE;
}


/*************************************************************
* UPDATE functions for neurons
*************************************************************/
static inline Status neuron_update(Neuron* neuron, float PSC) {
	Status spike = FALSE;
	
	switch (neuron->n_class->type)
	{
	case LIF_NEURON:
		neuron->u = neuron->n_class->u_factor * neuron->u + neuron->n_class->free_factor + neuron->n_class->i_factor * PSC;
		// check for spike
		if (neuron->u >= neuron->n_class->u_th) {
			neuron->u = neuron->n_class->u_rest;
			spike = TRUE;
		}
	default:
		break;
	}

	return spike;
}


static inline void neuron_update_out_synapses(Neuron* neuron, uint32_t simulation_time) {
	uint32_t i = 0;
	Synapse* synapse = NULL;

	for (i = 0u; i < neuron->out_synapses_refs->length; i++) {
		synapse = *(Synapse**)vector_get(neuron->out_synapses_refs, i);
		synapse_add_spike_time(synapse, simulation_time);
	}
}


static inline float neuron_compute_psc(Neuron* neuron, uint32_t simulation_time) {
	// gather PSC
	float PSC = 0.0f;
	uint32_t i = 0u;
	Synapse* synapse = NULL;

	for (i = 0u; i < neuron->in_synapses->length; i++) {
		synapse = (Synapse*)vector_get(neuron->in_synapses, i);
		PSC += synapse_compute_PSC(synapse, neuron->u);
		synapse_step(synapse, simulation_time);
	}

	return PSC;
}


/*************************************************************
* NeuronClass Functionality
*************************************************************/

NeuronClass* neuron_class_create(NeuronType type) {
	NeuronClass* neuron_class = NULL;
	check(type == LIF_NEURON, invalid_argument("type"));
	
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
	return NULL;
}


void neuron_class_destroy(NeuronClass* neuron_class) {
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));

	free(neuron_class);
error:
	return;
}


Status neuron_class_set_LIF_parameters(NeuronClass* neuron_class, float u_th, float u_rest, float r, float c) {
	Status status = FAIL;
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));

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


// TODO: maybe we can allocate from start the amount of input, output synapses we need -> more efficient use of memory
Status neuron_init(Neuron* neuron, NeuronClass* neuron_class) {
	// the neuron can be invalid
	check(neuron != NULL, null_argument("neuron"));
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));

	neuron->in_synapses = vector_create(NEURON_INITIAL_SYNAPSE_LENGHT, sizeof(Synapse));
	check_memory(neuron->in_synapses);

	neuron->out_synapses_refs = vector_create(NEURON_INITIAL_SYNAPSE_LENGHT, sizeof(Synapse*));
	check_memory(neuron->out_synapses_refs);

	neuron->n_class = neuron_class;
	neuron->u = neuron_class->u_rest;

	return SUCCESS;

error:
	// @neuron->in_synapses FAIL
	if (neuron != NULL) {
		// @neuron->out_synapses FAIL
		if (neuron->in_synapses != NULL) {
			vector_destroy(neuron->in_synapses, synapse_reset);
		}
		// NOTE: the called should manage the memory of @neuron
	}
	return FAIL;
}


void neuron_reset(Neuron* neuron) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	vector_destroy(neuron->in_synapses, synapse_reset);
	vector_destroy(neuron->out_synapses_refs, NULL);
	neuron->n_class = NULL;
	// NOTE: neuron_class should be managed by the called, may be common to multiple neurons

error:
	return;
}


Neuron* neuron_create(NeuronClass* neuron_class) {
	Neuron* neuron = NULL;
	neuron = (Neuron*)calloc(1, sizeof(Neuron));
	check_memory(neuron);

	check(neuron_init(neuron, neuron_class) == SUCCESS, init_argument("neuron"));

	return neuron;

error:
	if (neuron != NULL) {
		// could not init @neuron
		free(neuron);
	}
	return NULL;
}


void neuron_destroy(Neuron* neuron) {
	// TODO: vezi ca fara acest check daca pusca in reset tu dai free si nu stiu daca asta se doreste
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	neuron_reset(neuron);
	free(neuron);

error:
	return;
}


Status neuron_add_in_synapse(Neuron* neuron, Synapse* synapse) {
	Status status = FAIL;
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
	
	// NOTE: @neuron keeps internally its INPUT synapses, so copy its content and free it
	check(vector_append(neuron->in_synapses, synapse) == SUCCESS, "Could not add new INPUT synapse");
	free(synapse); // do not delete the queue which was just copied in @neuron->in_synapses

	status = SUCCESS;

error:
	return status;
}


Status neuron_add_out_synapse(Neuron* neuron, Synapse* synapse) {
	Status status = FAIL;
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	// NOTE: @neuron keeps references to its OUTPUT synapses, so keeps a reference to it
	check(vector_append(neuron->out_synapses_refs, &synapse) == SUCCESS, "Could not add new OUTPUT synapse");

	status = SUCCESS;

error:
	return status;
}


Status neuron_step(Neuron* neuron, uint32_t simulation_time) {
	Status status = FAIL;
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	float PSC = neuron_compute_psc(neuron, simulation_time);
	neuron->spike = neuron_update(neuron, PSC);
	if (neuron->spike == TRUE) {
		neuron_update_out_synapses(neuron, simulation_time);
	}

	status = SUCCESS;

error:
	return status;
}


Status neuron_force_spike(Neuron* neuron, uint32_t simulation_time) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	neuron->spike = TRUE;
	neuron_update_out_synapses(neuron, simulation_time);

	return SUCCESS;

error:
	return FAIL;
}


Status neuron_inject_current(Neuron* neuron, float PSC, uint32_t simulation_time) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	neuron->spike = neuron_update(neuron, PSC);
	if (neuron->spike == TRUE) {
		neuron_update_out_synapses(neuron, simulation_time);
	}

	return SUCCESS;

error:
	return FAIL;
}
