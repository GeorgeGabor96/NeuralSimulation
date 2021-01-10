#include "string_tests.h"
#include "Containers.h"


TestStatus string_create_destroy_test() {
	TestStatus status = TEST_FAILED;
	char* c_string_p = "Test string";
	String* string_p = string_create(c_string_p);
	assert(string_p != NULL, null_argument("string_p"));
	assert(string_p->element_size == sizeof(char), "@string_p->element_size is %llu, not %lluu", string_p->element_size, sizeof(char));
	assert(string_p->length == strlen(c_string_p) + 1, "@string_p->length is %u, not %llu", string_p->length, strlen(c_string_p) + 1);
	assert(string_p->max_length == strlen(c_string_p) + 1, "@string_p->max_length is %u, not %llu", string_p->max_length, strlen(c_string_p) + 1);
	assert(memcmp(string_p->data, c_string_p, array_data_size(string_p)) == 0, invalid_argument("string_p->data"));

	string_destroy(string_p);
	assert(memory_leak() == TRUE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus strings_create_destroy_test() {
	TestStatus status = TEST_FAILED;

	char* c_s1_p = "test 1";
	char* c_s2_p = "test 2";
	char* c_s3_p = "test 3";
	char* c_strings_pp[2] = { c_s1_p, c_s2_p };

	Array* strings_p = strings_create(c_strings_pp, 2);
	String* string_p = string_create(c_s3_p);
	String** string_pp = NULL;
	array_append(&strings_p, &string_p);

	string_pp = (String**)array_get(strings_p, 0);
	assert(string_pp != NULL, null_argument("string_pp"));
	assert(*string_pp != NULL, null_argument("*string_pp"));
	assert(memcmp((*string_pp)->data, c_s1_p, array_data_size(*string_pp)) == 0, invalid_argument("(*string_pp)->data"));

	string_pp = (String**)array_get(strings_p, 1);
	assert(string_pp != NULL, null_argument("string_pp"));
	assert(*string_pp != NULL, null_argument("*string_pp"));
	assert(memcmp((*string_pp)->data, c_s2_p, array_data_size(*string_pp)) == 0, invalid_argument("(*string_pp)->data"));

	string_pp = (String**)array_get(strings_p, 2);
	assert(string_pp != NULL, null_argument("string_pp"));
	assert(*string_pp != NULL, null_argument("*string_pp"));
	assert(memcmp((*string_pp)->data, c_s3_p, array_data_size(*string_pp)) == 0, invalid_argument("(*string_pp)->data"));

	strings_destroy(strings_p);
	assert(memory_leak() == TRUE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}


TestStatus string_memory_stress_test() {
	TestStatus status = TEST_FAILED;

	// string_create
	String* string_pp[1000] = { NULL };
	uint32_t i = 0;
	for (i = 0; i < 1000; ++i) string_pp[i] = string_create("Ana has apples");
	for (i = 0; i < 1000; ++i) string_destroy(string_pp[i]);
	assert(memory_leak() == TRUE, "Memory leak");

	// strings_create
	Array* strings_pp[1000] = { NULL };
	char* c_strings_pp[2] = { "Ana has apples", "George likes Ana" };
	for (i = 0; i < 1000; ++i) strings_pp[i] = strings_create(c_strings_pp, 2);
	for (i = 0; i < 1000; ++i) strings_destroy(strings_pp[i]);
	assert(memory_leak() == TRUE, "Memory leak");

	status = TEST_SUCCESS;
error:
	return status;
}
