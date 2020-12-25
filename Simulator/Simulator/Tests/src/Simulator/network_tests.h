#ifndef __NETWORK_TESTS_H__
#define __NETWORK_TESTS_H__

#include "../testing.h"


TestStatus network_create_test();
TestStatus network_destroy_test();
TestStatus network_is_ready_test();
TestStatus network_compile_test();
TestStatus network_get_layer_by_idx_test();
TestStatus network_add_layer_test();
TestStatus network_step_test();

#endif // __NETWORK_TESTS_H__