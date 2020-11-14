#ifndef __NEURON_TESTS_H__
#define __NEURON_TESTS_H__

#include "../testing.h"


TestStatus neuron_class_create_test();
TestStatus neuron_class_destroy_test();
TestStatus neuron_class_set_LIF_parameters_test();

TestStatus neuron_create_test();
TestStatus neuron_destroy_test();
TestStatus neuron_add_in_synapse_test();
TestStatus neuron_add_out_synapse_test();
TestStatus neuron_step_test();


#endif // __NEURON_TESTS_H__