#include "utils/os.h"
#include <stdlib.h>

#define COMMAND_MAX_LENGTH 256

BOOL os_mkdir(const char* dir) {
	char mkdir_command[COMMAND_MAX_LENGTH] = { 0 };
	sprintf(mkdir_command, "if not exist %s mkdir %s", dir, dir);
	if (system(mkdir_command) == 0) return TRUE;
	return FALSE;
}

BOOL os_rmdir(const char* dir) {
	char rmdir_command[COMMAND_MAX_LENGTH] = { 0 };
	// /S - deletes even if not empty
	// /Q - does not ask for permission to delete
	sprintf(rmdir_command, "rmdir /Q /S \"%s\"", dir);
	if (system(rmdir_command) == 0) return TRUE;
	return FALSE;
}

BOOL os_file_exits(const char* file) {
	char exits_command[COMMAND_MAX_LENGTH] = { 0 };
	sprintf(exits_command, "if exist %s (exit 1) else (exit 0)", file);
	if (system(exits_command) == 1) return TRUE;
	return FALSE;
}