#ifndef ZLE_H
#define ZLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "util.h"


Text zleEncoding(const Text);

void countZeroRun(const double, unsigned char *const ,
				  size_t *const);

unsigned *decToBin(size_t, const size_t);


#endif
