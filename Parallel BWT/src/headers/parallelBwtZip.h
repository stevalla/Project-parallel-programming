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


typedef struct ResultList
{
	Text result;
	struct ResultList *next;

} ResultList;

typedef struct Result
{
	ResultList *resultList;
	pthread_mutex_t mutex;

} Result;


static Buffer readin = {NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
static Buffer bwt = {NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
static Buffer arith = {NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

void compressParallel(FILE *, FILE *, const long);

void writeOutput(FILE *, int *);

void writeOutputArray(FILE *, int *);

void insertInOrderResult(Text);

void initBuffer(Buffer *);

void *bwtStage(void *);

void *mtfZleStage(void *);

void *arithStage(void *);

void setAffinity(cpu_set_t *, int, pthread_attr_t *);


#endif
