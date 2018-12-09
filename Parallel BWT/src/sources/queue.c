#include "../headers/queue.h"

void initQueue(Queue *q)
{
	q->counter = 0;
	q->rear = NULL;
	q->front = NULL;
}

int empty(Queue *q)
{
	return (q->rear == NULL);
}

void enqueue(Text elem, Queue *q)
{
	Elem *new = (Elem *) malloc(sizeof(Elem));
	new->elem = elem;
	new->next = NULL;

	if(!empty(q)) {

		q->rear->next = new;
		q->rear = new;

	} else
		q->front = q->rear = new;

	q->counter++;
}

Text dequeue(Queue *q)
{
	Elem *tmp;
	Text elem = q->front->elem;

	tmp = q->front;
	q->front = q->front->next;
	q->counter--;

	free(tmp);

	return elem;
}
