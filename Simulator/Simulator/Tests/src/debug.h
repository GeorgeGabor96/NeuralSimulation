#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>

typedef enum { SUCCESS = 0, FAIL = 1 } Status;

#define errno_text() (errno == 0 ? "None" : strerror(errno))

// An error is something that should never happen (ex: invalid index in an array, not enough memory, ...)
#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, errno_text(), ##__VA_ARGS__)

// A warning is something that should not happen, but can be corrected (ex: in a constructor you receive something that is < 0 and you only accept >= 0 and you clip it to 0 and warn the intervention)
#define log_warning(M, ...) fprintf(stderr, "[WARNING] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, errno_text(), ##__VA_ARGS__)

// An info is something of interest that happends in the execution (ex: the execution starts a new processing step and it prints that to inform the user)
#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#ifdef NDEBUG

#define debug(M, ...)
#define check(A, M, ...)
#define check_memory(A)

#else

#define debug(M, ...) fprintf(stderr, "[DEBUG] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define check(A, M, ...) if (!(A)) { log_error(M, ##__VA_ARGS__); errno=0; goto error; }
#define check_memory(A) check((A), "Out of Memory.")

#endif // __NDEBUG__



#endif // __DEBUG_H__