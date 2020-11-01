#ifndef __SIMULATOR_TESTS__
#define __SIMULATOR_TESTS__

#include "../testing.h"

#include <stdint.h>


TestStatus simulator_generic_generator_create_test();
TestStatus simulator_generic_array_get_at_index_test();
TestStatus simulator_generic_array_set_at_index();
TestStatus simulator_generic_array_append();


#endif // __SIMULATOR_TESTS__