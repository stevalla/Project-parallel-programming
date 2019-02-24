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
 * @file 	unbwt.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing the reverse Burrows-Wheeler
 * 			transformation.
 */

#include "../headers/unbwt.h"

/**
 * The implementation follows the one based on the LF mapping, a function from
 * the last column of the BWT to the first column. It is based on the
 * lemma that the i-th occurrence of char c in the last column of
 * the BWT matrix is the same exact character as the i-th occurrence
 * c in the first column. @n
 * The algorithm starts reading the index of the first and of the
 * sentinel characters. Then it calculates the frequencies of each
 * character in the input sequence. After that it computes two arrays:
 * 		-	One array that contains for each possible value i
 * 			the number of values that are alphabetically smaller
 * 			than i (mappings).
 * 		-	One array that contains the LF mapping (links) calculated
 * 			from mappings and frequencies.
 *
 * Finally it outputs the original string from the LF mapping.
 */
Text unbwt(Text input)
{
	Text output;
	unsigned first, sentinel;
	unsigned *links = (unsigned *)malloc(sizeof(unsigned)*input.len);
	unsigned freqs[257], mapping[257];

	first  = readUnsigned(input.text, 0);
	sentinel = readUnsigned(input.text, 4);

	output.text = (unsigned char *)malloc(sizeof(unsigned char)*input.len);
	output.len = input.len - 9;

	input.text += 8;
	input.len  -= 8;

	//Init freqs vector
	for(unsigned i=0; i<257; i++)
		freqs[i] = 0;

	//Compute K
	for(unsigned i=0; i<input.len; i++) {
		if(i == sentinel)
			freqs[256]++;
		else
			freqs[input.text[i]]++;
	}

	//Compute M
	mapping[0] = 0;
	for(unsigned i=1; i<257; i++)
		mapping[i] = mapping[i - 1] + freqs[i - 1];

	//Compute L
	for(unsigned i=0; i<input.len; i++) {
		if(i == sentinel)
			links[i] = mapping[256]++;
		else
			links[i] = mapping[input.text[i]]++;
	}

	//Compute output
	first = links[first];
	for(int i=input.len-2; i>=0; i--) {
		first = links[first];
		output.text[i] = input.text[first];
	}

	free(input.text-8);
	free(links);

	return output;
}
