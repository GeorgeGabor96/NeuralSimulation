#include "Neuron.h"
#include "MemoryManagement.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status neuron_class_is_valid(NeuronClass* neuron_class) {
	check(neuron_class != NULL, null_argument("neuron_class"));
	// NOTE: if you add more types this check needs to be updated
	check(neuron_class->type == LIF_NEURON, invalid_argument("type"));

	return TRUE;

ERROR
	return FALSE;
}


Status neuron_is_valid(Neuron* neuron) {
	check(neuron != NULL, null_argument("neuron"));
	check(neuron_class_is_valid(neuron->n_class) == TRUE, invalid_argument("neuron->n_class"));
	check(array_is_valid(&(neuron->in_synapses_refs)) == TRUE, invalid_argument("neuron->in_synapses_refs"));
	check(array_is_valid(&(neuron->out_synapses_refs)) == TRUE, invalid_argument("neuron_>out_synapses_refs"));

	// check the synapses, which are kept as references, so the @array_get return **
	uint32_t i = 0;
	Synapse* synapse = NULL;
	// check input synapses
	for (i = 0; i < neuron->in_synapses_refs.length; ++i) {
		synapse = *((Synapse**)array_get(&(neuron->in_synapses_refs), i));
		check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
	}
	// check output synapses
	for (i = 0; i < neuron->out_synapses_refs.length; ++i) {
		synapse = *((Synapse**)array_get(&(neuron->out_synapses_refs), i));
		check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
	}

	return TRUE;

ERROR
	return FALSE;
}


/*************************************************************
* Neuron Type Functionality
*************************************************************/
const char* neuron_type_C_string(type) {
	const char* name = NULL;
	if (type == LIF_NEURON) name = "LIF_NEURON";
	else name = "NEURON_UNKNOWN";
	return name;
}


/*************************************************************
* UPDATE functions for neurons
*************************************************************/
static inline BOOL neuron_update(Neuron* neuron, float PSC) {
	BOOL spike = FALSE;
	
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

	for (i = 0u; i < neuron->out_synapses_refs.length; i++) {
		synapse = *((Synapse**)array_get(&(neuron->out_synapses_refs), i));
		check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
		synapse_add_spike_time(synapse, simulation_time);
	}
ERROR
	return;
}


static inline float neuron_compute_psc(Neuron* neuron, uint32_t simulation_time) {
	float PSC = 0.0f;
	uint32_t i = 0ui32;
	Synapse* synapse = NULL;

	for (i = 0u; i < neuron->in_synapses_refs.length; i++) {
		synapse = *((Synapse**)array_get(&(neuron->in_synapses_refs), i));
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
	
	neuron_class = (NeuronClass*)malloc(sizeof(NeuronClass), "neuron_class_create");
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

ERROR
	return NULL;
}


void neuron_class_reset(NeuronClass* neuron_class) {
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));
	neuron_class->free_factor = 0.0f;
	neuron_class->i_factor = 0.0f;
	neuron_class->u_factor = 0.0f;
	neuron_class->tau = 0.0f;
	neuron_class->type = INVALID_NEURON;
	neuron_class->u_rest = 0.0f;
	neuron_class->u_th = 0.0f;
		
ERROR
	return;
}

void neuron_class_destroy(NeuronClass* neuron_class) {
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));
	neuron_class_reset(neuron_class);
	free(neuron_class);
ERROR
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

ERROR
	return status;
}


/*************************************************************
* Neuron Functionality
*************************************************************/

// TODO: maybe we can allocate from start the amount of input, output synapses we need -> more efficient use of memory
Status neuron_init(Neuron* neuron, NeuronClass* neuron_class) {
	Status status = FAIL;
	// the neuron can be invalid
	check(neuron != NULL, null_argument("neuron"));
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));

	status = array_init(&(neuron->in_synapses_refs), NEURON_INITIAL_SYNAPSE_LENGTH, 0, sizeof(Synapse*));
	check(status == SUCCESS, "Couldn't init @neuron->in_synapses");

	status = array_init(&(neuron->out_synapses_refs), NEURON_INITIAL_SYNAPSE_LENGTH, 0, sizeof(Synapse*));
	check(status == SUCCESS, "Couldn't init @neuron->out_synapses_refs");

	neuron->n_class = neuron_class;
	neuron->u = neuron_class->u_rest;

	return SUCCESS;

ERROR
	// @neuron->in_synapses FAIL
	if (neuron != NULL) {
		if (array_is_valid(&(neuron->in_synapses_refs)) == TRUE) array_reset(&(neuron->in_synapses_refs), NULL);
		if (array_is_valid(&(neuron->out_synapses_refs)) == TRUE) array_reset(&(neuron->out_synapses_refs), NULL);
	}
	return FAIL;
}


void neuron_reset(Neuron* neuron) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	array_reset(&(neuron->in_synapses_refs), synapse_destroy_2p);
	array_reset(&(neuron->out_synapses_refs), NULL);
	neuron->n_class = NULL;

ERROR
	return;
}


Neuron* neuron_create(NeuronClass* neuron_class) {
	Neuron* neuron = NULL;
	Status status = FAIL;
	neuron = (Neuron*)calloc(1, sizeof(Neuron), "neuron_create");
	check_memory(neuron);

	status = neuron_init(neuron, neuron_class);
	check(status == SUCCESS, "Could not init @neuron");

	return neuron;

ERROR
	if (neuron != NULL) {
		free(neuron);
	}
	return NULL;
}


void neuron_destroy(Neuron* neuron) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	neuron_reset(neuron);
	free(neuron);

ERROR
	return;
}


Status neuron_add_in_synapse(Neuron* neuron, Synapse* synapse) {
	Status status = FAIL;
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	status = array_append(&(neuron->in_synapses_refs), &synapse);
	check(status == SUCCESS, "Could not add new INPUT synapse");

ERROR
	return status;
}


Status neuron_add_out_synapse(Neuron* neuron, Synapse* synapse) {
	Status status = FAIL;
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
	check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));

	status = array_append(&(neuron->out_synapses_refs), &synapse);
	check(status == SUCCESS, "Could not add new OUTPUT synapse");

ERROR
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

ERROR
	return status;
}


void neuron_clear_state(Neuron* neuron) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));
	neuron->spike = FALSE;
	neuron->u = neuron->n_class->u_rest;
	uint32_t i = 0;
	Synapse* synapse = NULL;
	for (i = 0; i < neuron->in_synapses_refs.length; ++i) {
		synapse = *((Synapse**)array_get(&(neuron->in_synapses_refs), i));
		synapse_clear_state(synapse);
	}

ERROR
	return;
}


Status neuron_step_force_spike(Neuron* neuron, uint32_t simulation_time) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	neuron->spike = TRUE;
	neuron_update_out_synapses(neuron, simulation_time);

	return SUCCESS;

ERROR
	return FAIL;
}


Status neuron_step_inject_current(Neuron* neuron, float PSC, uint32_t simulation_time) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	neuron->spike = neuron_update(neuron, PSC);
	if (neuron->spike == TRUE) {
		neuron_update_out_synapses(neuron, simulation_time);
	}

	return SUCCESS;

ERROR
	return FAIL;
}
