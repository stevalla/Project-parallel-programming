#ifndef ARITH_H
#define ARITH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POW_2(N)			(1 << (N))
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

typedef struct Decoder
{
	unsigned valueBits;
	unsigned value;
	unsigned fin;
	unsigned low;
	unsigned high;
	unsigned range;

} Decoder;

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

/*
 * encoding functions
 */
void initEncoder(Encoder *const);

void encodingRoutine(FILE *const);

void encodeSymbol(unsigned *const, unsigned *const,
				  const unsigned, const Interval);

void finishEncoding(Encoder *const, IOBuffer *const, FILE *const);

/*
 * Model functions
 */
void initModel(Model *const, const unsigned);

void updateModel(Model *const, const unsigned);

void findInterval(Interval *const, Model *const, const unsigned);

unsigned findChar(Decoder *const, Interval *const, Model *const);

/*
 * Output functions
 */

void initIOBuffer(IOBuffer *);

void checkForOutputBit(Encoder *const, IOBuffer *const, FILE *const);

void outputBits(Encoder *const, const unsigned, IOBuffer *const, FILE *const);

void outputBit(IOBuffer *const, const unsigned, FILE *);

void updateInterval(Decoder *const, IOBuffer *const, FILE *const);

/*
 * Decoder functions
 */
void initDecoder(Decoder *const);

void decodingRoutine(FILE *const);

unsigned decodeSymbol(Decoder *const, Model *const, Interval *const,
		IOBuffer *const, FILE *const);

unsigned inputBit(Decoder *const, IOBuffer *const, FILE *const);

static inline
unsigned scaleFrequencies(const unsigned v,
						  const unsigned range,
						  const unsigned scale)
{
//	printf("v=%d, scale=%d, range=%d\n", v, scale, range);
	return (((unsigned)v + 1) * scale - 1) / range;
}

#endif
