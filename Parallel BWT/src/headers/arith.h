#ifndef ARITH_H
#define ARITH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"

#define BUF_BITS			(8 * sizeof(unsigned char))

/*
 * Number of bits to represent arithmetic values.
 * This value is exact and the maximum is
 * 2^FREQUENCY_BITS - 1
 */
enum {FREQUENCY_BITS = 15 };
enum {MAX_FREQUENCY = POW_2(FREQUENCY_BITS) - 1};
/*
 * Number of bits used to represent arithmetic coder
 * intervals. This value is not exact, in fact the maximum
 * value uses one bit more, but it's the only one.
 */
enum {BITS_RANGE = 17};
enum {MAX_RANGE	= POW_2(BITS_RANGE)};


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
	unsigned lowCount;
	unsigned highCount;
	unsigned scale;

} Interval;

typedef struct Encoder
{
	unsigned low;
	unsigned high;
	unsigned range;
	unsigned underflow;

} Encoder;

typedef struct IOBuffer
{
	unsigned char buf;
	unsigned bufBits;

} IOBuffer;

typedef struct Model
{
	unsigned size;
	unsigned short *freq;

} Model;

typedef struct IOHelper
{
	unsigned char *text;
	int index;

} IOHelper;

/*
 * encoding functions
 */
void initEncoder(Encoder *const);

Text *encodingRoutine(Text *const);

void encodeSymbol(unsigned *const, unsigned *const,
				  const unsigned, const Interval);

void finishEncoding(Encoder *const, IOBuffer *const, IOHelper *const);

/*
 * Model functions
 */
void initModel(Model *const, const unsigned);

void updateModel(Model *const, const unsigned);

void findInterval(Interval *const, Model *const, const unsigned);

/*
 * Output functions
 */

void initIOBuffer(IOBuffer *);

void checkForOutputBit(Encoder *const, IOBuffer *const, IOHelper *const);

void outputBits(Encoder *const, const unsigned,
				IOBuffer *const, IOHelper *const);

void outputBit(IOBuffer *const, const unsigned, IOHelper *);



#endif
