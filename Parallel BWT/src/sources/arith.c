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
 * @file 	arith.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing an arithmetic coder compressor.
 */

#include "../headers/arith.h"

/// Number of bits of the byte buffer.
#define BUF_BITS	(8 * sizeof(unsigned char))

static const unsigned          RANGE_MIN = 0;					///< Range 0/1.
static const unsigned          RANGE_MAX = MAX_RANGE;			///< Range 1/1.
static const unsigned         RANGE_HALF = MAX_RANGE / 2;		///< Range 1/2.
static const unsigned   RANGE_ONE_FOURTH = MAX_RANGE / 4;		///< Range 1/4.
static const unsigned RANGE_THREE_FOURTH = (MAX_RANGE * 3) / 4; ///< Range 3/4.

static const unsigned EOS = 1 << (8 * sizeof(unsigned char));	///< EOS=256.

/**
 * This functions compresses an input sequence of bytes using an
 * arithmetic encoder and an adaptive model of probabilities. @n
 * It reads the sequence byte by byte and for each byte:
 * 	-	Encode the symbol using the actual probabilities.
 * 	-	Check if the ByteBuffer is full to print out a char.
 * 	-	Update probabilities.
 */
Text encodingRoutine(const Text input)
{
	Encoder    *en;
	Interval    currentInt;
	ByteBuffer *byteBuf;
	Model	   *model;
	IOBuffer   *buffer;

	en    	 	  = (Encoder *)  	 malloc(sizeof(Encoder));
	byteBuf   	  = (ByteBuffer *) 	 malloc(sizeof(ByteBuffer));
	model 	 	  = (Model *)    	 malloc(sizeof(Model));

	//Init buffer
	buffer        = (IOBuffer *)     malloc(sizeof(IOBuffer));
	buffer->text  = (unsigned char *)malloc(sizeof(unsigned char)*input.len*2);
	buffer->index = 0;

	initEncoder(en);
	initModel(model, EOS + 1);
	initByteBuffer(byteBuf);

	for(unsigned i=0; i<input.len; i++) {

		findInterval(&currentInt, model, input.text[i]);
		encodeSymbol(&en->low, &en->high, en->range, currentInt);
		checkForOutputBit(en, byteBuf, buffer);
	}

	//Encode EOS
	findInterval(&currentInt, model, EOS);
	encodeSymbol(&en->low, &en->high, en->range, currentInt);
	checkForOutputBit(en, byteBuf, buffer);

	finishEncoding(en, byteBuf, buffer);

	//Write result
	Text output;
	output.text = (unsigned char *)malloc(sizeof(unsigned char)*buffer->index);
	output.len  = buffer->index;
	output.id   = input.id;

	for(unsigned i=0; i<buffer->index; i++)
		output.text[i] = buffer->text[i];

	free(buffer->text);
	free(buffer);
	free(en);
	free(byteBuf);
	free(model->freq);
	free(model);
	free(input.text);

	return output;
}

/**
 * The encoder is initialize so that the intervals is the total interval [0,1].
 */
void initEncoder(Encoder *const encoder)
{
	encoder->low 	   = RANGE_MIN;
	encoder->high 	   = RANGE_MAX;
	encoder->range 	   = RANGE_MAX - RANGE_MIN + 1;
	encoder->underflow = 0;
}


void encodeSymbol(unsigned *const 	 low,
				  unsigned *const 	 high,
				  const 	unsigned range,
				  const 	Interval interval)
{
	*high = *low + range * interval.highCount / interval.scale;
	*low  = *low + range * interval.lowCount  / interval.scale;
}


void findInterval(Interval *const    interval,
				  Model    *const    model,
				  const 	unsigned ch)
{
	interval->lowCount  = model->freq[ch];
	interval->highCount = model->freq[ch + 1];
	interval->scale     = model->freq[model->size - 1];

	updateModel(model, ch);
}


void finishEncoding(Encoder    *const en,
					ByteBuffer *const byteBuf,
					IOBuffer   *const buf)
{
	if(RANGE_MIN == en->low && en->underflow == 0 && RANGE_MAX != en->high)
		outputBits(en, 0, byteBuf, buf);

	else if(RANGE_MAX == en->high && en->underflow == 0 && en->low != 0)
		outputBits(en, 1, byteBuf, buf);

	else {
		if(en->low != RANGE_MIN && en->high != RANGE_MAX)
			++en->underflow;

		outputBits(en, RANGE_ONE_FOURTH <= en->low, byteBuf, buf);
	}

	if (byteBuf->bufBits != 0) {
		(void) byteBuf->bufBits;
		buf->text[buf->index++] = byteBuf->buf;
	}
}


void initModel(Model *const model, const unsigned size)
{
	model->size = size + 1;
	model->freq = (unsigned short *)
				   malloc(sizeof(model->freq[0]) * model->size);

	for(unsigned i=0; i<model->size; i++)
		model->freq[i] = i;
}

void updateModel(Model *const model, const unsigned ch)
{
	for(unsigned i=ch + 1; i<model->size; i++)
		++model->freq[i];

	if(MAX_FREQUENCY > model->freq[model->size - 1])
		return;

	unsigned base = 0;
	for(unsigned i=1; i<model->size; i++) {

		unsigned d = model->freq[i] - base;

		if(d > 1)
			d /= 2;
		base = model->freq[i];
		model->freq[i] = model->freq[i - 1] + d;
	}
}


void initByteBuffer(ByteBuffer *const byteBuf)
{
	byteBuf->buf 	 = 0;
	byteBuf->bufBits = 0;
}


void checkForOutputBit(Encoder    *const encoder,
					   ByteBuffer *const byteBuf,
					   IOBuffer   *const buf)
{
	for(;;) {

		if(encoder->high <= RANGE_HALF) {

			outputBits(encoder, 0, byteBuf, buf);
			encoder->low  = 2 * encoder->low;
			encoder->high = 2 * encoder->high;

		} else if(encoder->low >= RANGE_HALF) {

			outputBits(encoder, 1, byteBuf, buf);
			encoder->low  = 2 * (encoder->low  - RANGE_HALF);
			encoder->high = 2 * (encoder->high - RANGE_HALF);

		} else if(encoder->low  >= RANGE_ONE_FOURTH   &&
			      encoder->high <= RANGE_THREE_FOURTH) {

			encoder->underflow++;
			encoder->low  = 2 * (encoder->low  - RANGE_ONE_FOURTH);
			encoder->high = 2 * (encoder->high - RANGE_ONE_FOURTH);

		} else
			break;
	}
	encoder->range = encoder->high - encoder->low + 1;
}

void outputBits(Encoder    *const    en,
		 	 	const 	    unsigned bit,
				ByteBuffer *const    byteBuf,
				IOBuffer   *const    buffer)
{
	outputBit(byteBuf, bit, buffer);

	for(const unsigned inv = !bit; en->underflow > 0; --en->underflow)
		outputBit(byteBuf, inv, buffer);
}


void outputBit(ByteBuffer *const 	byteBuf,
			   const 	   unsigned bit,
			   IOBuffer   *const 	buffer)
{
	byteBuf->buf |= bit << (BUF_BITS - ++byteBuf->bufBits);
	if(BUF_BITS == byteBuf->bufBits) {

		(void) BUF_BITS;
		buffer->text[buffer->index++] = byteBuf->buf;
		byteBuf->buf 	 = 0;
		byteBuf->bufBits = 0;
	}

}








