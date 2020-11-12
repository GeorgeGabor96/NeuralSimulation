#ifndef __TEST_H__
#define __TEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "debug.h"


#define assert(test, message, ...) check(test, message, ##__VA_ARGS__)
#define FLOAT_PRECISION 0.001
#define float_test(a, b) ((a) < (b) + FLOAT_PRECISION && (a) > (b) - FLOAT_PRECISION)

typedef enum { TEST_SUCCESS = 0, TEST_FAILED = 1, TEST_UNIMPLEMENTED = 2 } TestStatus;


typedef TestStatus (*TestFunction)();

typedef struct TestInfo {
	TestFunction function;
	char* name;
} TestInfo;



#endif // __TEST_H__