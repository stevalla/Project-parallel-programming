#ifndef BWT_ZIP_H
#define BWT_ZIP_H

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


void compress(FILE *, FILE *, const long);

void writeOutput(FILE *, int *);

void insertInOrderResult(Text);

void initBuffer(Buffer *);

void freeBuffer(Buffer *);

void *bwtStage(void *);

void *mtfZleStage(void *);

void *arithStage(void *);

void setAffinity(cpu_set_t *, int, pthread_attr_t *);


#endif
