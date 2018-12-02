#ifndef UNARITH_H
#define UNARITH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "arith.h"
#include "util.h"

typedef struct Decoder
{
	unsigned valueBits;
	unsigned value;
	unsigned fin;
	unsigned low;
	unsigned high;
	unsigned range;

} Decoder;


/*
 * Decoder functions
 */
void initDecoder(Decoder *const);

Text *decodingRoutine(Text *const);

unsigned decodeSymbol(Decoder *const, Model *const, Interval *const,
					  IOBuffer *const, IOHelper *const);

unsigned inputBit(Decoder *const, IOBuffer *const,
				  IOHelper *const);

unsigned findChar(Decoder *const, Interval *const, Model *const);

void updateInterval(Decoder *const, IOBuffer *const,
		IOHelper *const);

static inline
unsigned scaleFrequencies(const unsigned v,
						  const unsigned range,
						  const unsigned scale)
{
	return (((unsigned)v + 1) * scale - 1) / range;
}
#endif
