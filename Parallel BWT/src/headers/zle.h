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
 * @file 	zle.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing a zero length encoder.
 *
 * @details This header defines the functions needed to perform the zero length
 * 			encoding, third phase of the BWT compression. @n
 * 			This type of compression has been developed by D.J.Wheeler and it is a kind
 * 			of run-length encoding, that encodes sequences of zeroes in a number that
 * 			holds the length of the zero-run. @n
 * 			Given a sequence of zeroes of length N, it converts (N + 1) to binary and
 * 			outputs the binary representation, starting from the LSB and leaving out
 * 			the MSB.<br/>
 *
 * ###Examples
 *
 * - Cleartext sequence of zeroes of length: 1, 2,  3,  4,  5,  6,   7,   8,   9
 * - Encoded: N + 1 in binary, MSB elided:   0, 1, 00, 01, 10, 11, 000, 001, 010 @n@n
 *
 * - Cleartext: A byte B = 0x01 or 0x02 or ... or 0xFD
 * - Encoded: B + 1 @n@n
 *
 * - Cleartext: A 0xFE byte
 * - Encoded: 0xFF 0x00 @n@n
 *
 * - Cleartext: A 0xFF byte
 * - Encoded: 0xFF 0x01
 *
 * @see Burrows Wheeler Compression: Principles and Reflections, Peter Fenwick,
 * 		Private Bag 92019, Auckland, New Zealand.
 */

#ifndef ZLE_H
#define ZLE_H

#include "util.h"

/**
 * @brief	Performs the zero length encoding.
 *
 * @param[in] 		input	Sequence of bytes together with its length.
 * @return 	The text encoded as describe above.
 */
Text zleEncoding(const Text input);

/**
 * @brief	Encodes and stores the zero-run in input.
 *
 * @param[in] 		runLen	The run of zeroes to encode.
 * @param[in,out] 	res		Pointer to the actual result.
 */
void encodeZeroRun(size_t runLen, Text *res);

#endif
