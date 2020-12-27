#ifndef __SIMULATOR_ARRAY_TESTS__
#define __SIMULATOR_ARRAY_TESTS__

#include "../testing.h"

#include <stdint.h>


TestStatus array_create_test();
TestStatus array_destroy_test();
TestStatus array_set_get_test();
TestStatus array_expand_test();
TestStatus array_show_test();

#endif // __SIMULATOR_ARRAY_TESTS__