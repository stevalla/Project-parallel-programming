#ifndef ARITH_H
#define ARITH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * The counts uniquely define where on the 0 to 1 range
 * the symbol lies, and the scale value tells what the
 * total span of the 0 to 1 range scale is.
 *
 * If 1000 characters are read so far, scale will be 1000.
 *
 * To calculate the current interval we calculate:
 * 		(lowCount/scale, highCount/scale)
 */
typedef struct Interval
{
	unsigned int lowCount;
	unsigned int highCount;
	unsigned int scale;

} Interval;

typedef struct Encoder
{
	unsigned int low;
	unsigned int high;
	unsigned long underflow;

} Encoder;


short *encodingRoutine(short *, size_t);

void initEncoder(Encoder *);

void findInterval(Interval *, size_t, short *, short);

void initModel(short *);

void updateModel(short *);

void encodeSymbol(Encoder *, Interval);

void checkForOutput(Encoder *, short *, int *);

void outputBit(unsigned short, short *, unsigned long *, int *);

void finishEncoding(Encoder *, short *, int *);


#endif
