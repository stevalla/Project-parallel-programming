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
 * @file 	queue.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing a queue.
 */

#include "../headers/queue.h"


void initQueue(Queue *q)
{
	q->counter = 0;
	q->rear    = NULL;
	q->front   = NULL;
}


void enqueue(Text elem, Queue *q)
{
	Elem *new = (Elem *) malloc(sizeof(Elem));
	new->elem = elem;
	new->next = NULL;

	if(!empty(q)) {

		q->rear->next = new;
		q->rear       = new;

	} else
		q->front = q->rear = new;

	q->counter++;
}

Text dequeue(Queue *q)
{
	Elem *tmp = q->front;
	Text elem = q->front->elem;

	q->front  = q->front->next;

	free(tmp);

	if(q->front == NULL)
		q->rear  = NULL;

	return elem;
}


int empty(Queue *q)
{
	return (q->rear == NULL);
}
