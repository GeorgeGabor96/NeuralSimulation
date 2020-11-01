#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "[DEBUG] %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif // __NDEBUG__

#define errno_text() (errno == 0 ? "None" : strerror(errno))

#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, errno_text(), ##__VA_ARGS__)

#define log_warning(M, ...) fprintf(stderr, "[WARNING] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, errno_text(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define check(A, M, ...) if (!(A)) { log_error(M, ##__VA_ARGS__); errno=0; goto error; }

#define check_memory(A) check((A), "Out of Memory.")


#endif // __DEBUG_H__