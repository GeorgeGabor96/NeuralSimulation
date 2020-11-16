#include "neuron_tests.h"
#include "Neuron.h"


TestStatus neuron_class_create_test() {
	// setup
	TestStatus status = TEST_FAILED;
	NeuronType n_type = LIF_NEURON;
	float tau = LIF_C * LIF_R;
	float u_factor = 1.0f - 1.0f / (LIF_C * LIF_R);
	float i_factor = 1.0f / LIF_C;
	float free_factor = LIF_U_REST / (LIF_C * LIF_R);
	NeuronClass* n_class = NULL;

	// call with invalid @type
	assert(neuron_class_create(5) == NULL, "Should not work for invalid @type");

	// call normal
	n_class = neuron_class_create(n_type);
	assert(n_class != NULL, "Should be able to create a @NeuronClass");
	assert(n_class->type == n_type, "@n_class->type should be %d not %d", n_type, n_class->type);

	// check LIF values
	assert(n_class->u_th == LIF_U_TH, "@n_class->u_th should be %f not %f", LIF_U_TH, n_class->u_th);
	assert(n_class->u_rest == LIF_U_REST, "@n_class->u_rest should be %f not %f", LIF_U_REST, n_class->u_rest);
	assert(float_test(n_class->tau, tau), "@n_class->tau should be %f not %f", tau, n_class->tau);
	assert(float_test(n_class->u_factor, u_factor), "@n_class->u_factor should be %f not %f", u_factor, n_class->u_factor);
	assert(float_test(n_class->i_factor, i_factor), "@n_class->i_factor should be %f not %f", i_factor, n_class->i_factor);
	assert(float_test(n_class->free_factor, free_factor), "@n_class->free_factor should be %f not %f", free_factor, n_class->free_factor);

	status = SUCCESS;

	// cleaup
error:
	if (n_class != NULL) {
		neuron_class_destroy(n_class);
	}
	return status;
}


TestStatus neuron_class_destroy_test() {
	// setup
	NeuronClass* n_class = neuron_class_create(LIF_NEURON);

	// call for @neuron_class = NULL
	neuron_class_destroy(NULL);

	// normal call
	neuron_class_destroy(n_class);

	// cleanup
	return TEST_SUCCESS;
}


TestStatus neuron_class_set_LIF_parameters_test() {
	// setup
	TestStatus status = TEST_FAILED;
	float noise = 1.5f;
	float u_th = LIF_U_TH + noise;
	float u_rest = LIF_U_REST + noise;
	float r = LIF_C + noise;
	float c = LIF_R + noise;
	float tau = r * c;
	float u_factor = 1.0f - 1.0f / (r * c);
	float i_factor = 1.0f / c;
	float free_factor = u_rest / (r * c);
	NeuronClass n_class;
	n_class.type = LIF_NEURON;
	NeuronClass n_class_copy;
	memcpy(&n_class_copy, &n_class, sizeof(NeuronClass));

	// call with @neuron_class = NULL
	assert(neuron_class_set_LIF_parameters(NULL, u_th, u_rest, r, c) == FAIL, "Should fail for @neuron_class = NULL");
	assert(memcmp(&n_class, &n_class_copy, sizeof(NeuronClass)) == 0, "Should have not modify the content");

	// normal call
	assert(neuron_class_set_LIF_parameters(&n_class, u_th, u_rest, r, c) == SUCCESS, "Should work");
	assert(n_class.u_th == u_th, "@n_class.u_th should be %f not %f", u_th, n_class.u_th);
	assert(n_class.u_rest == u_rest, "@n_class.u_rest should be %f not %f", u_rest, n_class.u_rest);
	assert(float_test(n_class.tau, tau), "@n_class.tau should be %f not %f", tau, n_class.tau);
	assert(float_test(n_class.u_factor, u_factor), "@n_class.u_factor should be %f not %f", u_factor, n_class.u_factor);
	assert(float_test(n_class.i_factor, i_factor), "@n_class.i_factor should be %f not %f", i_factor, n_class.i_factor);
	assert(float_test(n_class.free_factor, free_factor), "@n_class.free_factor should be %f not %f", free_factor, n_class.free_factor);

	status = SUCCESS;

	// cleaup
error:
	// no memory has allocated, only stack
	return status;
}

TestStatus neuron_create_test() {
	// setup
	TestStatus status = TEST_FAILED;
	NeuronClass n_class;
	n_class.type = LIF_NEURON;
	neuron_class_set_LIF_parameters(&n_class, LIF_U_TH, LIF_U_REST, LIF_R, LIF_C);
	Neuron* neuron = NULL;

	// call with @neuron_class = NULL
	assert(neuron_create(NULL) == NULL, "Should fail for @neuron_class = NULL");
	
	// call normal
	neuron = neuron_create(&n_class);
	assert(neuron != NULL, "@neuron should not be NULL");
	assert(neuron->in_synapses_refs != NULL, "@neuron->in_synapses_refs should not be NULL");
	assert(neuron->out_synapses_refs != NULL, "@neuron->out_synapses_refs should not be NULL");
	assert(neuron->n_class == &n_class, "@neuron->n_class should be %p not %p", &n_class, neuron->n_class);
	assert(neuron->u == n_class.u_rest, "@neuron->u should be %f not %f", n_class.u_rest, neuron->u);

	status = TEST_SUCCESS;

	// cleanup
error:
	if (neuron != NULL) {
		neuron_destroy(neuron);
	}
	return status;
}


TestStatus neuron_destroy_test() {
	// setup
	NeuronClass n_class;
	n_class.type = LIF_NEURON;
	neuron_class_set_LIF_parameters(&n_class, LIF_U_TH, LIF_U_REST, LIF_R, LIF_C);
	Neuron* neuron = neuron_create(&n_class);
	Vector* in_synapses = neuron->in_synapses_refs;
	Vector* out_synapses = neuron->out_synapses_refs;

	// call with @neuron = NULL
	neuron_destroy(NULL);

	// call with @neuron->in_synapses_refs = NULL
	neuron->in_synapses_refs = NULL;
	neuron_destroy(neuron);
	neuron->in_synapses_refs = in_synapses;

	// call with @neuron->out_synapses_refs = NULL
	neuron->out_synapses_refs = NULL;
	neuron_destroy(neuron);
	neuron->out_synapses_refs = out_synapses;

	// normal call
	neuron_destroy(neuron);

	return TEST_SUCCESS;
}


TestStatus neuron_add_in_synapse_test() {
	// setup
	TestStatus status = TEST_FAILED;
	NeuronClass n_class;
	n_class.type = LIF_NEURON;
	neuron_class_set_LIF_parameters(&n_class, LIF_U_TH, LIF_U_REST, LIF_R, LIF_C);
	Neuron* neuron = neuron_create(&n_class);
	Vector* in_synapses = neuron->in_synapses_refs;
	Vector* out_synapses = neuron->out_synapses_refs;
	
	// create tests synapses
	float w1 = 1.1f;
	float w2 = 3.4f;
	SynapseClass s_class;
	s_class.type = CONDUCTANCE_SYNAPCE;
	Synapse* sy1 = synapse_create(&s_class, w1);
	Synapse* sy2 = synapse_create(&s_class, w2);
	Synapse* sy = NULL;
	Synapse sy1_copy;
	Synapse sy2_copy;
	memcpy(&sy1_copy, sy1, sizeof(Synapse));
	memcpy(&sy2_copy, sy2, sizeof(Synapse));

	// call with @neuron = NULL
	assert(neuron_add_in_synapse(NULL, sy1) == FAIL, "Should fail for @neuron = NULL");

	// call with @neuron->in_synapses_refs = NULL
	neuron->in_synapses_refs = NULL;
	assert(neuron_add_in_synapse(neuron, sy1) == FAIL, "Should fail for @neuron->in_synapses_refs = NULL");
	neuron->in_synapses_refs = in_synapses;

	// call with @neuron->out_synapses_refs = NULL
	neuron->out_synapses_refs = NULL;
	assert(neuron_add_in_synapse(neuron, sy1) == FAIL, "Should fail for @neuron->out_synapses_refs = NULL");
	neuron->out_synapses_refs = out_synapses;

	// normal calls
	assert(neuron_add_in_synapse(neuron, sy1) == SUCCESS, "Should work for sy1");
	assert(neuron_add_in_synapse(neuron, sy2) == SUCCESS, "Should work for sy2");
	assert(neuron->in_synapses_refs->length == 2, "@neuron->in_synapses_refs->lenght should be 2 not %u", neuron->in_synapses_refs->length);
	
	// check synapses
	sy = *(Synapse**)vector_get(neuron->in_synapses_refs, 0);
	assert(sy == sy1, "@sy should be %p not %p", sy1, sy);
	assert(memcmp(sy, &sy1_copy, sizeof(Synapse)) == 0, "@sy and @sy1_copy should have the same values");
	sy = *(Synapse**)vector_get(neuron->in_synapses_refs, 1);
	assert(sy == sy2, "@sy should be %p not %p", sy2, sy);
	assert(memcmp(sy, &sy2_copy, sizeof(Synapse)) == 0, "@sy and @sy2_copy should have the same values");

	status = TEST_SUCCESS;

error:
	if (neuron != NULL) {
		neuron_destroy(neuron);
	}
	if (sy1 != NULL) {
		synapse_destroy(sy1);
	}
	if (sy2 != NULL) {
		synapse_destroy(sy2);
	}

	return status;
}


TestStatus neuron_add_out_synapse_test() {
	// setup
	TestStatus status = TEST_FAILED;
	NeuronClass n_class;
	n_class.type = LIF_NEURON;
	neuron_class_set_LIF_parameters(&n_class, LIF_U_TH, LIF_U_REST, LIF_R, LIF_C);
	Neuron* neuron = neuron_create(&n_class);
	Vector* in_synapses = neuron->in_synapses_refs;
	Vector* out_synapses = neuron->out_synapses_refs;

	// create tests synapses
	float w1 = 1.1f;
	float w2 = 3.4f;
	SynapseClass s_class;
	s_class.type = CONDUCTANCE_SYNAPCE;
	Synapse* sy1 = synapse_create(&s_class, w1);
	Synapse* sy2 = synapse_create(&s_class, w2);
	Synapse* sy = NULL;
	Synapse sy1_copy;
	Synapse sy2_copy;
	memcpy(&sy1_copy, sy1, sizeof(Synapse));
	memcpy(&sy2_copy, sy2, sizeof(Synapse));

	// call with @neuron = NULL
	assert(neuron_add_out_synapse(NULL, sy1) == FAIL, "Should fail for @neuron = NULL");

	// call with @neuron->in_synapses_refs = NULL
	neuron->in_synapses_refs = NULL;
	assert(neuron_add_out_synapse(neuron, sy1) == FAIL, "Should fail for @neuron->in_synapses_refs = NULL");
	neuron->in_synapses_refs = in_synapses;

	// call with @neuron->out_synapses_refs = NULL
	neuron->out_synapses_refs = NULL;
	assert(neuron_add_out_synapse(neuron, sy1) == FAIL, "Should fail for @neuron->out_synapses_refs = NULL");
	neuron->out_synapses_refs = out_synapses;

	// normal calls
	assert(neuron_add_out_synapse(neuron, sy1) == SUCCESS, "Should work for sy1");
	assert(neuron_add_out_synapse(neuron, sy2) == SUCCESS, "Should work for sy2");
	assert(neuron->out_synapses_refs->length == 2, "@neuron->in_synapses_refs->lenght should be 2 not %u", neuron->in_synapses_refs->length);

	// check synapses
	sy = *(Synapse**)vector_get(neuron->out_synapses_refs, 0);
	assert(sy == sy1, "@sy should be %p not %p", sy1, sy);
	assert(memcmp(sy, &sy1_copy, sizeof(Synapse)) == 0, "@sy and @sy1_copy should have the same values");
	sy = *(Synapse**)vector_get(neuron->out_synapses_refs, 1);
	assert(sy == sy2, "@sy should be %p not %p", sy2, sy);
	assert(memcmp(sy, &sy2_copy, sizeof(Synapse)) == 0, "@sy and @sy2_copy should have the same values");

	status = TEST_SUCCESS;

error:
	if (neuron != NULL) {
		neuron_destroy(neuron);
	}
	if (sy1 != NULL) {
		synapse_destroy(sy1);
	}
	if (sy2 != NULL) {
		synapse_destroy(sy2);
	}

	return status;
}


TestStatus neuron_step_test() {
	// TODO can you mock the inside?
	return TEST_UNIMPLEMENTED;
}

