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
 * @file 	bwtUnzip.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing the sequential version of the
 * 			Burrows-Wheeler decompression.
 *
 * @details	This interface provides the functions needed to decompress
 * 			a given file. The function ::bwtUnzip decompresses a given
 * 			sequence of bytes and returns a decompressed sequence of bytes.
 * 			While ::decompress decompresses an entire file, previously
 * 			compressed. The file must respect the format defined in the
 * 			compression version.
 */

#ifndef BWT_UNZIP_H
#define BWT_UNZIP_H

#include "parallelBwtZip.h"
#include "unbwt.h"
#include "unmtf.h"
#include "unzle.h"
#include "unarith.h"

/**
 * @brief	Performs the BWT decompression.
 *
 * @param[in]	input	The sequence of bytes to decompress.
 * @return	The sequence of bytes decompressed.
 */
Text bwtUnzip(const Text input);

/**
 * @brief	Decompress a given input file, writing the result in the
 * 			specified output file.
 *
 * @param[in] 	input		The file to decompress
 * @param[out]	output		The file where decompress.
 */
void decompress(FILE *input, FILE *output);

#endif
