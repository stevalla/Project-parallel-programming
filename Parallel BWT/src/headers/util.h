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
 * @brief 	Header file implementing a util interface for encoding
 * 			bytes and managing files.
 *
 * @details This interface defines some functions for encoding unsigned
 * 			values and for managinfg files.
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>

#define POW_2(N)       		(1 << (N))				///< 	2^N.
#define MAX_CHUNK_SIZE 		(5 * 1024 * 1024)		///<	Max chunk size allowed.
#define DEFAULT_CHUNK_SIZE	(0.9  * 1024 * 1024)	///<	Default chunk size.
#define MIN_CHUNK_SIZE    	(300)	///< Min size for a chunk to be compressed.


/**
 * This structure is the one who stores the sequence of bytes to be compress
 * or decompress  together with its length. In addition, the id variable stores
 * the id of the chunk during compression. This is foundamental for the parallel
 * version of the compression algorithm, since the execution of the chunk
 * compression could be out of order, so they need to be ordered at the end of the
 * compression.
 */
typedef struct Text {
	unsigned char *text;	///<	Pointer to the sequence of bytes.
	size_t 		   len;		///<	The length of the sequence of bytes.
	long 		   id;		///<	The id of the chunk size.

} Text;

/**
 * @brief	Utility function to encode an unsigned.
 *
 * @param[in] 		u			The unsigned to decompose.
 * @param[in] 		n			The byte of the decomposed unsigned.
 * @return	Returns the nth big-endian byte of an unsigned int. (0 <= n <= 3)
 */
unsigned char decomposeUnsigned(unsigned u, int n);

/**
 * @brief	Stores in an array of unsigned char that represents ul big-endian.
 *
 * @param[in]		u			The unsigned to encode.
 * @param[out]		output		Pointer to the output unsigned char where the result
 * 								is stored.
 * @param[in,out]	outIndex	Index of the first element of the output array to
 * 								be write.
 */
void encodeUnsigned(const unsigned u, unsigned char *const output, int outIndex);

/**
 * @brief	Returns the unsigned encoded big endian by the 4 bytes of the input
 * 			from location n.
 *
 * @param[in]		input	Pointer to the encoded unsigned int.
 * @param[in] 		n		The index of the first byte of the encoded unsigned.
 * @return	Returns the unsigned int encoded big-endian by the 4 bytes starting
 * 			at location n.
 */
unsigned readUnsigned(unsigned char *const input, size_t n);

/**
 * @brief	Open a file for reading it.
 *
 * @param[in]		filename	The filename of the file to be opened.
 * @return	The file construct of the opened file.
 */
FILE *openFileRB(char *const filename);

/**
 * @brief	Reads at most size bytes from the file in input.
 *
 * @param[in]		file		The file to be read.
 * @param[in]		size		The number of bytes to read.
 * @return	The sequence of bytes of the size specified read in the file in input.
 */
Text readFile(FILE *const file, long size);

/**
 * @brief	Opens a file for wiriting into it.
 *
 * @param[in]		filename	The filename of the file to be opened.
 * @return	The file construct of the opened file.
 */
FILE *openFileWB(char *const filename);

/**
 * @brief	Writes the buffer into the file.
 *
 * @param[in]		file		The file to be written.
 * @param[in] 		buffer		The buffer to be written into the file.
 * @param[in]		len			The length of the sequence of bytes to be written.
 */
void writeFile(FILE 		 *const file,
			   unsigned char *const buffer,
			   long					len);

/**
 * @brief	gets the number of bytes of the file.
 *
 * @param[in]		file	Input file.
 * @return	The number of bytes of the file.
 */
long fileSize(FILE *const file);

/**
 * @brief	Compare two files byte by byte.
 *
 * @param[in]		file1		The first file to compare.
 * @param[in]		file2		The second file to compare.
 * @param[in]		size1		The number of bytes of the first file-
 * @param[in]		size2		The number of bytes of the second files.
 * @return
 * 			-	2 if the two sizes are different.
 * 			-	1 if the two files are equal.
 * 			-	0 if the two files are different.
 */
int compareFiles(FILE *const file1, FILE *const file2, long size1, long size2);

#endif
