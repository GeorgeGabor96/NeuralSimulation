#include <math.h>

#include "Synapse.h"
#include "utils/MemoryManagement.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status synapse_class_is_valid(SynapseClass* synapse_class) {
	check(synapse_class != NULL, null_argument("synapse_class"));
	// NOTE: If you add more types this check needs to be updated
	check(synapse_class->type == CONDUCTANCE_SYNAPSE || synapse_class->type == VOLTAGE_DEPENDENT_SYNAPSE, invalid_argument("synapse_class->type"));

	return TRUE;

ERROR
	return FALSE;
}


Status synapse_is_valid(Synapse* synapse) {
	check(synapse != NULL, null_argument("synapse"));
	check(synapse_class_is_valid(synapse->s_class) == TRUE, invalid_argument("synapse->s_class"));
	check(queue_is_valid(&(synapse->spike_times)) == TRUE, invalid_argument("synapse->spike_times"));

	return TRUE;

ERROR
	return FALSE;
}


/*************************************************************
* Synapse Type Functionality
*************************************************************/
const char* synapse_type_C_string(SynapseType type) {
	const char* name = NULL;
	if (type == CONDUCTANCE_SYNAPSE) name = "CONDUCTANCE_SYNAPSE";
	else if (type == VOLTAGE_DEPENDENT_SYNAPSE) name = "VOLTAGE_DEPENDENT_SYNAPSE";
	else name = "SYNAPSE_UNKNOWN";
	return name;
}


/*************************************************************
* SynapseClass Functionality
*************************************************************/
SynapseClass* synapse_class_create(float rev_potential, float tau_ms, uint32_t delay, SynapseType type, float simulation_step_ms) {
	check(tau_ms > 0.0, "@tau_ms should be > 0");
	check(simulation_step_ms > 0.0, "@simulation_step_ms should be > 0");
	check(type == CONDUCTANCE_SYNAPSE || type == VOLTAGE_DEPENDENT_SYNAPSE, invalid_argument("type"));

	SynapseClass* synapse_class = (SynapseClass*)malloc(sizeof(SynapseClass), "synapse_class_create");
	check_memory(synapse_class);

	synapse_class->E = rev_potential;
	synapse_class->tau_exp = (float)(exp(- (double)simulation_step_ms / tau_ms));
	synapse_class->delay = delay;
	synapse_class->type = type;

	return synapse_class;
ERROR
	return NULL;
}


SynapseClass* synapse_class_create_default() {
	return synapse_class_create(SYNAPSE_REV_POTENTIAL_DF, SYNAPSE_TAU_MS_DF, SYNAPSE_DELAY_DF, SYNAPSE_TYPE_DF, SYNAPSE_SIMULATION_TIME_MS_DF);
}


void synapse_class_reset(SynapseClass* synapse_class) {
	check(synapse_class_is_valid(synapse_class) == TRUE, invalid_argument("synapse_class"));
	synapse_class->delay = 0;
	synapse_class->E = 0.0f;
	synapse_class->tau_exp = 0.0f;
	synapse_class->type = INVALID_SYNAPSE;
ERROR
	return;
}


void synapse_class_destroy(SynapseClass* synapse_class) {
	check(synapse_class_is_valid(synapse_class) == TRUE, invalid_argument("synapse_class"));
	synapse_class_reset(synapse_class);
	free(synapse_class);

ERROR
	return;
}


/*************************************************************
* Synapse Functionality
*************************************************************/
Status synapse_init(Synapse* synapse, SynapseClass* s_class, float w) {
	Status status = FAIL;
	check(synapse != NULL, null_argument("synapse"));
	check(synapse_class_is_valid(s_class) == TRUE, invalid_argument("s_class"));

	status = queue_init(&(synapse->spike_times), SYNAPSE_INITIAL_SPIKE_CAPACITY, sizeof(uint32_t));
	check(status == SUCCESS, "Could not init @synapse->spike_times");
	synapse->s_class = s_class;
	synapse->w = w;
	synapse->g = 0.0f;

ERROR
	return status;
}


void synapse_reset(Synapse* synapse) {
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	queue_reset(&(synapse->spike_times), NULL);
	synapse->s_class = NULL;

ERROR
	return;
}


void synapse_destroy_2p(Synapse** synapse) {
	synapse_destroy(*synapse);
}



Synapse* synapse_create(SynapseClass* s_class, float w) {
	Synapse* synapse = NULL;
	Status status = FAIL;

	synapse = (Synapse*)malloc(sizeof(Synapse), "synapse_create");
	check_memory(synapse);
		
	status = synapse_init(synapse, s_class, w);
	check(status == SUCCESS, init_argument("synapse"));

	return synapse;

ERROR
	if (synapse != NULL) {
		free(synapse);
	}

	return NULL;
}


void synapse_destroy(Synapse* synapse) {
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	synapse_reset(synapse);
	free(synapse);

ERROR
	return;
}


Status synapse_add_spike_time(Synapse* synapse, uint32_t spike_time) {
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
	// add synaptic delay
	spike_time += synapse->s_class->delay;

	// current spike time should be older than one already in queue
	if_check(queue_is_empty(&(synapse->spike_times)) == FALSE, *((uint32_t*)queue_head(&(synapse->spike_times))) < spike_time, "Spike %u should not be older than the head %u", spike_time, *((uint32_t*)queue_head(&(synapse->spike_times))));
	queue_enqueue(&(synapse->spike_times), &spike_time);

	return SUCCESS;

ERROR
	return FAIL;
}


float synapse_compute_PSC(Synapse* synapse, float u) {
	float I = 0.0f;
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	switch (synapse->s_class->type)
	{
	case CONDUCTANCE_SYNAPSE:
		I = synapse->w * synapse->g;
		break;

	case VOLTAGE_DEPENDENT_SYNAPSE:
		I = - synapse->w * synapse->g * (u - synapse->s_class->E);
		break;

	default:
		log_error("Undefined SYNAPTIC TYPE %d", synapse->s_class->type);
		break;
	}

ERROR
	return I;
}


Status synapse_step(Synapse* synapse, uint32_t simulation_time) {
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	// check if there is a spike that arrives at this time step
	if ((queue_is_empty(&(synapse->spike_times)) == FALSE) && (*((uint32_t*)queue_head(&(synapse->spike_times))) == simulation_time)) {
		synapse->g += 1.0f;
		queue_dequeue(&(synapse->spike_times));
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

	return SUCCESS;

ERROR
	return FAIL;
}


void synapse_clear_state(Synapse* synapse) {
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
	
	// empty the queue
	while (queue_is_empty(&(synapse->spike_times)) == FALSE) {
		queue_dequeue(&(synapse->spike_times));
	}
	
	// reset conductance
	synapse->g = 0.0f;
ERROR
	return;
}

