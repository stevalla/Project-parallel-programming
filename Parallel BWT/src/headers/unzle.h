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
 * @file 	unzle.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing the reverse zero-length encoding.
 *
 * @details This interface defines the functions needed to reverse the
 * 			zero-length encoding routine of the BWT compression. In
 * 			particular, the function ::zleDecoding takes as input a sequence
 * 			of bytes and undos the ZLE. Another function ::outputZeroes
 * 			is used to reverse the binary number that represents the
 * 			number of zeroes of the original string. @n
 * 			For the format of the input string refers to the zero-length
 * 			encoding interfaces @ref zle.h
 */

#ifndef UNZLE_H
#define UNZLE_H

#include "util.h"

/**
 * @brief	Reverse the zero-length encoding routine.
 *
 * @param[in]		input	The input sequence of bytes to be rebersed.
 * @return	The reverse ZLE of the input.
 */
Text zleDecoding(const Text input);

/**
 * @brief	Writes in the output array the sequences of zeroes encoded by
 * 			the value pointed by runLen.
 *
 * @param[in] 		runLen	Pointer to the binary number representing the
 * 							number of zeroes of the original input.
 * @param[in,out] 	out		Pointer to the output array where writes the
 * 							zeroes.
 * @param[in,out]	index	Pointer to the index of the next cell to be
 * 							written in the output array.
 */
void outputZeroes(unsigned char *runLen,
				  unsigned char *out,
				  size_t 		*index);


#endif
