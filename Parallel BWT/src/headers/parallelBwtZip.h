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
 * @file 	parallelBwtZip.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing the parallel version of the
 * 			Burrows-Wheeler compression.
 *
 * @details	This interface defines the threads and the functions needed to
 * 			implement the BWT compression in parallel. @n
 * 			The four task of the BWT algorithm has been divided into three set:
 * 				-	One for the Burrows-Wheeler transformation.
 * 				-	One for the move to front transformation and the zero-length
 * 					encoding.
 * 				-	One for the arithmetic coding compression.
 *
 * 			The main thread reads the file chunk by chunk and outputs the
 * 			compressed chunks as soon as they become available. @n
 * 			The header offers also a function to set a particular logical core
 * 			to a thread.
 */

#ifndef PARALLEL_BWT_ZIP_H
#define PARALLEL_BWT_ZIP_H

#define _GNU_SOURCE
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sched.h>

#include "bwt.h"
#include "mtf.h"
#include "zle.h"
#include "arith.h"
#include "queue.h"
#include "util.h"

#define NUM_THREADS 8			///< Number of threads.

/**
 * Shared buffer data structure to implement the consumer-producer pattern
 * to make the thread communicates.
 */
typedef struct Buffer
{
	Queue 		 	*queue;		///<	The shared queue.
	pthread_mutex_t  mutex;		///<	The mutex of the queue.
	pthread_cond_t	 cond;		///<	The condition for the empty queue.

} Buffer;

/**
 * @brief	Performs the parallel BWT compression.
 *
 * @param[in]		input		The input file to compress.
 * @param[in,out]	output		The output where to write the compressed file.
 * @param[in]	 	chunkSize	The size of the chunks in which the file must
 * 								be splitted before compression.
 */
void compressParallel(FILE 		 *input,
					  FILE 		 *output,
					  const long  chunkSize);

/**
 * @brief	Thread for the Burrow-Wheeler transform stage.
 *
 * @param	arg NULL.
 */
void *bwtStage(void *);

/**
 * @brief	Thread for the MTF and ZLE stage.
 *
 * @param	arg NULL.
 */
void *mtfZleStage(void *);

/**
 * @brief	Thread for the arithmetic coding stage.
 *
 * @param	arg	NULL.
 */
void *arithStage(void *);

/**
 * @brief	Initialize a queue buffer.
 *
 * @param[in,out]	 buf	The buffer to be initialized.
 */
void initBuffer(Buffer *const buf);

/**
 * @brief	Write the compressed output from the result array to the
 * 			output file.
 *
 * @param[in]		output		Pointer to the result to be written out.
 * @param[in,out] 	index		Pointer to the index of the next cell of
 * 								the output array to be written out.
 * @param[in] 		littleChunk	0 if the chunk has not been compressed, 1
 * 								otherwise
 */
void writeOutput(FILE *output, int *const index, const int littleChunk);

/**
 * @brief	Set a specific core where a thread will be run.
 *
 * @param[in] 		cpus	Pointer to the number of cpus.
 * @param[in,out] 	cpu		The cpu number to be set for the thread.
 * @param[in,out] 	attr	Pointer to the attribute of the thread that
 * 							must run on the specific core.
 */
void setAffinity(cpu_set_t 		*const cpus,
		 	 	 int 				   cpu,
				 pthread_attr_t *const attr);

#endif
