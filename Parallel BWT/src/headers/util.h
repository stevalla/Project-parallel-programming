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
 * @file 	util.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing the reverse of the move to front
 * 			transformation.
 *
 * @details This header defines the functions needed to reverse the move
 * 			to front transformation. The main function is ::unmtf that takes
 * 			as input a sequence of bytes and reverses the transformation
 * 			returning the original sequence of bytes. @n
 * 			The utility function ::searchSymbol search the symbol in input
 * 			in the list of symbols having as input the position of the
 * 			symbol.
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>

#define POW_2(N)       		(1 << (N))
#define MAX_CHUNK_SIZE 	(5 * 1024 * 1024)
#define DEFAULT_CHUNK_SIZE	(0.9  * 1024 * 1024)
#define MIN_CHUNK_SIZE    	(300)///< Min size for a chunk to be compressed

typedef struct Text {
	unsigned char *text;
	size_t len;
	long id;

} Text;

// Return the nth big-endian byte of an unsigned int. 0 <= n <= 3
unsigned char decomposeUnsigned(unsigned, int);

// Return a vector<unsigned char> that represents u big-endian.
void encodeUnsigned(const unsigned, unsigned char * const, int);

// Return the unsigned int encoded big-endian by
// the 4 bytes starting at location n.
unsigned readUnsigned(unsigned char * const, size_t);

long fileSize(FILE * const);

Text readFile(FILE * const, long);

void writeFile(FILE * const, unsigned char * const, long);

FILE *openFileRB(char * const);

FILE *openFileWB(char * const);

int compareFiles(FILE * const, FILE * const, long, long);

#endif
