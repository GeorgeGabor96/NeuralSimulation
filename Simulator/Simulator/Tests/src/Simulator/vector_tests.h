#ifndef __VECTOR_TESTS_H__
#define __VECTOR_TESTS_H__


#include "../testing.h"


TestStatus vector_create_test();
TestStatus vector_destroy_test();
TestStatus vector_append_test();

// !!!!! NO NEED TO test vector_get and vector_set because they are just an array_set and array_get which are tested


#endif // __VECTOR_TESTS_H__