#include "Synapse.h"
#include <math.h>
#include <stdlib.h>


/*************************************************************
* CHECKS FUNCTIONS and ERROR MESSAGES
*************************************************************/
Status synapse_is_valid(Synapse* synapse) {
	check(synapse != NULL, null_argument("synapse"));
	check(synapse->s_class != NULL, null_argument("synapse->s_class"));
	check(synapse->spike_times != NULL, null_argument("synapse->spike_times"));

	return TRUE;

error:
	return FALSE;
}


/*************************************************************
* SynapseClass Functionality
*************************************************************/
SynapseClass* synapse_class_create(float rev_potential, float tau_ms, uint32_t delay, SynapseType type, float simulation_step_ms) {
	check(tau_ms > 0.0, "@tau_ms should be > 0");
	check(simulation_step_ms > 0.0, "@simulation_step_ms should be > 0");
	check(type == CONDUCTANCE_SYNAPCE || type == VOLTAGE_DEPENDENT_SYNAPSE, "@type has invalid valud");

	SynapseClass* synapse_class = (SynapseClass*)malloc(sizeof(SynapseClass));
	check_memory(synapse_class);

	synapse_class->E = rev_potential;
	synapse_class->tau_exp = (float)(exp(- (double)simulation_step_ms / tau_ms));
	synapse_class->delay = delay;
	synapse_class->type = type;

	return synapse_class;
error:
	return NULL;
}


void synapse_class_destroy(SynapseClass* synapse_class) {
	check(synapse_class != NULL, "@synaptic_class should not be NULL");
	free(synapse_class);

error:
	return;
}


/*************************************************************
* Synapse Functionality
*************************************************************/
Synapse* synapse_create(SynapseClass* s_class, float w) {
	Synapse* synapse = NULL;
	check(s_class != NULL, null_argument("s_class"));

	synapse = (Synapse*)malloc(sizeof(Synapse));
	check_memory(synapse);

	synapse->spike_times = queue_create(SYNAPSE_INITIAL_SPIKE_CAPACITY, sizeof(uint32_t));
	check_memory(synapse->spike_times);
	synapse->s_class = s_class;
	synapse->w = w;
	synapse->g = 0.0f;

	return synapse;

error:
	// queue allocation failed
	if (synapse != NULL) {
		free(synapse);
	}

	return NULL;
}


void synapse_destroy(Synapse* synapse) {
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	// because the synaptic_class will be shared, to save memory, we cannot free it here
	queue_destroy(synapse->spike_times);
	free(synapse);

error:
	return;
}


Status synapse_add_spike_time(Synapse* synapse, uint32_t spike_time) {
	Status status = FAIL;
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
	if_check(!queue_is_empty(synapse->spike_times), *(uint32_t*)queue_head(synapse->spike_times) < spike_time, "Spike should not be older then the head");
	
	// add synaptic delay
	spike_time += synapse->s_class->delay;
	queue_enqueue(synapse->spike_times, &spike_time);

	status = SUCCESS;

error:
	return status;
}


float synapse_compute_PSC(Synapse* synapse, float u) {
	float I = 0.0f;
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	switch (synapse->s_class->type)
	{
	case CONDUCTANCE_SYNAPCE:
		I = synapse->w * synapse->g;
		break;

	case VOLTAGE_DEPENDENT_SYNAPSE:
		I = synapse->w * synapse->g * (u - synapse->s_class->E);
		break;

	default:
		log_error("Undefined SYNAPTIC TYPE %d", synapse->s_class->type);
		break;
	}

error:
	return I;
}


Status synapse_step(Synapse* synapse, uint32_t simulation_time) {
	Status status = FAIL;
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	// check if their is a spike that arrives at this time step
	if (!queue_is_empty(synapse->spike_times) && *(uint32_t*)queue_head(synapse->spike_times) == simulation_time) {
		synapse->g += 1.0f;
		queue_dequeue(synapse->spike_times);
	}
	else {
		/* The conductance should never be negative, at least this is what I understand now */
		/* The else clips the conductance because currently I don't care for that much precision*/
		check(synapse->g >= 0.0f, "@synapse->g is negative");
		if (synapse->g > 0.00001f) {
			synapse->g *= synapse->s_class->tau_exp;
		} 
		else {
			synapse->g = 0.0f;
		}
	}

	status = SUCCESS;

error:
	return status;
}

