#include <stdlib.h>
#include <math.h>

#include "Containers.h"
#include "utils/MemoryManagement.h"


/*************************************************************
* CHECKS FUNCTIONS
*************************************************************/
BOOL array_is_valid(Array* array) {
	check(array != NULL, null_argument("array"));
	check(array->length <= array->max_length, "@array->length > @array->max_length");
	check(array->max_length > 0, "@array->max_length == 0");
	check(array->element_size > 0, "@array->element_size == 0");
	check(array->data != NULL, null_argument("array->data"));

	return TRUE;
ERROR
	return FALSE;
}


/*************************************************************
* Array Functionality
*************************************************************/
Array* array_create(uint32_t length, uint32_t initial_length, size_t element_size) {
	Array* array = NULL;
	Status status = FAIL;

	array = (Array*)calloc(1, sizeof(Array), "array_create");
	check_memory(array);

	status = array_init(array, length, initial_length, element_size);
	check(status == SUCCESS, "Could not initialize @array");

	return array;

ERROR
	if (array != NULL) free(array);
	return NULL;
}


Status array_init(Array* array, uint32_t length, uint32_t initial_length, size_t element_size) {
	check(length > 0, "@length is 0");
	check(initial_length <= length, "@initial_length > @length");
	check(element_size > 0, "@element_size is 0"); 
	
	array->length = initial_length;
	array->max_length = length;
	array->element_size = element_size;
	array->data = malloc(length * element_size, "array_init");
	check_memory(array->data);
	return SUCCESS;

ERROR
	array->data = NULL;
	return FAIL;
}


void array_reset(Array* array, ElemReset reset) {
	void* elem = NULL;
	check(array_is_valid(array) == TRUE, invalid_argument("array"));

	if (reset != NULL) {
		for (uint32_t i = 0; i < array->length; ++i) {
			elem = array_get(array, i);
			reset(elem);
		}
	}
	free(array->data);
	array->element_size = 0;
	array->length = 0;
	array->max_length = 0;
	array->data = NULL;

ERROR
	return;
}


void array_destroy(Array* array, ElemReset reset) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	array_reset(array, reset);
	free(array);

ERROR
	return;
}


void* array_get(Array* array, uint32_t index) {
	void* out = NULL;
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(index < array->length, "Out of bound value for @index: %u; @array->max_length: %u", index, array->length);
	
	out = array_get_fast(array, index);
	
ERROR
	return out;
}


Status array_set(Array* array, uint32_t index, void* data) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(data != NULL, null_argument("data"));
	check(index < array->length, "@index %u >= array->length %u. Consider using @array_append", index, array->length);
	
	array_set_fast(array, index, data);
	return SUCCESS;

ERROR
	return FAIL;
}


Status array_append(Array* array, void* data) {
	Status status = FAIL;
	check(array_is_valid(array) == TRUE, invalid_argument("vector"));
	check(data != NULL, null_argument("data"));

	if (array_is_full(array)) {
		status = array_expand(array);
		check(status == SUCCESS, "Array is full and could not allocate more memory");
	}

	array_set_fast(array, array->length, data);
	(array->length)++;

	return SUCCESS;

ERROR
	return FAIL;
}


Status array_expand(Array* array) {
	uint32_t new_max_length = 0;
	void* new_data = NULL;
	
	check(array_is_valid(array) == TRUE, invalid_argument("array"));

	new_max_length = array->length + ARRAY_EXPAND_RATE;
	new_data = realloc(array->data, new_max_length * array->element_size, "array_expand");
	check_memory(new_data);

	array->data = (uint8_t*) new_data;
	array->max_length = new_max_length;

	return SUCCESS;

ERROR
	return FAIL;
}


void array_show(Array* array, ShowElem show) {
	uint32_t i = 0;
	
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(show != NULL, null_argument("show"));

	for (i = 0; i < array->length; ++i) {
		show(array_get(array, i));
	}
	printf("\n");

ERROR
	return;
}


void array_copy_data(Array* array, void* data, uint32_t start_idx, uint32_t elem_cnt) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(data != NULL, null_argument("data"));
	check(start_idx < array->length, "@start_idx >= array->length");
	check(elem_cnt > 0, "@elem_cnt == 0");
	if (start_idx + elem_cnt > array->length) {
		log_info("@elem_cnt too big, will trucate it");
		elem_cnt = array->length - start_idx;
	}

	memcpy(array_get(array, start_idx), data, elem_cnt * array->element_size);

ERROR
	return;
}


Status array_swap(Array* array, uint32_t i, uint32_t j) {
	void* data1 = NULL;
	void* data2 = NULL;
	void* aux = NULL;
	
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(i < array->length, "Out of bound value for @i: %u; @array->length: %u", i, array->length);
	check(j < array->length, "Out of bound value for @j: %u; @array->length: %u", j, array->length);

	data1 = array_get_fast(array, i);
	data2 = array_get_fast(array, j);
	aux = malloc(array->element_size, "array_swap");
	check_memory(aux);

	memcpy(aux, data1, array->element_size);
	memcpy(data1, data2, array->element_size);
	memcpy(data2, aux, array->element_size);
	
	free(aux);

	return SUCCESS;
ERROR
	return FAIL;
}


Status array_resize(Array* array, uint32_t new_max_length) {
	void* new_data = NULL;

	check(array_is_valid(array) == TRUE, invalid_argument("array"));

	new_data = realloc(array->data, new_max_length * array->element_size, "array_resize");
	check_memory(new_data);

	array->data = (uint8_t*)new_data;
	array->max_length = new_max_length;

	if (array->length > array->max_length) {
		log_warning("@array->length %u > @array->max_length %u -> array is tructated", array->length, array->max_length);
		array->length = array->max_length;
	}

	return SUCCESS;

ERROR
	return FAIL;
}


// conversion functions
ArrayFloat* array_bool_to_float(ArrayBool* array_b, BOOL destroy_array_bool) {
	ArrayFloat* array_f = (ArrayFloat*)array_create(array_b->length, array_b->length, sizeof(float));
	check_memory(array_f);
	uint32_t i = 0;
	float float_v = 0.0f;
	BOOL bool_v = FALSE;

	for (i = 0; i < array_b->length; ++i) {
		bool_v = *((BOOL*)array_get(array_b, i));
		float_v = (float)bool_v;
		array_set(array_f, i, &float_v);
	}
	if (destroy_array_bool == TRUE) 
		array_destroy(array_b, NULL);

	return array_f;

ERROR
	return NULL;
}


ArrayDouble* array_float_to_double(ArrayFloat* array_f, BOOL destroy_array_float) {
	ArrayDouble* array_d = (ArrayDouble*)array_create(array_f->length, array_f->length, sizeof(double));
	check_memory(array_d);
	uint32_t i = 0;
	double double_v = 0.0;
	float float_v = 0.0f;

	for (i = 0; i < array_f->length; ++i) {
		float_v = *((float*)array_get(array_f, i));
		double_v = (double)float_v;
		array_set(array_d, i, &double_v);
	}
	if (destroy_array_float == TRUE)
		array_destroy(array_f, NULL);

	return array_d;

ERROR
	return NULL;
}


// utility functions
ArrayFloat* array_arange_float(uint32_t length) {
	ArrayFloat* array_f = (ArrayFloat*)array_create(length, length, sizeof(float));
	check_memory(array_f);

	float i_f = 0.0f;
	uint32_t i = 0;
	for (i = 0; i < length; ++i) {
		i_f = (float)i;
		array_set(array_f, i, &i_f);
	}

ERROR
	return array_f;
}


ArrayBool* array_ones_bool(uint32_t length) {
	ArrayBool* array_b = (ArrayBool*)array_create(length, length, sizeof(BOOL));
	check_memory(array_b);

	BOOL true_v = TRUE;
	uint32_t i = 0;
	for (i = 0; i < length; ++i)
		array_set(array_b, i, &true_v);

ERROR
	return array_b;
}


ArrayUint32* array_zeros_uint23(uint32_t length) {
	ArrayUint32* array_u32 = (ArrayUint32*)array_create(length, length, sizeof(uint32_t));
	check_memory(array_b);

	uint32_t zero = 0;
	uint32_t i = 0;
	for (i = 0; i < length; ++i)
		array_set(array_u32, i, &zero);

ERROR
	return array_u32;
}


ArrayFloat* array_ones_float(uint32_t length) {
	ArrayFloat* array_f = (ArrayFloat*)array_create(length, length, sizeof(float));
	check_memory(array_f);

	float one = 1.0f;
	uint32_t i = 0;
	for (i = 0; i < length; ++i)
		array_set(array_f, i, &one);
ERROR
	return array_f;
}


Status array_of_arrays_init(Array* data, uint32_t length, size_t inner_element_size) {
	uint32_t i = 0;
	Status status = FAIL;
	Status status_data = FAIL;

	check(data != NULL, null_argument("data"));

	status_data = array_init(data, length, length, sizeof(Array));
	check(status_data == SUCCESS, "@status is %u", status_data);
	for (i = 0; i < data->length; ++i) {
		status = array_init((Array*)array_get(data, i), 10, 0, inner_element_size);
		check(status == SUCCESS, "@status is %u", status);
	}
	return SUCCESS;

ERROR
	if (status_data == SUCCESS) {
		// I know it failed at i, reset everything under i
		while (i != 0) {
			i--;
			array_reset((Array*)array_get(data, i), NULL);
		}
	}
	return FAIL;
}


Status array_of_arrays_reset(Array* data) {
	uint32_t i = 0;
	Array* inner_array = NULL;

	check(array_is_valid(data) == TRUE, invalid_argument("data"));
	for (i = 0; i < data->length; ++i) {
		inner_array = (Array*)array_get(data, i);
		check(array_is_valid(inner_array) == TRUE, "invalid @inner_array %u", i);
		array_reset(inner_array, NULL);
	}
	array_reset(data, NULL);
	return SUCCESS;

ERROR
	return FAIL;
}


/*************************************
* Statistics functions
*************************************/
GaussianDist* array_float_get_gaussian_dist(ArrayFloat* array) {
	check(array_is_valid(array) == TRUE, invalid_argument("array"));
	check(array->element_size == sizeof(float), "@array doesn't contain floats");

	float sum = 0.0f;
	float mean = 0.0f;
	float value = 0.0f;
	double std = 0.0f;
	GaussianDist* dist = NULL;
	uint32_t i = 0;
	
	// compute the mean
	for (i = 0; i < array->length; ++i) {
		sum += *((float*)array_get(array, i));
	}
	mean = sum / ((float)array->length + EPSILON);

	// compute the std
	for (i = 0; i < array->length; ++i) {
		value = *((float*)array_get(array, i));
		std += pow((double)(value - mean), 2.0);
	}
	std = sqrt(std / ((double)array->length + EPSILON));

	dist = (GaussianDist*)malloc(sizeof(GaussianDist), "array_float_get_gaussian_dist");
	check_memory(dist);
	dist->mean = mean;
	dist->std = (float)std;
	return dist;

ERROR
	return NULL;
}


/*************************************
* Dumping functions
*************************************/
void array_dump(Array* array, String* file_path, String* data_name, uint8_t type) {
	check(array_is_valid(array), invalid_argument("array"));
	check(string_is_valid(file_path), invalid_argument("file_path"));
	check(string_is_valid(data_name), invalid_argument("data_name"));
	FILE* fp = fopen(string_get_C_string(file_path), "wb");
	check(fp != NULL, "Couldn't open file %s for writting in binary mode. %s", string_get_C_string(file_path), null_argument("fp"));

	// write the data_name, do not write the \0 at the end of the string
	uint32_t length = data_name->length - 1;
	fwrite(&length, sizeof(data_name->length), 1, fp);
	fwrite(string_get_C_string(data_name), data_name->element_size, length, fp);

	// write the elements of the array
	fwrite(&(array->length), sizeof(array->length), 1, fp);
	fwrite(&(array->element_size), sizeof(array->element_size), 1, fp);
	fwrite(&type, sizeof(type), 1, fp);
	fwrite(array->data, array->element_size, array->length, fp);
	
	fclose(fp);

ERROR
	return;
}

void array_float_dump(Array* array, String* file_path, String* data_name) {
	uint8_t type = 0;
	array_dump(array, file_path, data_name, type);
}


void array_bool_dump(Array* array, String* file_path, String* data_name) {
	uint8_t type = 1;
	array_dump(array, file_path, data_name, type);
}