#ifndef __OS__
#define __OS__

#include "config.h"

// currently expects WINDOWS like path
BOOL os_mkdir(const char* dir);

// FAILS if dir not empty or dir doesn't exist
BOOL os_rmdir(const char* dir);
BOOL os_file_exits(const char* file);
void os_print_cwd();
BOOL os_plot_layers(const char* dump_folder);

#endif // __OS__