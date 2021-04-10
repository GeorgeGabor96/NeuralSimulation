#ifndef __NETWORK_TESTS_H__
#define __NETWORK_TESTS_H__

#include "../testing.h"


TestStatus network_compile_general_use_case_test();
TestStatus network_step_test();
TestStatus network_summary_test();
TestStatus network_get_min_byte_size_test();

#endif // __NETWORK_TESTS_H__