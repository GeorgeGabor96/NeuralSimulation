#ifndef __NEURON_TESTS_H__
#define __NEURON_TESTS_H__

#include "../testing.h"


TestStatus neuron_class_general_use_case_test();
TestStatus neuron_class_memory_test();

TestStatus neuron_general_use_case_test();
TestStatus neuron_memory_test();

TestStatus neuron_LIF_refractor_general_test();
TestStatus neuron_get_min_byte_size_test();

#endif // __NEURON_TESTS_H__