#include <stdio.h>

#include "testing.h"
#include "debug.h"

#include "Simulator/array_tests.h"
#include "Simulator/string_tests.h"
#include "Simulator/stack_tests.h"
#include "Simulator/queue_tests.h"
#include "Simulator/synapse_tests.h"
#include "Simulator/vector_tests.h"
#include "Simulator/neuron_tests.h"
#include "Simulator/layer_tests.h"
#include "Simulator/network_tests.h"


TestInfo tests[] = {

	// ARRAY
	{ array_create_test, "array_create_test" },
	{ array_destroy_test, "array_destroy_test" },
	{ array_set_get_test, "array_set_get_test" },
	{ array_expand_test, "array_expand_test" },
	{ array_show_test, "array_show_test" },

	// STRING
	{ string_create_destroy_test, "string_create_destroy_test" },
	{ string_vector_create_destroy_test, "string_vector_create_destroy_test" },

	// STACK
	{ stack_create_test, "stack_create_test" },
	{ stack_destroy_test, "stack_destroy_test" },
	{ stack_push_test, "stack_push_test" },
	{ stack_pop_test, "stack_pop_test" },
	{ stack_top_test, "stack_top_test" },

	// QUEUE
	{ queue_create_test, "queue_create_test" },
	{ queue_destroy_test, "queue_destroy_test" },
	{ queue_enqueue_test, "queue_enqueue_test" },
	{ queue_dequeue_test, "queue_dequeue_test" },
	{ queue_head_test, "queue_head_test" },

	// VECTOR
	{ vector_create_test, "vector_create_test" },
	{ vector_destroy_test, "vector_destroy_test" },
	{ vector_append_get_set_test, "vector_append_get_set_test" },
	{ vector_show_test, "vector_show_test" },

	// SYNAPSE --> TODO: Need to be rethinked
	{ synapse_class_create_test, "synapse_class_create_test" },
	{ synapse_class_destroy_test, "synapse_class_destroy_test" },
	{ synapse_create_test, "synapse_create_test" },
	{ synapse_destroy_test, "synapse_destroy_test" },
	{ synapse_add_spike_time_test, "synapse_add_spike_time_test" },
	{ synapse_compute_PSC_test, "synapse_compute_PSC_test" },
	{ synapse_step_test, "synapse_step_test" },

	// NEURON --> TODO: Need to be rethinked
	{ neuron_class_create_test, "neuron_class_create_test" },
	{ neuron_class_destroy_test, "neuron_class_destroy_test" },
	{ neuron_class_set_LIF_parameters_test, "neuron_class_set_LIF_parameters_test" },
	{ neuron_create_test, "neuron_create_test" },
	{ neuron_destroy_test, "neuron_destroy_test" },
	{ neuron_add_in_synapse_test, "neuron_add_in_synapse_test" },
	{ neuron_add_out_synapse_test, "neuron_add_out_synapse_test" },
	{ neuron_step_test, "neuron_step_test" },

	// LAYER
	{ layer_create_destroy_test, "layer_create_destroy_test" },
	{ layer_step_test, "layer_step_test" },

	// NETWORK
	//{ network_step_test, "network_step_test" }
};


int main() {
	uint32_t number_tests_runned = 0;
	uint32_t number_tests_success = 0;
	uint32_t number_tests_failed = 0;
	uint32_t number_tests_unimplemented = 0;
	uint32_t i = 0;
	uint32_t number_of_test = sizeof(tests) / sizeof(TestInfo);
	TestStatus status = TEST_SUCCESS;

	for (i = 0; i < number_of_test; i++) {
		log_info("Running %s", tests[i].name);
		number_tests_runned++;

		status = tests[i].function();
		if (status == TEST_SUCCESS) {
			log_info("Test %s SUCCESS\n", tests[i].name);
			number_tests_success++;
		}
		else if (status == TEST_FAILED) {
			log_error("Test %s FAILED\n", tests[i].name);
			number_tests_failed++;
			break;
		}
		else if (status == TEST_UNIMPLEMENTED) {
			log_warning("Test %s UNIMPLEMENTED\n", tests[i].name);
			number_tests_unimplemented++;
		}
		else {
			log_error("Test %s return UNKNOWN status\n", tests[i].name);
			break;
		}
	}

	log_info("Number of RUNNED tests: %d", number_tests_runned);
	log_info("Number of SUCCESS tests: %d", number_tests_success);
	log_info("Number of FAILED tests: %d", number_tests_failed);
	log_info("Number of UNIMPLEMENTED tests: %d", number_tests_unimplemented);

	return 0;
}