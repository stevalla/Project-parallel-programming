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
 * @file 	arith.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing an arithmetic coder compressor.
 *
 * @details This header defines the function needed to perform an airthmetic
 * 			compression, that is the last phase of the BWT compression. The code
 * 			has been taken from <https://github.com/wonder-mice/arcd> even is it
 * 			is slightly modified to make it fit better in the application. @n
 *			The header offers the possibility to encode a given sequence of bytes
 *			in input, by using an adaptive model of probabilities. It also gives a
 *			set of functions to print out the encode sequence byte by byte. @n
 *			The parameters for the compression such as the number of bits to
 *			represent probability intervals are fixed because the purpose of the
 *			application was not to found the best compression possible, rather it
 *			aims to have an optimal compression time with a good compression rate.
 */

#ifndef ARITH_H
#define ARITH_H

#include "util.h"

/**
 * Defines how to represent frequency values. This value is exact.
 */
enum FREQUENCY
{
	FREQUENCY_BITS = 15,						///< Number of bits.
	MAX_FREQUENCY  = POW_2(FREQUENCY_BITS) - 1	///< Max value of bits.
};

/**
 * Number of bits used to represent arithmetic coder
 * intervals. This value is not exact, in fact the maximum
 * value uses one bit more, but it's the only one.
 */
enum BITS
{
	BITS_RANGE  = 17,							///< Number of bits.
	MAX_RANGE	= POW_2(BITS_RANGE) 			///< Maximum possible range.
};


/**
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
	unsigned lowCount;	///<	Low value of the interval.
	unsigned highCount;	///<	High value of the interval.
	unsigned scale;		///<	Scale.

} Interval;

/**
 * Maintains the state of the encoder, defined by the pair (low, high) that
 * represents the border of the current interval of the compression. At the end
 * of the compression any value between this interval will be ok to use for the
 * arithmetic compression.
 */
typedef struct Encoder
{
	unsigned low;		///<	Low value of the current interval.
	unsigned high;		///<	High value of the current interval.
	unsigned range;		///<	Range = High - Low.
	unsigned underflow;	///<	Keeps track of the underflow bits to be printed out.

} Encoder;

/**
 * Stores the model of probabilities. It maintains 256 cumulative probabilities one
 * for each possible character in input between 0 and 255.
 */
typedef struct Model
{
	unsigned size;			///<	Number of probabilities stored in.
	unsigned short *freq;	/**<	Array of the cumulative probabilities of each
									value of the mdoel. */
} Model;

/**
 * Store a byte bit by bit before it is printed out in the output buffer.
 */
typedef struct ByteBuffer
{
	unsigned char buf;	///<	Byte buffer.
	unsigned bufBits;	///<	Keeps track of the number of bits written.

} ByteBuffer;

/**
 * Buffer that stores the intermediate results of the compression. Each time
 * the byte buffer is full, it is emptied into the IOBuffer.
 */
typedef struct IOBuffer
{
	unsigned char *text;	///<	Maintains the already written compressed text.
	int index;				///<	Index of the current char to be written.

} IOBuffer;

/**
 * @brief	Main function of the arithmetic encoder.
 *
 * @param[in] 	  input		A sequence of bytes.
 * @return	The compressed text.
 */
Text encodingRoutine(const Text input);

/**
 * @brief	Initialize the encoder pointed by the pointer in input
 *
 * @param[in,out] encoder
 */
void initEncoder(Encoder *const encoder);

/**
 * @brief	Encode the symbol by scaling the range basing on the interval of the
 * 			symbol.
 *
 * @param[in,out] low		Current low value of the interval.
 * @param[in,out] high		Current high value of the interval.
 * @param[in] 	  range		Current range.
 * @param[in] 	  interval	Interval relative to the current symbol in input.
 */
void encodeSymbol(unsigned *const 	 low,
		  	  	  unsigned *const 	 high,
				  const 	unsigned range,
				  const 	Interval interval);

/**
 * @brief	Find the interval probability of the input symbol.
 *
 * @param[out]	  interval	The interval that needs to be found.
 * @param[in] 	  model		The model of probabilities.
 * @param[in] 	  ch		The character which interval has to be found.
 */
void findInterval(Interval *const 	 interval,
				  Model    *const 	 model,
				  const     unsigned ch);

/**
 * @brief	It checks if there are still bits to write in output, and writes it.
 *
 * @param[in] 	  encoder	The encoder.
 * @param[in,out] byteBuf	The byte buffer.
 * @param[in,out] buf		The buffer for output.
 */
void finishEncoding(Encoder    *const encoder,
					ByteBuffer *const byteBuf,
					IOBuffer   *const buf);

/**
 * @brief	Initialization of the mdoel. Assigns a uniform probability to all
 * 		    the values.
 *
 * @param[in,out] model		The model to initialize.
 * @param[in] 	  size		The size of the model, i.e. the number of probabilities.
 */
void initModel(Model *const model, const unsigned size);

/**
 * @brief	Updates the model basing on the new occurrence of the character in
 * 			input.
 *
 * @param[in,out] model		The model to be updated.
 * @param[in] 	  ch		The new occurrence for which the model has to be updated.
 */
void updateModel(Model *const model, const unsigned ch);

/**
 * @brief	Byte buffer initialization.
 *
 * @param[in,out] byteBuf
 */
void initByteBuffer(ByteBuffer *byteBuf);

/**
 * @brief	Check if there are bits that can be output and it output those bit in
 * 			the byte buffer.
 *
 * @param[in,out] encoder	The encoder.
 * @param[in,out] byteBuf	The byte buffer.
 * @param[in,out] buf		The buffer for output.
 */
void checkForOutputBit(Encoder 	  *const encoder,
					   ByteBuffer *const byteBuf,
					   IOBuffer   *const buf);

/**
 * @brief	Output the bit in input in the byte Buffer and checks and outputs
 * 			underflow bits.
 *
 * @param[in,out] encoder	The encoder.
 * @param[in] 	  bit		The bit to be printed out.
 * @param[in,out] byteBuf	The byte buffer to store the current byte to be printed.
 * @param[in,out] buffer	The buffer to store the current overall output.
 */
void outputBits(Encoder    *const 	  encoder,
				const    	unsigned  bit,
				ByteBuffer *const	  byteBuf,
				IOBuffer   *const	  buffer);

/**
 * @brief	Writes the bit in the byte buffer and then, if the byte buffer is full,
 * 			it empties it into the buffer.
 *
 * @param[in,out] byteBuf	The byte buffer.
 * @param[in] 	  bit		The bit to be printed.
 * @param[in,out] buffer	The buffer.
 */
void outputBit(ByteBuffer *const 	byteBuf,
			   const 	   unsigned bit,
			   IOBuffer   *const	buffer);

#endif
