#include "string_tests.h"
#include "Containers.h"


TestStatus string_create_destroy_test() {
	TestStatus status = TEST_FAILED;
	char* c_string = "Test string";
	Array* string = string_create(c_string);
	assert(string != NULL, null_argument("string"));
	assert(string->element_size == sizeof(char), invalid_argument("string->element_size"));
	assert(string->length == strlen(c_string) + 1, invalid_argument("string->length"));
	assert(memcmp(string->data, c_string, string->length * string->element_size) == 0, invalid_argument("string->data"));

	string_destroy(string);

	status = TEST_SUCCESS;

error:

	return status;
}


TestStatus string_vector_create_destroy_test() {
	TestStatus status = TEST_FAILED;

	char* c_s1 = "test 1";
	char* c_s2 = "test 2";
	char* c_s3 = "test 3";
	char* c_strings[2] = { c_s1, c_s2 };

	Array* strings = strings_create(c_strings, 2);
	Array* string = string_create(c_s3);
	array_append(strings, &string);

	string = *((Array**)array_get(strings, 0));
	assert(string != NULL, null_argument("string"));
	assert(memcmp(string->data, c_s1, string->length * string->element_size) == 0, invalid_argument("string->data"));

	string = *((Array**)array_get(strings, 1));
	assert(string != NULL, null_argument("string"));
	assert(memcmp(string->data, c_s2, string->length * string->element_size) == 0, invalid_argument("string->data"));

	string = *((Array**)array_get(strings, 2));
	assert(string != NULL, null_argument("string"));
	assert(memcmp(string->data, c_s3, string->length * string->element_size) == 0, invalid_argument("string->data"));

	strings_destroy(strings);

	status = TEST_SUCCESS;

error:


	return status;
}
