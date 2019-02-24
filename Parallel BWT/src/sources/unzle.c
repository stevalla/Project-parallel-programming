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
 * @file 	unzle.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing the reverse zero-length encoding.
 */

#include "../headers/unzle.h"


Text zleDecoding(const Text input)
{
	int j;
	size_t len;
	Text output;

	unsigned char *out    = (unsigned char *)
			 malloc(sizeof(unsigned char) *(MAX_CHUNK_SIZE + 9) * 2);
	unsigned char *runLen = (unsigned char *)
			 malloc(sizeof(unsigned char) * MAX_CHUNK_SIZE);

	len = 0;
	j = 0;
	runLen[j] = 255;

	for(unsigned i=0; i<input.len; i++) {

		if(input.text[i] <= 1) {			//Count encoded zeros run
			runLen[j++] = input.text[i];
			runLen[j]   = 255;
			continue;

		} else if(runLen[0] <= 1) {			//End of zeroes run

			outputZeroes(runLen, out, &len);
			runLen[0] = 255;				//Restart the zeroes run
			j = 0;
		}

		if(input.text[i] == 0xFF) {			//Special characters
			const unsigned nextByte = input.text[++i];

			if(nextByte == 0x00)
				out[len++] = 0xFE;

			else if(nextByte == 0x01)
				out[len++] = 0xFF;

		} else								//All the other chars
			out[len++] = input.text[i] - 1;

	}

	if(runLen[0] <= 1)
		outputZeroes(runLen, out, &len);

	output.len = len;
	output.text = (unsigned char *) malloc(sizeof(unsigned char) * len);

	for(unsigned i=0; i<len; i++)
		output.text[i] = out[i];

	free(input.text);
	free(runLen);
	free(out);

	return output;
}

void outputZeroes(unsigned char *runLen,
				  unsigned char *out,
				  size_t 	    *index)
{
	int dec = 0;
	unsigned i;

	for(i=0; runLen[i]<=1; i++)
		dec += (POW_2(i) * runLen[i]);

	dec += POW_2(i);

	for(i=0; i<dec-1; i++)
		out[(*index)++] = 0;
}
