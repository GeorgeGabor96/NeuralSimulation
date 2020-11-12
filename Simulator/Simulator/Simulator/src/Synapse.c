#include "Synapse.h"
#include <math.h>


SynapticClass* synaptic_class_create(float rev_potential, float tau_ms, uint32_t delay, SynapseType type, float simulation_step_ms) {
	check(tau_ms > 0.0, "@tau_ms should be > 0");
	check(simulation_step_ms > 0.0, "@simulation_step_ms should be > 0");
	
	SynapticClass* synaptic_class = (SynapticClass*)malloc(sizeof(SynapticClass));
	check_memory(synaptic_class);

	synaptic_class->e = rev_potential;
	synaptic_class->tau_exp = (float)(exp(- (double)simulation_step_ms / tau_ms));
	synaptic_class->delay = delay;
	synaptic_class->type = type;

	return synaptic_class;
error:
	return NULL;
}


void synaptic_class_destroy(SynapticClass* synaptic_class) {
	check(synaptic_class != NULL, "@synaptic_class should not be NULL");
	free(synaptic_class);

error:
	return;
}


Synapse* synapse_create(SynapticClass* s_class, float w) {
	Synapse* synapse = NULL;
	check(s_class != NULL, "@s_class should not be NULL");

	synapse = (Synapse*)malloc(sizeof(Synapse));
	check_memory(synapse);

	synapse->spike_times = queue_create(SYNAPSE_INITIAL_SPIKE_CAPACITY, sizeof(uint32_t));
	check_memory(synapse->spike_times);
	synapse->s_class = s_class;
	synapse->w = w;
	synapse->g = 0;

	return synapse;

error:
	// queue allocation failed
	if (synapse != NULL) {
		free(synapse);
	}

	return NULL;
}


void synapse_destroy(Synapse* synapse) {
	check(synapse != NULL, "@synapse should not be NULL");
	check(synapse->s_class != NULL, "@synapse->s_class should not be NULL");
	check(synapse->spike_times != NULL, "@synapse->spike_times should not be NULL");

	// because the synaptic_class will be shared, to save memory, we cannot free it here
	queue_destroy(synapse->spike_times);
	free(synapse);

error:
	return;
}

Status synapse_add_spike_time(Synapse* synapse, uint32_t spike_time) {
	Status status = FAIL;
	check(synapse != NULL, "@synapse should not be NULL");
	check(synapse->s_class != NULL, "@synapse->s_class should not be NULL");
	check(synapse->spike_times != NULL, "@synapse->spike_times should not be NULL");

	// add synaptic delay
	spike_time += synapse->s_class->delay;
	queue_enqueue(synapse->spike_times, &spike_time);

	status = SUCCESS;

error:
	return status;
}


float synapse_compute_psc(Synapse* synapse, float u) {
	float I = 0;
	check(synapse != NULL, "@synapse should not be NULL");
	check(synapse->s_class != NULL, "@synapse->s_class should not be NULL");
	check(synapse->spike_times != NULL, "@synapse->spike_times should not be NULL");

	switch (synapse->s_class->type)
	{
	case CONDUCTANCE_SYNAPCE:
		I = synapse->w * synapse->g;
		break;

	case VOLTAGE_DEPENDENT_SYNAPSE:
		I = synapse->w * synapse->g * (u - synapse->s_class->e);
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
	check(synapse != NULL, "@synapse should not be NULL");
	check(synapse->s_class != NULL, "@synapse->s_class should not be NULL");
	check(synapse->spike_times != NULL, "@synapse->spike_times should not be NULL");

	// check if their is a spike that arrives at this time step
	uint32_t* next_spike_time = queue_head(synapse->spike_times);
	if (*next_spike_time == simulation_time) {
		synapse->g += 1.0;
		queue_dequeue(synapse->spike_times);
	}
	else {
		// Raul had here some clipping for very small g
		// can the conductance be < 0??
		check(synapse->g >= 0.0, "@synapse->g is negative");
		synapse->g *= synapse->s_class->tau_exp;
	}

error:
	return status;
}

