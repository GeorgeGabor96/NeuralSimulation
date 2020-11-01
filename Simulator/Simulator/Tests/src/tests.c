#include <stdio.h>

#include "testing.h"
#include "debug.h"

#include "Simulator/simulator_generic_generator_tests.h"



TestInfo tests[] = {
	{ simulator_generic_generator_create_test, "simulator_generic_generator_create_test" },
	{ simulator_generic_array_get_at_index_test, "simulator_generic_array_get_at_index_test" },
	{ simulator_generic_array_set_at_index, "simulator_generic_array_set_at_index" },
	{ simulator_generic_array_append, "simulator_generic_array_append" }
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