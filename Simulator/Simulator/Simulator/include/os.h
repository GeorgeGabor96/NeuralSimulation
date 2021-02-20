#ifndef __OS__
#define __OS__

#include "config.h"

// currently expects WINDOWS like path
BOOL os_mkdir(const char* dir);

// FAILS if dir not empty or dir doesn't exist
BOOL os_rmdir(const char* dir);
BOOL os_file_exits(const char* file);

#endif // __OS__