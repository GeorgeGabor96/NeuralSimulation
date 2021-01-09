#ifndef __TEST_H__
#define __TEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "MemoryManagement.h"

#define assert(test, message, ...) if (!(test)) { fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " message "\n", __FILE__, __LINE__, errno_text(), ##__VA_ARGS__); errno=0; goto error; }
#define FLOAT_PRECISION 0.001
#define float_test(a, b) ((a) < (b) + FLOAT_PRECISION && (a) > (b) - FLOAT_PRECISION)

typedef enum { TEST_SUCCESS = 0, TEST_FAILED = 1, TEST_UNIMPLEMENTED = 2 } TestStatus;

typedef TestStatus (*TestFunction)();

typedef struct TestInfo {
	TestFunction function;
	char* name;
} TestInfo;


static inline bool memory_leak() {
	bool leak = memory_manage_is_empty();
	if (leak == FALSE) memory_manage_report();
	return leak;
}

#endif // __TEST_H__