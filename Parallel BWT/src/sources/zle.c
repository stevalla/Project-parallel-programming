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
 *
 * @file 	zle.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing a zero length encoder.
 */

#include "../headers/zle.h"
#include <math.h>

#define LOG2E 1.44269504089		///< Log base 2 of e.

/**
 * The main method of the file encodes a sequence of length N by
 * compressing sequences of consecutive zeroes. Each one of that will
 * be encoded in a binary number that represents the length of the
 * zero-run.
 * Each byte in input is encoded in an online fashion, written it down
 * as soon as it is read, while each sequence of zeroes is encoded as soon
 * as it ends by calling the function ::encodeZeroRun.
 */
Text zleEncoding(const Text input)
{
	size_t runLen;
	Text res;

	runLen 	 = 0;
	res.id 	 = input.id;
	res.len  = 0;
	res.text = (unsigned char *)malloc(sizeof(unsigned char) * input.len*2);

	for(unsigned j=0; j<input.len; j++) {

		if(input.text[j] == 0) {			//Zeros run
			runLen++;
			continue;

		} else if(runLen != 0) {			//End of zeros run
			encodeZeroRun(runLen + 1, &res);
			runLen = 0;

		}

		//No run
		if(input.text[j] == 0xFE) {			//ch = 254
			res.text[res.len++] = 0xFF;
			res.text[res.len++] = 0x00;

		} else if(input.text[j] == 0xFF) {	//ch = 255
			res.text[res.len++]  = 0xFF;
			res.text[res.len++]  = 0x01;

		} else
			res.text[res.len++] = input.text[j] + 1;
	}

	if(runLen != 0)
		encodeZeroRun(runLen + 1, &res);

	free(input.text);

	return res;
}

/**
 * This function encodes a sequence of zeroes of length zeroRun
 * in the big-endian representation of (zeroRun + 1), eliding the
 * MSB.
 */
void encodeZeroRun(size_t runLen, Text *res)
{
	int k;

	size_t binLen = (log(runLen) * LOG2E) + 1;

	for(k=0; k<binLen-1; k++) {
		(*res).text[((*res).len)++] = runLen%2;
		runLen /= 2;
	}
}

