#include "Containers.h"


static inline uint32_t safe_strlen(char* c_string_p) {
	uint32_t i = 0;
	for (i = 0; i < STRING_LIMIT; ++i) {
		if (c_string_p[i] == 0) {
			break;
		}
	}
	return i;
}


String* string_create(char* c_string_p) {
	check(c_string_p != NULL, null_argument("c_string_p"));
	uint32_t n_chars = safe_strlen(c_string_p);
	String* string_p = array_create(n_chars + 1, n_chars, sizeof(char));
	check_memory(string_p);
	array_copy_data(string_p, c_string_p, 0, n_chars);

	// didn't put end of string
	char end_s = '\0';
	array_append(string_p, &end_s);

	return string_p;

ERROR
	return NULL;
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


int string_compare(String* string1_p, String* string2_p) {
	check(array_is_valid(string1_p), invalid_argument("string1_p"));
	check(array_is_valid(string2_p), invalid_argument("string2_p"));
	check(string1_p->length == string2_p->length, "@string1_p->length != @string2_p->length");
	check(string1_p->max_length == string2_p->max_length, "@string1_p->max_length != @string2_p->max_length");
	check(string1_p->element_size == string2_p->element_size, "@string1_p->element_size != @string2_p->element_size");

	return memcmp(string1_p->data, string2_p->data, string1_p->element_size * string1_p->length);
ERROR
	return -1;
}