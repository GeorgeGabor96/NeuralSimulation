#include "Containers.h"


void show_float(void* data) {
	float a = *((float*)data);
	printf("%f ", a);
}


void show_bool(void* data) {
	float a = *((BOOL*)data);
	if (a > 0) {
		printf("1 ");
	} 
	else {
		printf("0 ");
	}
}


void show_uint8_t(void* data) {
	uint8_t a = *((uint8_t*)data);
	printf("%u ", a);
}


void show_uint16_t(void* data) {
	uint16_t a = *((uint16_t*)data);
	printf("%u ", a);
}


void show_uint32_t(void* data) {
	uint32_t a = *((uint32_t*)data);
	printf("%u ", a);
}


void show_status(void* data) {
	Status a = *((Status*)data);
	if (a == TRUE) {
		printf("1 ");
	}
	else {
		printf("0 ");
	}
}