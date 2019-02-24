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
 * @file 	unarith.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing an arithmetic coder compressor.
 *
 * @details This header defines the function needed to perform an airthmetic
 * 			decompression. The code has been taken from
 * 			<https://github.com/wonder-mice/arcd> even is it
 * 			is slightly modified to make it fit better in the application. @n
 *			The header offers the possibility to decode a given sequence of bytes
 *			in input, by using an adaptive model of probabilities, that must
 *			be the same of the one used to compress. @n
 *			The parameters for the compression such as the number of bits to
 *			represent probability intervals are fixed because the purpose of the
 *			application was not to found the best compression possible, rather it
 *			aims to have an optimal compression time with a good compression rate.
 */

#ifndef UNARITH_H
#define UNARITH_H

#include "arith.h"

/**
 * Maintains the state of the decoder.
 */
typedef struct Decoder
{
	unsigned value;		///<	Currently read code value to decompress.
	unsigned fin;		///<	Number of bits passed the end of file.
	unsigned low;		///<	Low value of the current interval.
	unsigned high;		///<	High value of the current interval.
	unsigned range;		///<	Range = High - Low.

} Decoder;

/**
 * @brief	Decode a sequence of bytes compressed by an arithmetic coder.
 *
 * @param[in]	input	The sequence of bytes in input.
 * @return	The sequence of bytes decoded.
 */
Text arithDecoding(const Text input);

/**
 * @brief	Initializes the decoder.
 *
 * @param[in,out] 	de			Pointer to the decoder to initialize.
 * @param[in,out] 	inBuf		Pointer to the buffer that keeps the current
 * 								sequence of bits to decode.
 * @param[in] 		in			Pointer to the sequence of bytes to decompress.
 * @param[in] 		inputLen	The length of the sequence to decompress.
 */
void initDecoder(Decoder    *const de,
		 	 	 ByteBuffer *const inBuf,
				 IOBuffer 	*const in,
				 size_t 	       inputLen);

/**
 * @brief	Decode a sequence of bits and retrieve the symbol associated with
 * 			the current interval read from the input sequence, that represents
 * 			the interval in the model of probabilities.
 *
 * @param[in,out] 	de			Pointer to the decoder.
 * @param[in,out] 	model		Pointer to the model of probabilities.
 * @param[in,out] 	inBuf		Pointer to the buffer that keeps the current
 * 								sequence of bits to decode.
 * @param[in] 		in			Pointer to the sequence of bytes to decompress.
 * @param[in] 		inputLen	The length of the sequence to decompress.
 */
unsigned decodeSymbol(Decoder    *const de,
					  Model      *const model,
					  ByteBuffer *const inBuf,
					  IOBuffer   *const in,
					  size_t		    inputLen);

/**
 * @brief	Selects the corresponding encoded symbol given a sequence of bits
 * 			read. Sets also the interval of the decoded symbol and updates the
 * 			model.
 *
 * @param[in]		de			Pointer to the decoder.
 * @param[out] 		interval	Pointer to the interval to be set.
 * @param[in,out] 	model		Pointer to the model of probabilities.
 */
unsigned findChar(Decoder  *const de,
				  Interval *const interval,
				  Model    *const model);

/**
 * @brief	Updates the probability model.
 *
 * @param[in]		de			Pointer to the decoder.
 * @param[in,out] 	inBuf		Pointer to the buffer that keeps the current
 * 								sequence of bits to decode.
 * @param[in] 		in			Pointer to the sequence of bytes to decompress.
 * @param[in] 		inputLen	The length of the sequence to decompress.
 */
void updateInterval(Decoder    *const de,
					ByteBuffer *const inBuf,
					IOBuffer   *const in,
					size_t 			  inputLen);

/**
 * @brief	Reads compressed bit and returns it.
 *
 * @param[in]		de			Pointer to the decoder.
 * @param[in,out] 	inBuf		Pointer to the buffer that keeps the current
 * 								sequence of bits to decode.
 * @param[in] 		in			Pointer to the sequence of bytes to decompress.
 * @param[in] 		inputLen	The length of the sequence to decompress.
 * @return
 */
unsigned inputBit(Decoder 	 *const de,
		  	  	  ByteBuffer *const inBuf,
		  	  	  IOBuffer 	 *const in,
		  	  	  size_t 		 	inputLen);

/**
 * @brief	Scales value from coder range to model frequency interval.
 *
 * Inverse of what encoder does when it maps the probability value on the
 * current range.
 *
 * @param[in] 		v			The current read encoded value.
 * @param[in] 		range		The current range.
 * @param[in]		scale		The current maximum value of the frequencies
 * 								of the model of probabilities.
 * @return	The current frequency in the model of probabilities of the read
 * 			input value.
 */
static inline
unsigned scaleFrequencies(const unsigned v,
						  const unsigned range,
						  const unsigned scale)
{
	return (((unsigned)v + 1) * scale - 1) / range;
}
#endif
