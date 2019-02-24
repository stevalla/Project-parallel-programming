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
 * @file 	bwtUnzip.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing the sequential version of the
 * 			Burrows-Wheeler decompression.
 */

#include "../headers/bwtUnzip.h"

/**
 * This function applies in reverse the four step of the BWT
 * decompression in sequence to a single sequence of bytes. @n
 * The main phase are:
 */
Text bwtUnzip(const Text input)
{
	///-# Arithmetic decoding.
	Text decompressed = arithDecoding(input);

	///-# Zero-length deconding.
	Text zleDecoded = zleDecoding(decompressed);

	///-# MTF reverse transformation.
	Text mtfReverse = unmtf(zleDecoded);

	///-# BWT reverse transformation.
	Text bwtReverse = unbwt(mtfReverse);

	return bwtReverse;
}

/**
 * The compressed file is read. First the encoded length and the id
 * are extract from the file. Then if the id is 1 the compressed chunk
 * of the length read before is decompressed and written out in the
 * output file. The code is consistency with the compression version.
 */
void decompress(FILE *input, FILE *output)
{
	while(1) {

		Text inUnzip, decompressed, length;
		unsigned char *id;

		length = readFile(input, 4);

		if(length.len == 0) {
			free(length.text);
			break;
		}

		id = readFile(input, 1).text;
		inUnzip.len = readUnsigned(length.text, 0);
		inUnzip.text = readFile(input, inUnzip.len).text;

		if(id[0] == 1)
			decompressed = bwtUnzip(inUnzip);
		else
			decompressed = inUnzip;

		writeFile(output, decompressed.text, decompressed.len);

		free(decompressed.text);
		free(length.text);
		free(id);
	}
}
