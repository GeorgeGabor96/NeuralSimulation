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

	// calls
	NeuronClass* n_class = neuron_class_create(n_type);
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
	return TEST_UNIMPLEMENTED;
}


TestStatus neuron_destroy_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus neuron_add_in_synapse_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus neuron_add_out_synapse_test() {
	return TEST_UNIMPLEMENTED;
}


TestStatus neuron_step_test() {
	return TEST_UNIMPLEMENTED;
}

