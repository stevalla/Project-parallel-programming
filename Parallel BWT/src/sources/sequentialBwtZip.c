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
 * @file 	sequentialBwtZip.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing the sequential version of the
 * 			Burrows-Wheeler compression.
 */

#include "../headers/sequentialBwtZip.h"

/**
 * This function applies the four step of the BWT compression
 * in sequence to a single sequence of bytes. @n
 * The main phase are:
 */
Text bwtZip(const Text input)
{
	///-# BWT transformation.
	Text bwtOutput = bwtTransformation(input);

	///-# MTF transformation.
	Text mtfOutput = mtf(bwtOutput);

	///-# Zero-length encoding.
	Text zleOutput = zleEncoding(mtfOutput);

	///-# Arithmetic compression.
	Text compressed = encodingRoutine(zleOutput);

	return compressed;
}

/**
 * The file is read chunk by chunk until is finished. Each chunk is
 * compressed when is read and then written in output folloring
 * its length (encoded in 4 bytes) and the id of the chunk.
 * If one chunk is smaller than ::MIN_CHUNK_SIZE the chunk is not
 * compressed for efficiency reasons. @n
 * The id is 1 if the chunk is compressed, 0 otherwise.
 */
void compressSequential(FILE *input, FILE *output, long chunkSize)
{
	while(1) {

		Text compressed, inZip;
		unsigned char length[4];
		unsigned char id[1];

		inZip = readFile(input, chunkSize);

		if(inZip.len == 0) {
			free(inZip.text);
			break;
		}

		if(inZip.len < MIN_CHUNK_SIZE) {
			compressed = inZip;
			id[0] = 0;

		} else {
			compressed = bwtZip(inZip);
			id[0] = 1;
		}

		encodeUnsigned(compressed.len, length, 0);

		writeFile(output, length, 4);
		writeFile(output, id, 1);
		writeFile(output, compressed.text, compressed.len);

		free(compressed.text);
	}

}
