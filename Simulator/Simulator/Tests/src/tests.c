#include <stdio.h>

#include "testing.h"
#include "debug.h"

#include "Simulator/memory_management_tests.h"

// need rework
#include "Simulator/array_tests.h"
#include "Simulator/string_tests.h"
#include "Simulator/stack_tests.h"

#include "Simulator/queue_tests.h"
#include "Simulator/synapse_tests.h"
#include "Simulator/neuron_tests.h"
#include "Simulator/layer_tests.h"
#include "Simulator/network_tests.h"


TestInfo tests[] = {
	{ memory_management_general_test, "memory_management_general_test" },
	{ memory_manage_report_test, "memory_manage_report_test" },

	// ARRAY
	{ array_general_use_case_test, "array_general_use_case_test" },
	{ array_memory_test, "array_memory_test" },
	{ array_expand_test, "array_expand_test" },
	{ array_show_test, "array_show_test" },
	{ array_copy_data_test, "array_copy_data_test" },
	{ array_swap_test, "array_swap_test" },

	// String
	{ string_create_destroy_test, "string_create_destroy_test" },
	{ strings_create_destroy_test, "strings_create_destroy_test" },
	{ string_memory_stress_test, "string_memory_stress_test" },

	// STACK
	{ stack_general_use_case_test, "stack_general_use_case_test" },
	{ stack_memory_test, "stack_memory_test" },

	// QUEUE
	{ queue_general_use_case_test, "queue_general_use_case_test" },
	{ queue_memory_test, "queue_memory_test" },

	// SYNAPSE
	{ synapse_class_create_destroy_test, "synapse_class_create_destroy_test" },
	{ synapse_class_memory_test, "synapse_class_memory_test" },
	{ synapse_create_destroy_test, "synapse_create_destroy_test" },
	{ synapse_class_memory_test, "synapse_class_memory_test" },
	{ synapse_add_spike_time_test, "synapse_add_spike_time_test" },
	{ synapse_compute_PSC_test, "synapse_compute_PSC_test" },
	{ synapse_step_test, "synapse_step_test" },
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