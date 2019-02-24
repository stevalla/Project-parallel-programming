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
 * @file 	parallelBwtZip.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing the parallel version of the
 * 			Burrows-Wheeler compression.
 */

#include "../headers/parallelBwtZip.h"

/**
 *	Shared buffer where the chunks are stored before they begin the
 *	compression.
 */
Buffer readin = {NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

/**
 * Shared buffer to hold the intermediate result between the BWT stage and the
 * MTF stage.
 */
Buffer bwt    = {NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

/**
 * Shared buffer to hold the intermediate result between the ZLE phase and
 * the arithmetic coding phase.
 */
Buffer arith  = {NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

/**
 * Output array to hold the compressed chunks in order before they are written
 * into the output file.
 */
Text *result;

/**
 * Number of blocks (chunks) of the input file to be compressed.
 */
int nBlocks;


struct timespec timeout =
{
		.tv_nsec = 100000,
		.tv_sec = 0
};

/**
 * This function applies the BWT compression to a file in input.
 * The main thread creates other 8 threads: 6 for the BWT phase, 1 for the
 * MTF and ZLE phases and one for the arithmetic phase. Threads communicate
 * with each other by using shared global buffers.
 * Each thread, including the main one, acts as consumer as well as a producer.
 * In fact:
 * 		-	The main thread is the producer of the read buffer because it
 * 			fills in it after the read of one chunk of bytes from the input
 * 			file. Then it acts as a consumer of the result array, because it
 * 			has to write the compressed chunk store in the result in the
 * 			output file.
 * 		-	The bwt thread is the consumer of the read buffer and the producer
 * 			of the bwt buffer.
 * 		-	The mtf and zle thread is the consumer of the bwt buffer and the
 * 			producer of the arith buffer.
 * 		-	Finally the arith thread is the consumer of the arith buffer and the
 * 			producer of the result array.
 *
 *	Each chunk of data is processed asynchronously and they are written in order
 *	given the id of the chunk, setted before the compression starts.
 */
void compressParallel(FILE 		 *input,
					  FILE 		 *output,
					  const long  chunkSize)
{
	int i = 0, index = 0, flag = 0, littleChunk = 0;
	pthread_t 	   threads[NUM_THREADS];
	pthread_attr_t attr;
	cpu_set_t 	   cpus;
	Text 		   inZip;

	//Initialization
	nBlocks = ceil((float)fileSize(input) / (float)chunkSize);

	initBuffer(&readin);
	initBuffer(&bwt);
	initBuffer(&arith);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	result = (Text *) malloc(sizeof(Text) * nBlocks);

	for(int j=0; j<nBlocks; j++)
		result[j].id = -1;

	if(fileSize(input) - (chunkSize * (nBlocks - 1)) < MIN_CHUNK_SIZE) {
		nBlocks--;
		littleChunk = 1;
	}

	CPU_ZERO(&cpus);
	CPU_SET(0, &cpus);
	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpus);

	//Fill the read buffer at the beginning with some chunks
	for(int j=0; j<6; j++) {

		inZip = readFile(input, chunkSize);

		inZip.id = i++;

		if(inZip.len == 0) {
			flag = 1;
			break;
		}

		if(inZip.len < MIN_CHUNK_SIZE) {
			result[inZip.id] = inZip;
			flag = 1;
			break;
		}

		enqueue(inZip, readin.queue);
	}

	//Creates threads
	for(int j=0; j<5; j++) {
		setAffinity(&cpus, j+1, &attr);
		pthread_create(&threads[j], &attr, bwtStage, NULL);
	}

	setAffinity(&cpus, 0, &attr);
	pthread_create(&threads[7], &attr, bwtStage, NULL);
	setAffinity(&cpus, 6, &attr);
	pthread_create(&threads[5], &attr, mtfZleStage, NULL);
	setAffinity(&cpus, 7, &attr);
	pthread_create(&threads[6], &attr, arithStage, NULL);

	//Read the remaining chunks
	for(int j=0; j<nBlocks-6 && !flag; j++) {

		usleep(10000);

		inZip    = readFile(input, chunkSize);
		inZip.id = i++;

		pthread_mutex_lock(&readin.mutex);
		enqueue(inZip, readin.queue);
		pthread_cond_signal(&readin.cond);
		pthread_mutex_unlock(&readin.mutex);
	}

	//If there war a chunk too little to be compressed output it not compressed.
	if(littleChunk) {
		inZip = readFile(input, chunkSize);
		inZip.id++;

		result[inZip.id] = inZip;
	}

	//Write output as soon as they are available, in order.
	while(index < nBlocks) {
		writeOutput(output, &index, littleChunk);
		usleep(1000);
	}

	if(flag)
		free(inZip.text);

	free(result);
	pthread_attr_destroy(&attr);
}

/**
 * Performs the bwt on a chunk of data read from the read buffer as soon it
 * is available. Note that the thread must first lock the mutex on the read
 * buffer and checks that it is not empty. Then it can read the data and
 * perform the operation. Finally it needs to lock the lock on the output
 * buffer, in this case the bwt buffer, before written the result into the
 * buffer.
 */
void *bwtStage(void *arg)
{
	Text bwtInput;

	while(1) {

		pthread_mutex_lock(&readin.mutex);
		while(empty(readin.queue) && readin.queue->counter < nBlocks)
			pthread_cond_timedwait(&readin.cond, &readin.mutex, &timeout);

		if(!empty(readin.queue))
			bwtInput = dequeue(readin.queue);

		else if(readin.queue->counter == nBlocks) {
			pthread_mutex_unlock(&readin.mutex);
			break;

		} else {
			pthread_mutex_unlock(&readin.mutex);
			continue;
		}
		pthread_mutex_unlock(&readin.mutex);

		Text bwtOutput = bwtTransformation(bwtInput);

		pthread_mutex_lock(&bwt.mutex);
		enqueue(bwtOutput, bwt.queue);
		pthread_cond_signal(&bwt.cond);
		pthread_mutex_unlock(&bwt.mutex);
	}
	return 0;
}

/**
 * Performs the bwt on a chunk of data read from the bwt buffer as soon it
 * is available. Note that the thread must first lock the mutex on the bwt
 * buffer and checks that it is not empty. Then it can read the data and
 * perform the operation. Finally it needs to lock the lock on the output
 * buffer, in this case the arith buffer, before written the result into the
 * buffer.
 */
void *mtfZleStage(void *arg)
{
	Text mtfInput;

	while(1) {

		pthread_mutex_lock(&bwt.mutex);
		while(empty(bwt.queue) && bwt.queue->counter < nBlocks)
			pthread_cond_timedwait(&bwt.cond, &bwt.mutex, &timeout);

		if(!empty(bwt.queue))
			mtfInput = dequeue(bwt.queue);

		else if(bwt.queue->counter == nBlocks) {
			pthread_mutex_unlock(&bwt.mutex);
			break;

		} else {
			pthread_mutex_unlock(&bwt.mutex);
			continue;
		}
		pthread_mutex_unlock(&bwt.mutex);

		Text mtfOutput = mtf(mtfInput);
		Text zleOutput = zleEncoding(mtfOutput);

		pthread_mutex_lock(&arith.mutex);
		enqueue(zleOutput, arith.queue);
		pthread_cond_signal(&arith.cond);
		pthread_mutex_unlock(&arith.mutex);
	}
	return 0;
}

/**
 * Performs the bwt on a chunk of data arith from the read buffer as soon it
 * is available. Note that the thread must first lock the mutex on the arith
 * buffer and checks that it is not empty. Then it can read the data and
 * perform the operation. Finally it needs to lock the lock on the output
 * array, in this case the result array, before written the result into the
 * array.
 */
void *arithStage(void *arg)
{
	Text arithInput;

	while(1) {

		pthread_mutex_lock(&arith.mutex);
		while(empty(arith.queue) && arith.queue->counter < nBlocks)
			pthread_cond_timedwait(&arith.cond, &arith.mutex, &timeout);

		if(!empty(arith.queue))
			arithInput = dequeue(arith.queue);

		else if(arith.queue->counter == nBlocks) {
			pthread_mutex_unlock(&arith.mutex);
			break;

		} else {
			pthread_mutex_unlock(&arith.mutex);
			continue;
		}
		pthread_mutex_unlock(&arith.mutex);

		Text compressed = encodingRoutine(arithInput);

		result[compressed.id] = compressed;
	}
	return 0;
}


void initBuffer(Buffer *const buf)
{
	buf->queue = (Queue *) malloc(sizeof(Queue));
	initQueue(buf->queue);
}

/**
 * The main thread continuously calls this function to check if the
 * next block to be output is available. @n
 * The index keeps trace of the id of the next block to output. The id of the
 * result are initialized to -1 so the main thread knows when a compressed
 * block has been stored into the array.
 */
void writeOutput(FILE *output, int *const index, const int littleChunk)
{
	unsigned char length[4];
	unsigned char id[1];
	int i = *index;

	for(; i<nBlocks; i++) {

		if(result[i].id != *index)
			return;

		encodeUnsigned(result[i].len, length, 0);

		if(i == nBlocks - 1 && littleChunk)
			id[0] = 0;
		else
			id[0] = 1;

		writeFile(output, length, 4);
		writeFile(output, id, 1);
		writeFile(output, result[i].text, result[i].len);

		free(result[i].text);
		(*index)++;
	}
}

void setAffinity(cpu_set_t 		*const cpus,
				 int 				   cpu,
				 pthread_attr_t *const attr)
{
	cpu = cpu%8;

	CPU_ZERO(cpus);
	CPU_SET(cpu, cpus);
	pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), cpus);
}

