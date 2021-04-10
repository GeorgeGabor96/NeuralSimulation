#include <stdio.h>

#include "testing.h"
#include "config.h"

#include "Simulator/memory_management_tests.h"
#include "Simulator/os_tests.h"
#include "Simulator/array_tests.h"
#include "Simulator/string_tests.h"
#include "Simulator/stack_tests.h"
#include "Simulator/queue_tests.h"
#include "Simulator/synapse_tests.h"
#include "Simulator/neuron_tests.h"
#include "Simulator/layer_tests.h"
#include "Simulator/network_tests.h"
#include "Simulator/callbacks_tests.h"
#include "Simulator/data_gen_tests.h"
#include "Simulator/simulator_tests.h"


// TODO: some tests take time, mark them and set a flag to run them or not
TestInfo tests[] = {

	// simulator
	{ simulator_infer_test, "simulator_infer_test" },

	// data generators
	{ data_generator_spike_pulses, "data_generator_spike_pulses" },
	{ data_generator_constant_current_test, "data_generator_constant_current_test" },
	{ data_generator_random_spikes_test, "data_generator_random_spikes_test" },
	{ data_generator_with_step_between_neurons, "data_generator_with_step_between_neurons" },

	// callbacks
	{ callback_dump_layer_neurons_test, "callbacks_dump_layer_neurons_test" },
	{ callback_dump_network_test, "callback_dump_network_test" },

	// OS
	{ os_mkdir_rmdir_test, "os_mkdir_rmdir_test" },
	{ os_file_exits_test, "os_file_exits_test" },

	// NETWORK
	{ network_summary_test, "network_summary_test" },
	{ network_compile_general_use_case_test, "network_compile_general_use_case_test" },
	{ network_step_test, "network_step_test" },

	// LAYER
	{ layer_general_use_case_test, "layer_general_use_case_test" },
	{ layer_memory_test_test, "layer_memory_test_test" },
	{ layer_fully_connected_test, "layer_fully_connected_test" },
	{ layer_fully_link_input_layer_test, "layer_fully_link_input_layer_test" },
	{ layer_get_min_byte_size_test, "layer_get_min_byte_size_test" },

	// NEURON
	{ neuron_LIF_refractor_general_test, "neuron_LIF_refractor_general_test" },
	{ neuron_general_use_case_test, "neuron_general_use_case_test" },
	{ neuron_memory_test, "neuron_memory_test" },
	{ neuron_get_min_byte_size_test, "neuron_get_min_byte_size_test" },
	
	// SYNAPSE
	{ synapse_class_create_destroy_test, "synapse_class_create_destroy_test" },
	{ synapse_create_destroy_test, "synapse_create_destroy_test" },
	{ synapse_class_memory_test, "synapse_class_memory_test" },
	{ synapse_add_spike_time_test, "synapse_add_spike_time_test" },
	{ synapse_compute_PSC_test, "synapse_compute_PSC_test" },
	{ synapse_step_test, "synapse_step_test" },
	{ synapse_class_memory_test, "synapse_class_memory_test" },
	{ synapse_get_min_byte_size_test, "synapse_get_min_byte_size_test" },

	// STACK
	{ stack_general_use_case_test, "stack_general_use_case_test" },
	{ stack_memory_test, "stack_memory_test" },

	// String
	{ string_create_destroy_test, "string_create_destroy_test" },
	{ strings_create_destroy_test, "strings_create_destroy_test" },
	{ string_memory_stress_test, "string_memory_stress_test" },
	{ string_path_join_test, "string_path_join_test" },

	// ARRAY
	{ array_general_use_case_test, "array_general_use_case_test" },
	{ array_expand_test, "array_expand_test" },
	{ array_show_test, "array_show_test" },
	{ array_copy_data_test, "array_copy_data_test" },
	{ array_swap_test, "array_swap_test" },
	{ array_memory_test, "array_memory_test" },
	{ array_get_byte_min_size_test, "array_get_byte_min_size_test" },

	// MEMORY MANAGEMENT
	{ memory_management_general_test, "memory_management_general_test" },
	{ memory_manage_report_test, "memory_manage_report_test" },
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

	log_info("Number of RUNNED tests: %u", number_tests_runned);
	log_info("Number of SUCCESS tests: %u", number_tests_success);
	log_info("Number of FAILED tests: %u", number_tests_failed);
	log_info("Number of UNIMPLEMENTED tests: %u", number_tests_unimplemented);

	return 0;
}