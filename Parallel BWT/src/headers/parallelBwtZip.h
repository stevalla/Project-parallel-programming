#ifndef PARALLEL_BWT_ZIP_H
#define PARALLEL_BWT_ZIP_H

#define _GNU_SOURCE
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sched.h>

#include "bwt.h"
#include "mtf.h"
#include "zle.h"
#include "arith.h"
#include "queue.h"

#define NUM_THREADS 8

typedef struct Buffer
{
	Queue 		    *queue;
	pthread_mutex_t  mutex;
	pthread_cond_t	 cond;

} Buffer;

typedef struct Result
{
	Text *text;
	pthread_mutex_t mutex;

} Result;

void compressParallel(FILE *, FILE *, const long);

void writeOutput(FILE *, int *const, const int);

void initBuffer(Buffer *const);

void *bwtStage(void *);

void *mtfZleStage(void *);

void *arithStage(void *);

void setAffinity(cpu_set_t *const, const int, pthread_attr_t *const);


#endif
