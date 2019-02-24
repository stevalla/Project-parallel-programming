/******************************************************************************
 * Copyright (C) 2018 by Stefano Valladares                                   *
 *                                                                            *
 * This file is part of ParallelBWTzip.                                       *
 *                                                                            *
 *   ParallelBWTzip is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 3 of the       *
 *   License, or (at your option) any later version.                          *
 *                                                                            *
 *   ParallelBWTzip is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public         *
 *   License along with ParallelBWTzip. 									  *
 *   If not, see <http://www.gnu.org/licenses/>.     						  *
 ******************************************************************************/

/**
 * @file 	unarith.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing an arithmetic coder compressor.
 */

#include "../headers/unarith.h"

/// Number of bits of the byte buffer.
#define BUF_BITS	(8 * sizeof(unsigned char))

//Useful values
static const unsigned         RANGE_BITS = BITS_RANGE;			///< Bits range.
static const unsigned          RANGE_MIN = 0;					///< Range 0/1.
static const unsigned          RANGE_MAX = MAX_RANGE;			///< Range 1/1.
static const unsigned         RANGE_HALF = MAX_RANGE / 2;		///< Range 1/2.
static const unsigned   RANGE_ONE_FOURTH = MAX_RANGE / 4;		///< Range 1/4.
static const unsigned RANGE_THREE_FOURTH = (MAX_RANGE * 3) / 4; ///< Range 3/4.

/**
 * Bit used to extend encoded bit stream past its end. Currently can be set to
 * either 0 or 1.
 */
static const unsigned CONTINUATION_BIT = 0;

static const unsigned EOS = 1 << (8 * sizeof(unsigned char));	///< EOS = 256.

/**
 * This functions decompresses an input sequence of bytes using an
 * arithmetic decoder and an adaptive model of probabilities. @n
 * It reads the sequence byte by byte and for each byte:
 * 	-	Dcode the symbol using the actual probabilities..
 * 	-	Update probabilities.
 *
 * The probability model must be the same used for compressing the sequence.
 */
Text arithDecoding(const Text input)
{
	Decoder    *de;
	Model 	   *model;
	ByteBuffer *inBuf;
//	Interval 	currentInt;
	IOBuffer   *in, *o;
	Text 	    result;
	unsigned    ch;
	size_t 	    inputLen;

	de    = (Decoder *)    malloc(sizeof(Decoder));
	model = (Model *)      malloc(sizeof(Model));
	inBuf = (ByteBuffer *) malloc(sizeof(ByteBuffer));

	//Init in buffer
	in        = (IOBuffer *) malloc(sizeof(IOBuffer));
	in->text  = input.text;
	in->index = 0;
	inputLen  = input.len;

	//Init out buffer
	o        = (IOBuffer *) malloc(sizeof(IOBuffer));
	o->index = 0;
	o->text  = (unsigned char *)
			    malloc(sizeof(unsigned char) * MAX_CHUNK_SIZE * 2);

	initModel(model, EOS + 1);
	initByteBuffer(inBuf);
	initDecoder(de, inBuf, in, inputLen);

	//Decode routine
	while((ch = decodeSymbol(de, model, inBuf, in, inputLen)) != EOS)
		o->text[o->index++] = (unsigned char) ch;

	//Write result
	result.len  = o->index;
	result.text = (unsigned char *)
			malloc(sizeof(unsigned char) * o->index);

	for(unsigned i=0; i<o->index; i++)
		result.text[i] = o->text[i];

	free(o->text);
	free(o);
	free(in);
	free(de);
	free(model->freq);
	free(model);
	free(inBuf);
	free(input.text);

	return result;
}

/**
 * The decoder is initialize so that the intervals is the total interval [0,1]
 * and the read values are zero. The first 17 bits are read from the input.
 */
void initDecoder(Decoder    *const de,
				 ByteBuffer *const inBuf,
				 IOBuffer 	*const in,
				 size_t 	       inputLen)
{
	de->value     = 0;
	de->fin       = 0;
	de->low       = RANGE_MIN;
	de->high      = RANGE_MAX;
	de->range     = RANGE_MAX - RANGE_MIN + 1;

	for(unsigned i = RANGE_BITS; i > 0; i--)
		de->value = de->value << 1 | inputBit(de, inBuf, in, inputLen);
}

/**
 * The operation is done by following three steps:
 */
unsigned decodeSymbol(Decoder 	 *const de,
					  Model 	 *const model,
					  ByteBuffer *const inBuf,
					  IOBuffer 	 *const in,
					  size_t 		    inputLen)
{
	Interval interval;

	/** -	Find char associated with the current state of the decoder
	 * 		and set the current interval of the symbol decoded.
	 */
	const unsigned ch = findChar(de, &interval, model);

	/// -	Update the current interval of the decoder with the read one.
	encodeSymbol(&de->low, &de->high, de->range, interval);

	/// -	Update the probability model and check for other bits to read.
	updateInterval(de, inBuf, in, inputLen);

	return ch;
}


unsigned findChar(Decoder  *const de,
				  Interval *const interval,
				  Model    *const model)
{
	const unsigned v    = de->value - de->low;
	const unsigned last = model->size - 1;
	const unsigned freq = scaleFrequencies(v, de->range, model->freq[last]);

	for(unsigned i=0; i<last; i++) {

		if(model->freq[i] <= freq && model->freq[i + 1] > freq) {

			interval->lowCount  = model->freq[i];
			interval->highCount = model->freq[i + 1];
			interval->scale     = model->freq[last];

			updateModel(model, i);
			return i;
		}
	}
	return -1;
}

void updateInterval(Decoder    *const de,
					ByteBuffer *const inBuf,
					IOBuffer   *const in,
					size_t 			  inputLen)
{
	for(;;) {

		if(de->high <= RANGE_HALF) {
			de->high = 2 * de->high;
			de->low  = 2 * de->low;

		} else if(de->low >= RANGE_HALF) {

			de->high   = (de->high - RANGE_HALF) * 2;
			de->low    = (de->low  - RANGE_HALF) * 2;
			de->value -= RANGE_HALF;

		} else if(de->low  >= RANGE_ONE_FOURTH &&
				  de->high <= RANGE_THREE_FOURTH) {

			de->high   = (de->high - RANGE_ONE_FOURTH) * 2;
			de->low    = (de->low  - RANGE_ONE_FOURTH) * 2;
			de->value -= RANGE_ONE_FOURTH;

		} else
			break;

		de->value = de->value << 1 | inputBit(de, inBuf, in, inputLen);
	}

	de->range = de->high - de->low + 1;
}

/**
 * This function reads one compressed bit from the input buffer and returns it.
 * When the end of file is reached, byt the fin variable of the decoder is set
 * to 1, this functions returns the continuation bit to let the decoder finish
 * the decompression.
 */
unsigned inputBit(Decoder 	 *const de,
				  ByteBuffer *const inBuf,
				  IOBuffer 	 *const in,
				  size_t 		 	inputLen)
{
	if(de->fin)
		return CONTINUATION_BIT;

	if(inBuf->bufBits == 0) {

		if(in->index < inputLen) {
			inBuf->buf = in->text[in->index];
			inBuf->bufBits = 8;
			in->index++;
		} else
			inBuf->bufBits = 0;

		if(inBuf->bufBits == 0) {
			de->fin = !de->fin;
			return CONTINUATION_BIT;
		}
	}

	const unsigned bit = inBuf->buf >> (BUF_BITS - 1);
	inBuf->buf <<= 1;
	--inBuf->bufBits;

	return bit;
}
