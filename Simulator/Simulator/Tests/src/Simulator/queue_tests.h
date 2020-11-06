#ifndef __SIMULATOR_QUEUE_TESTS__
#define __SIMULATOR_QUEUE_TESTS__

#include "../testing.h"

#include <stdint.h>


TestStatus queue_create_test();
TestStatus queue_destroy_test();
TestStatus queue_enqueue_test();
TestStatus queue_dequeue_test();
TestStatus queue_head_test();


#endif // __SIMULATOR_QUEUE_TESTS__
