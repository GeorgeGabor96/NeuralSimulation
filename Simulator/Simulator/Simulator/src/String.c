#include "Containers.h"
#include "MemoryManagement.h"


/*************************************************************
* Check Functions
*************************************************************/
BOOL string_is_valid(String* string) {
	check(array_is_valid(string) == TRUE, invalid_argument("string"));
	check(string->element_size == 1, "@string->element_size != 1");
	return TRUE;
ERROR
	return FALSE;
}


/*************************************************************
* Helper Functions
*************************************************************/
static inline uint32_t safe_strlen(char* c_string_p) {
	uint32_t i = 0;
	for (i = 0; i < STRING_LIMIT; ++i) {
		if (c_string_p[i] == 0) {
			break;
		}
	}
	return i;
}


/*************************************************************
* String Functionality
*************************************************************/
// UNTESTED
Status string_init(String* string_p, char* c_string_p) {
	Status status = FAIL;
	check(string_p != NULL, null_argument("string_p"));
	check(c_string_p != NULL, null_argument("c_string_p"));

	uint32_t n_chars = safe_strlen(c_string_p);
	status = array_init(string_p, n_chars + 1, n_chars, sizeof(char));
	check(status == SUCCESS, "array_init failed");
	array_copy_data(string_p, c_string_p, 0, n_chars);

	// didn't put end of string
	char end_s = '\0';
	array_append(string_p, &end_s);

ERROR
	return status;
}


String* string_create(char* c_string_p) {
	// make a blank string
	String* string_p = (String*)malloc(sizeof(String), "string_create");
	check_memory(string_p);
	Status status = string_init(string_p, c_string_p);
	check(status == SUCCESS, "string_init failed");
	return string_p;

ERROR
	if (string_p != NULL) free(string_p);
	return NULL;
}


void string_reset(String* string_p) {
	check(array_is_valid(string_p) == TRUE, invalid_argument("string_p"));
	array_reset(string_p, NULL);

ERROR
	return;
}


void string_destroy(String* string_p) {
	check(array_is_valid(string_p) == TRUE, invalid_argument("string_p"));
	array_destroy(string_p, NULL);

ERROR
	return;
}


Array* strings_create(char** strings_pp, uint32_t cnt) {
	uint32_t i = 0;
	Array* safe_strings_p = NULL;
	String* string_p = NULL;

	check(cnt > 0, "@cnt == 0");
	check(strings_pp != NULL, null_argument("strings_pp"));

	// because strings are variable in size, need to save the pointers
	safe_strings_p = array_create(cnt, 0, sizeof(String*));
	check_memory(safe_strings_p);
	
	for (i = 0; i < cnt; ++i) {
		string_p = string_create(strings_pp[i]);
		check_memory(string_p);
		array_append(safe_strings_p, &string_p);
	}
	return safe_strings_p;

ERROR
	if (safe_strings_p != NULL) {
		--i; // because unsigned
		String** string_pp = NULL;
		while (i < cnt) {
			string_pp = ((String**)array_get(safe_strings_p, i));
			string_destroy(*string_pp);
			--i;
		}
	}
	return NULL;
}


char* string_get_C_string(String* string) {
	check(string_is_valid(string) == TRUE, invalid_argument("string"));
	return string->data;
ERROR
	return NULL;
}


String* string_copy(String* string) {
	return string_create(string->data);
}


void strings_destroy(Array* strings_p) {
	check(array_is_valid(strings_p) == TRUE, invalid_argument("strings_p"));
	uint32_t i = 0;
	String* string_p = NULL;

	// array contains pointers so we can't make a function to reset them
	for (i = 0; i < strings_p->length; ++i) {
		string_p = *((String**)array_get(strings_p, i));
		check(array_is_valid(string_p) == TRUE, invalid_argument("string_pp"));
		string_destroy(string_p);
	}
	array_destroy(strings_p, NULL);
ERROR
	return;
}


BOOL string_equal(String* string1_p, String* string2_p) {
	check(string_is_valid(string1_p), invalid_argument("string1_p"));
	check(string_is_valid(string2_p), invalid_argument("string2_p"));
	
	if (string1_p->length != string2_p->length) return FALSE;
	if (memcmp(string1_p->data, string2_p->data, string1_p->element_size * string1_p->length) == 0) return TRUE;
ERROR
	return FALSE;
}


// function for working with paths
#define MAX_PATH_LENGTH 256

String* string_path_join_strings(String* string1, String* string2) {
	check(string_is_valid(string1) == TRUE, invalid_argument("string1"));
	check(string_is_valid(string2) == TRUE, invalid_argument("string2"));
	char path[MAX_PATH_LENGTH] = { 0 };
	sprintf(path, "%s\\%s", (const char*)string1->data, (const char*)string2->data);
	return string_create(path);
ERROR
	return NULL;
}

String* string_path_join_string_and_C(String* string1, const char* string2) {
	check(string2 != NULL, null_argument("string2"));
	String s;
	string_init(&s, string2);
	String* string_join = string_path_join_strings(string1, &s);
	string_reset(&s);

	return string_join;
ERROR
	return NULL;
}