#ifndef __SIMULATOR_STACK_TESTS__
#define __SIMULATOR_STACK_TESTS__

#include "../testing.h"

#include <stdint.h>


TestStatus stack_create_test();
TestStatus stack_destroy_test();
TestStatus stack_push_test();
TestStatus stack_pop_test();
TestStatus stack_top_test();


#endif // __SIMULATOR_STACK_TESTS__