#include "Containers.h"

#define STRING_LIMIT 256


static inline size_t safe_strlen(char* string) {
	size_t i = 0;
	for (i = 0; i < STRING_LIMIT; ++i) {
		if (string[i] == 0) {
			break;
		}
	}
	if (i == STRING_LIMIT) {
		string[i - 1] = 0;
	}
	return i + 1;
}


Array* string_create(char* c_string) {
	size_t n_chars = safe_strlen(c_string);
	Array* string = array_create((uint32_t)n_chars, sizeof(char));
	check_memory(string);

	array_copy_data(string, c_string, 0, (uint32_t)n_chars);

	return string;

error:
	return NULL;
}


void string_destroy(Array* array) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	array_destroy(array, NULL);

error:
	return;
}


Vector* string_vector_create(char** strings, uint32_t cnt) {
	uint32_t i = 0;
	Vector* safe_strings = vector_create(cnt, sizeof(Array*));
	check_memory(safe_strings);
	Array* string = NULL;
	
	for (i = 0; i < cnt; ++i) {
		string = string_create(strings[i]);
		vector_append(safe_strings, &string);
	}

	return safe_strings;

error:
	--i;
	// because unsigned
	while (i < cnt) {
		string = *((Array**)vector_get(safe_strings, i));
		string_destroy(string);
		--i;
	}

	return NULL;
}


void string_vector_destroy(Vector* strings) {
	check(vector_is_valid(strings) == TRUE, invalid_argument("strings"));
	uint32_t i = 0;
	Array* string = NULL;

	for (i = 0; i < strings->length; ++i) {
		string = *((Array**)vector_get(strings, i));
		string_destroy(string);
	}

error:
	return;
}


int string_compare(Array* string1, Array* string2) {
	check(array_is_valid(string1), invalid_argument("string1"));
	check(array_is_valid(string2), invalid_argument("string2"));
	check(string1->element_size == string2->element_size, "@string1->element_size != @string2->element_size");
	check(string1->length == string2->length, "@string1->length != @string2->length");
	
	return memcmp(string1->data, string2->data, string1->element_size * string1->length);
error:
	return -1;
}