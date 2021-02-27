#ifndef __CONFIG__
#define __CONFIG__

#define MEMORY_MANAGE
#define CHECKS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "utils/debug.h"

typedef uint8_t BOOL;
#define TRUE 1ui8
#define FALSE 0ui8

// status type definition
typedef enum { SUCCESS = 0, FAIL = 1 } Status;


#endif // __CONFIG__
