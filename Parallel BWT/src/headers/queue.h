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
 * @file 	queue.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing a queue.
 *
 * @details	This header defines the function to implement a queue of ::Text
 * 			using a linked list. The ::Queue data structure has two pointer for
 * 			the front and the rear of the queue, respectively. In addition it has
 * 			also a counter for counting the number of element in the queue.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "util.h"

/**
 * Linked list for the queue.
 */
typedef struct Elem
{
	Text 	     elem;		///<	One element of the queue.
	struct Elem *next;		///<	Pointer to the next element in the queue.

} Elem;

/**
 * Queue. The counter counts how many elements enter in the queue, but then
 * it does not decrement the counter. So it just counts how many element are
 * entered into the queue.
 */
typedef struct Queue
{
	int   counter;			/**		Stores the number of element entered in
	 	 	 	 	 	 	 	 	the queue. */
	Elem *front;			/**<	Pointer to the first element to be filled
	 	 	 	 	 	 	 	 	in of the queue. */
	Elem *rear;				///<	Pointer to the last element of the queue.

} Queue;

/**
 * @brief	Queue initialization.
 *
 * @param[in,out]	q	The queue to initialize.
 */
void initQueue(Queue *q);

/**
 * @brief	Adds an element at the front of the queue.
 *
 * @param[in]		elem	The element to be added.
 * @param[in,out] 	   q	The queue qhere the element must be added.
 */
void enqueue(Text elem, Queue *q);

/**
 * @brief	Deletes an element from the rear of the queue and returns it.
 *
 * @param[in,out] 	q		The queue where the element must be retrieved.
 * @return	The element dequeued.
 */
Text dequeue(Queue *q);

/**
 * @brief	Checks if the queue is empty.
 *
 * @param[in] 		q		The queue.
 * @return
 * 			-	1 if the queue is empy.
 * 			-	0 otherwise.
 */
int empty(Queue *q);


#endif
