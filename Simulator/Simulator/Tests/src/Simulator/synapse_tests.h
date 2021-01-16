#ifndef __SYNAPSE_TESTS_H__
#define __SYNAPSE_TESTS_H__

#include "../testing.h"


TestStatus synapse_class_create_destroy_test();
TestStatus synapse_class_memory_test();

TestStatus synapse_create_destroy_test();
TestStatus synapse_memory_test();
TestStatus synapse_add_spike_time_test();
TestStatus synapse_compute_PSC_test();
TestStatus synapse_step_test();

#endif // __SYNAPSE_TESTS_H__
