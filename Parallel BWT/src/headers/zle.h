#ifndef ZLE_H
#define ZLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef struct ZeroRun
{
	int len;
	short *encoded;

} ZeroRun;

ZeroRun *zleEncoding(short *, int);

void countZeroRun(double, short *, int *);

short *decToBin(int, int);


#endif
