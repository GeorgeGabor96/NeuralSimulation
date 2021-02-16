#ifndef __PBPLOTS_SUPPORT_LIB__
#define __PBPLOTS_SUPPORT_LIB__

#include <stdlib.h>
#include <stdio.h>

unsigned char *DoubleArrayToByteArray(double *data, size_t length);
void WriteToFile(double *data, size_t dataLength, char *filename);
double *ByteArrayToDoubleArray(unsigned char *data, size_t length);

#endif // __PBPLOTS_SUPPORT_LIB__