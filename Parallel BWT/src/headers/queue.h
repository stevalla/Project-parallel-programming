#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

typedef struct Elem
{
	Text elem;
	struct Elem *next;

} Elem;

typedef struct Queue
{
	int counter;
	Elem *front;
	Elem *rear;

} Queue;


void initQueue(Queue *);

void enqueue(Text, Queue *);

Text dequeue(Queue *);

int empty(Queue *);


#endif
