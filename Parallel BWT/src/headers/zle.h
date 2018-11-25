#ifndef ZLE_H
#define ZLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned char* Ascii;


Ascii zleEncoding(Ascii, int);

int encodeZeroRun(int);


#endif
