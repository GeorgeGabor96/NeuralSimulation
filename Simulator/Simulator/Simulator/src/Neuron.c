#include "Neuron.h"
#include "utils/MemoryManagement.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
Status neuron_class_is_valid(NeuronClass* neuron_class) {
	check(neuron_class != NULL, null_argument("neuron_class"));
	check(string_is_valid(neuron_class->name) == TRUE, invalid_argument("neuron_class->name"));
	// NOTE: if you add more types this check needs to be updated
	check(neuron_class->type == LIF_NEURON || neuron_class->type == LIF_REFRACTORY_NEURON, invalid_argument("type"));

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
	// NOTE: DO not check for the validity of the output synapses, because in case of a recurrent synapses
	// the previous layer that uses outputs of this neuron will free its inputs (this neurons outputs) before
	// the current neuron is free, and in that case here the checks will fail
	/*
	for (i = 0; i < neuron->out_synapses_refs.length; ++i) {
		synapse = *((Synapse**)array_get(&(neuron->out_synapses_refs), i));
		check(synapse_is_valid(synapse) == TRUE, invalid_argument("synapse"));
	}
	*/
	return TRUE;

ERROR
	return FALSE;
}


/*************************************************************
* Neuron Type Functionality
*************************************************************/
const char* neuron_type_C_string(NeuronType type) {
	const char* name = NULL;
	if (type == INVALID_NEURON) name = "INVALID_NEURON";
	else if (type == LIF_NEURON) name = "LIF_NEURON";
	else if (type == LIF_REFRACTORY_NEURON) name = "LIF_REFRACTORY_NEURON";
	else name = "NEURON_UNKNOWN";
	return name;
}


/*************************************************************
* UPDATE functions for neurons
*************************************************************/
static inline BOOL neuron_update(Neuron* neuron, uint32_t time, float PSC) {
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
		break;
	case LIF_REFRACTORY_NEURON:
		// don't do anything if in refractory time
		if (time - neuron->last_spike_time <= neuron->n_class->refractory_time) {
			neuron->u = neuron->n_class->u_rest;
			spike = FALSE;
		}
		else {
			neuron->u = neuron->n_class->u_factor * neuron->u + neuron->n_class->free_factor + neuron->n_class->i_factor * PSC;
			if (neuron->u >= neuron->n_class->u_th) {
				neuron->u = neuron->n_class->u_rest;
				neuron->last_spike_time = time;		// update last spike time
				spike = TRUE;
			}
		}
		break;
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
	float EPSC = 0.0f;
	float IPSC = 0.0f;
	float current = 0.0f;
	uint32_t i = 0;
	Synapse* synapse = NULL;

	for (i = 0u; i < neuron->in_synapses_refs.length; i++) {
		synapse = *((Synapse**)array_get(&(neuron->in_synapses_refs), i));
		current = synapse_compute_PSC(synapse, neuron->u);
		
		// check if inhibitory of excitatory current
		if (current >= 0) EPSC += current;
		else IPSC += current;

		synapse_step(synapse, simulation_time);
	}

	// update neuron state
	neuron->EPSC = EPSC;
	neuron->IPSC = IPSC;

	// return the sum
	return EPSC + IPSC;
}


static inline void neuron_update_inputs(Neuron* neuron, uint32_t simulation_time) {
	uint32_t i = 0;
	Synapse* synapse = NULL;

	for (i = 0; i < neuron->in_synapses_refs.length; ++i) {
		synapse = *((Synapse**)array_get(&(neuron->in_synapses_refs), i));
		synapse_step(synapse, simulation_time);
	}
}


/*************************************************************
* NeuronClass Functionality
*************************************************************/

NeuronClass* neuron_class_create(const char* name, NeuronType type) {
	NeuronClass* neuron_class = NULL;
	check(type == LIF_NEURON || type == LIF_REFRACTORY_NEURON, invalid_argument("type"));

	neuron_class = (NeuronClass*)calloc(1, sizeof(NeuronClass), "neuron_class_create");
	check_memory(neuron_class);

	neuron_class->name = string_create(name);
	check(string_is_valid(neuron_class->name) == TRUE, invalid_argument("neuron_class->name"));
	neuron_class->type = type;

	switch (neuron_class->type)
	{
	case LIF_NEURON:
		neuron_class_set_LIF_parameters(neuron_class, LIF_U_TH, LIF_U_REST, LIF_R, LIF_C);
		break;
	case LIF_REFRACTORY_NEURON:
		neuron_class_set_LIF_refractor_parameters(neuron_class, LIF_U_TH, LIF_U_REST, LIF_R, LIF_C, LIF_REFRACT);
		break;
	default:
		log_error("Unkown neuron type");
		break;
	}

	return neuron_class;

ERROR
	if (neuron_class != NULL) {
		if (neuron_class->name != NULL) string_destroy(neuron_class->name);
		free(neuron_class);
	}
	return NULL;
}


NeuronClass* neuron_class_copy(NeuronClass* n_class) {
	NeuronClass* n_class_copy = NULL;
	check(neuron_class_is_valid(n_class) == TRUE, invalid_argument("n_class"));
	n_class_copy = calloc(1, sizeof(NeuronClass), "neuron_class_copy");
	check_memory(n_class_copy);

	n_class_copy->name = string_copy(n_class->name);
	check(string_is_valid(n_class_copy->name) == TRUE, invalid_argument("n_class_copy->name"));
	n_class_copy->free_factor = n_class->free_factor;
	n_class_copy->i_factor = n_class->i_factor;
	n_class_copy->refractory_time = n_class->refractory_time;
	n_class_copy->tau = n_class->tau;
	n_class_copy->type = n_class->type;
	n_class_copy->u_factor = n_class->u_factor;
	n_class_copy->u_rest = n_class->u_rest;
	n_class_copy->u_th = n_class->u_th;

	return n_class_copy;
ERROR
	if (n_class_copy != NULL) {
		if (n_class_copy->name != NULL) string_destroy(n_class_copy->name);
		free(n_class_copy);
	}
	return NULL;
}


void neuron_class_reset(NeuronClass* neuron_class) {
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));
	string_destroy(neuron_class->name);
	neuron_class->name = NULL;
	neuron_class->free_factor = 0.0f;
	neuron_class->i_factor = 0.0f;
	neuron_class->u_factor = 0.0f;
	neuron_class->tau = 0.0f;
	neuron_class->type = INVALID_NEURON;
	neuron_class->u_rest = 0.0f;
	neuron_class->u_th = 0.0f;

	neuron_class->refractory_time = 0;
		
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


void neuron_class_ref_destroy(NeuronClass** neuron_class) {
	neuron_class_destroy(*neuron_class);
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


Status neuron_class_set_LIF_refractor_parameters(NeuronClass* neuron_class, float u_th, float u_rest, float r, float c, uint32_t refract_time) {
	Status status = FAIL;
	
	status = neuron_class_set_LIF_parameters(neuron_class, u_th, u_rest, r, c);
	check(status == SUCCESS, "Couldn't set LIF parameters");
	neuron_class->refractory_time = refract_time;
	
ERROR
	return status;
}


// UNTESTED
String* neuron_class_get_desc(NeuronClass* neuron_class) {
	check(neuron_class_is_valid(neuron_class) == TRUE, invalid_argument("neuron_class"));
	char description[1024] = { 0 };
	sprintf(description, "name: %s\n"
		"type: %s\n"
		"u_th: %f\n"
		"u_rest: %f\n"
		"tau: %f\n"
		"u_factor: %f\n"
		"i_factor: %f\n"
		"free_factor: %f\n"
		"refractory_period: %u\n",
		string_get_C_string(neuron_class->name),
		neuron_type_C_string(neuron_class->type),
		neuron_class->u_rest, neuron_class->u_rest,
		neuron_class->tau,
		neuron_class->u_factor, neuron_class->i_factor, neuron_class->free_factor,
		neuron_class->refractory_time);
	return string_create((const char*)description);
ERROR
	return NULL;
}



/*************************************************************
* Neuron Functionality
*************************************************************/

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
	neuron->EPSC = 0.0f;
	neuron->IPSC = 0.0f;
	neuron->last_spike_time = 0;
	neuron->spike = FALSE;

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
	neuron->EPSC = 0.0f;
	neuron->IPSC = 0.0f;
	neuron->spike = FALSE;
	neuron->u = 0.0f;
	neuron->last_spike_time = 0;

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


Status neuron_step(Neuron* neuron, uint32_t simulation_time) {
	Status status = FAIL;
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	float PSC = neuron_compute_psc(neuron, simulation_time);
	neuron->spike = neuron_update(neuron, simulation_time, PSC);
	if (neuron->spike == TRUE) {
		neuron_update_out_synapses(neuron, simulation_time);
	}

	status = SUCCESS;

ERROR
	return status;
}


Status neuron_step_force_spike(Neuron* neuron, uint32_t simulation_time) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	neuron_update_inputs(neuron, simulation_time);
	neuron->spike = TRUE;
	neuron->u = neuron->n_class->u_rest;
	neuron->last_spike_time = simulation_time;
	neuron_update_out_synapses(neuron, simulation_time);

	return SUCCESS;

ERROR
	return FAIL;
}


Status neuron_step_inject_current(Neuron* neuron, float PSC, uint32_t simulation_time) {
	check(neuron_is_valid(neuron) == TRUE, invalid_argument("neuron"));

	// simulate receiving exterior current plus the normal one
	float total_PSC = neuron_compute_psc(neuron, simulation_time) + PSC;
	neuron->spike = neuron_update(neuron, simulation_time, total_PSC);
	if (neuron->spike == TRUE) {
		neuron_update_out_synapses(neuron, simulation_time);
	}

	return SUCCESS;

ERROR
	return FAIL;
}
