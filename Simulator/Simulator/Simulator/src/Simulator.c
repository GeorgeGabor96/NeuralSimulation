#include "Simulator.h"
#include "utils/MemoryManagement.h"


BOOL simulator_is_valid(Simulator* sim) {
	check(sim != NULL, null_argument("sim"));
	check(data_generator_is_valid(sim->data), invalid_argument("sim->data"));
	check(network_is_valid(sim->network), invalid_argument("sim->network"));
	check(array_is_valid(&(sim->callbacks)), invalid_argument("sim->callbacks"));
	
	// check every callback
	uint32_t i = 0;
	Callback* callback = NULL;
	for (i = 0; i < sim->callbacks.length; ++i) {
		callback = (Callback*)array_get(&(sim->callbacks), i);
		check(callback_is_valid(callback) == TRUE, invalid_argument("callback"));
	}

	return TRUE;
ERROR
	return FALSE;
}


Simulator* simulator_create(DataGenerator* data, Network* net) {
	Simulator* sim = NULL;
	Status status = FAIL;
	check(data_generator_is_valid(data) == TRUE, invalid_argument("data"));
	check(network_is_valid(net) == TRUE, invalid_argument("net"));
	
	sim = (Simulator*)malloc(sizeof(Simulator), "simulator_create");
	check_memory(sim);
	status = array_init(&(sim->callbacks), 1, 0, sizeof(Callback));
	check(status == SUCCESS, "Couldn't init array of callbacks");

	sim->data = data;
	sim->network = net;
	return sim;

ERROR
	if (sim != NULL) {
		if (sim->callbacks.data != NULL) array_reset(&(sim->callbacks), NULL);
		free(sim);
	}
	return NULL;
}


void simulator_destroy(Simulator* sim) {
	check(simulator_is_valid(sim) == TRUE, invalid_argument("sim"));

	// free callback, then data generator then the net, because callbacks and data_gen may have references to net
	for (uint32_t i = 0; i < sim->callbacks.length; ++i) {
		Callback* callback = (Callback*)array_get(&(sim->callbacks), i);
		callback_reset(callback);
	}
	array_reset(&(sim->callbacks), NULL);
	data_generator_destroy(sim->data);
	network_destroy(sim->network);
	free(sim);

ERROR
	return;
}


Status simulator_add_callback(Simulator* sim, Callback* call) {
	check(simulator_is_valid(sim) == TRUE, invalid_argument("sim"));
	check(callback_is_valid(call) == TRUE, invalid_argument("call"));

	array_append(&(sim->callbacks), call);
	// take ownership
	free(call);

	return SUCCESS;
ERROR
	return FAIL;
}


static inline void update_callbacks(Simulator* sim) {
	uint32_t callback_idx = 0;
	Callback* callback = NULL;

	for (callback_idx = 0; callback_idx < sim->callbacks.length; ++callback_idx) {
		callback = (Callback*)array_get(&(sim->callbacks), callback_idx);
		check(callback_is_valid(callback) == TRUE, invalid_argument("callback"));
		callback_update(callback, sim->network);
	}
ERROR
	return;
}

static inline void run_callbacks(Simulator* sim) {
	uint32_t callback_idx = 0;
	Callback* callback = NULL;

	for (callback_idx = 0; callback_idx < sim->callbacks.length; ++callback_idx) {
		callback = (Callback*)array_get(&(sim->callbacks), callback_idx);
		check(callback_is_valid(callback) == TRUE, invalid_argument("callback"));
		callback_run(callback, sim->network);
	}
	ERROR
		return;
}


void simulator_infer(Simulator* sim) {
	check(simulator_is_valid(sim) == TRUE, invalid_argument("sim"));

	DataElement* element = NULL;
	NetworkInputs* inputs = NULL;
	uint32_t elem_idx = 0;
	uint32_t time = 0;


	printf("-------------------------\n");
	printf("STARTING INFERENCE\n");
	for (elem_idx = 0; elem_idx < sim->data->length; ++elem_idx) {
		printf("Example %d\n", elem_idx);
		element = data_generator_get_element(sim->data, elem_idx);
		check(data_element_is_valid(element) == TRUE, invalid_argument("element"));

		network_clear_state(sim->network);
		for (time = 0; time < element->duration; ++time) {
			printf("%u\r", time);
			inputs = data_element_get_values(element, time);
			check(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
			
			network_step(sim->network, inputs, time);
			
			update_callbacks(sim);
			
			data_element_remove_values(element, inputs);
		}
		data_element_destroy(element);

		run_callbacks(sim);
	}
	printf("-------------------------\n");

ERROR
	return;
}

void simulator_train(Simulator* sim) {
	// TODO: has the concept of epoch
	log_warning("UNIMPLEMENTED METHOD");
	return;
}

void simulator_test(Simulator* sim) {
	log_warning("UNIMPLEMENTED METHOD");
	return;
}