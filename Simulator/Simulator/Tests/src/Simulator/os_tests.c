#include "os_tests.h"

#include "os.h"


TestStatus os_mkdir_rmdir_test() {
	// try to delete not exsistent dir
	BOOL success = os_rmdir(".\\non_existent");
	assert(success == FALSE, "Should fail for non-existent dir");

	// make a dir and delete it
	success = os_mkdir(".\\non_existent");
	assert(success == TRUE, "Should create dir");
	
	success = os_mkdir(".\\non_existent");
	assert(success == TRUE, "Should not fail for already existing dir");

	success = os_rmdir(".\\non_existent");
	assert(success == TRUE, "Should delete dir");

	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}


TestStatus os_file_exits_test() {
	remove(".\\file.txt");
	BOOL success = os_file_exits(".\\file.txt");
	assert(success == FALSE, "file should not exit");

	// create empty file
	FILE* f = fopen(".\\file.txt", "w");
	fclose(f);

	success = os_file_exits(".\\file.txt");
	assert(success == TRUE, "file should exit");
	remove(".\\file.txt");

	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}