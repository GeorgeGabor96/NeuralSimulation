#ifndef __MEMORY_MANAGEMENT__
#define __MEMORY_MANAGEMENT__

#include "debug.h"

#ifdef MEMORY_MANAGE
#define malloc(size, desc)			memory_manage_malloc(size, desc)
#define calloc(nitems, size, desc)	memory_manage_calloc(nitems, size, desc)
#define realloc(ptr, size, desc)	memory_manage_realloc(ptr, size, desc)
#define free(ptr)					memory_manage_free(ptr)

void* memory_manage_malloc(size_t size, char* desc);
void* memory_manage_calloc(size_t nitems, size_t size);
void* memory_manage_realloc(void* ptr, size_t size);
void  memory_manage_free(void* ptr);

#else
#define malloc(s_data, desc) malloc(size)
#define calloc(n_elem, s_elem, desc) calloc(n_elem, s_elem)
#define realloc(p, s_data, desc) realloc(p, s_data)
#define free(p) free(p)
#endif

#endif // __MEMORY_MANANGEMENT__
