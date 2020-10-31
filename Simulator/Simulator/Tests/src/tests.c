#include <stdio.h>

#include "test.h"
#include "debug.h"

#include "Simulator/simulator_generic_generator_tests.h"



TestInfo tests[] = {
	{ simulator_generic_generator_create_test, "simulator_generic_generator_create_test" },
	{ simulator_generic_generator_create_test, "simulator_generic_generator_create_test" }
};


int main() {
	uint32_t number_tests_runned = 0;
	uint32_t number_tests_success = 0;
	uint32_t i = 0;
	uint32_t number_of_test = sizeof(tests) / sizeof(TestInfo);

	for (i = 0; i < number_of_test; i++) {
		log_info("Running %s", tests[i].name);
		number_tests_runned++;
		check(tests[i].function() == 0, "%s failed\n", tests[i].name)
		number_tests_success++;
	}

error:
	log_info("Number of RUNNED tests: %d", number_tests_runned);
	log_info("Number of SUCCESS tests: %d", number_tests_success);
	log_info("Number of FAILED tests: %d", number_tests_runned - number_tests_success);

	return 0;
}