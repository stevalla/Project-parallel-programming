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
 * @file 	sequentialBwtZip.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing the sequential version of the
 * 			Burrows-Wheeler compression.
 *
 * @details	This interface provides the functions needed to compress
 * 			a given file. The function ::bwtZip compressed a given sequence
 * 			of bytes and returns a compressed sequence of bytes. While
 * 			::compressSequential compresses an entire file, divided into
 * 			chunks of some specified size.
 */

#ifndef SEQUENTIAL_BWT_ZIP_H
#define SEQUENTIAL_BWT_ZIP_H

#include "bwt.h"
#include "mtf.h"
#include "zle.h"
#include "arith.h"

/**
 * @brief Performs the sequential BWT compression.
 *
 * @param[in] 	input		The sequence of bytes to compress.
 * @return	The compressed sequence of bytes.
 */
Text bwtZip(const Text input);

/**
 * @brief	Compress a given input file, writing the result in the
 * 			specified output file.
 *
 * @param[in] 	input		The file to compress
 * @param[out]	output		The file where compress.
 * @param[in] 	chunkSize	The size of the chunks in which the file must
 * 							be splitted for compression.
 */
void compressSequential(FILE *input, FILE *output, long chunkSize);

#endif
