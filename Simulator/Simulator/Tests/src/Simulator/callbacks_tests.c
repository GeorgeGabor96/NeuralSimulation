#include "callbacks_tests.h"

#include "utils/os.h"
#include "callbacks/callback.h"
#include "callbacks/callback_dump.h"


TestStatus callbacks_dump_layer_neurons_test() {
	NeuronClass* n_class = neuron_class_create("TEST NEURON", LIF_NEURON);
	Layer* layer = layer_create_fully_connected(10, n_class, "layer_visu");
	Callback* callback_visu = callback_dump_layer_neurons_create(layer, ".\\callback_visu", FALSE);
	ArrayFloat* constant_current = array_ones_float(10);
	assert(callback_is_valid(callback_visu) == TRUE, invalid_argument("callback"));

	for (uint32_t i = 0; i < 100; ++i) {
		layer_step_inject_currents(layer, constant_current, i);
		callback_update(callback_visu, NULL);
	}
	callback_run(callback_visu, NULL);

	callback_destroy(callback_visu);
	layer_destroy(layer);
	neuron_class_destroy(n_class);
	array_destroy(constant_current, NULL);
	
	os_rmdir(".\\callback_visu");

	assert(memory_leak() == FALSE, "Memory leak");
	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}
