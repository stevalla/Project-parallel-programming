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
 * @file 	bwt.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief	Header file implementing the Burrows-Wheeler transformation (BWT).
 *
 * @details This header defines the functions needed to obtain, given a sequence
 * 			of bytes, the Burrows-Wheeler transformation of that sequence. By
 * 			calling the ::bwtTransformation function pssing as argument the sequence
 * 			of bytes, we obtain returned the bwt together with its length. @n
 * 			The function ::getBWT offers the possibility to obtain the BWT, by
 * 			passing as argument the input sequence and the suffix tree of the
 * 			sequence. @n
 * 			Finally the function ::createSuffixArray allows to create the suffix
 * 			array of a sequence of bytes from the suffix tree of that sequence.
 *
 */

#ifndef BWT_H
#define BWT_H

#include "suffixTree.h"

/**
 * @brief  	Returns the BWT transformation of the input text.
 *
 * @param[in]  		input		Sequence of bytes together with its length.
 * @return	The BWT of the input text.
 */
Text bwtTransformation(const Text input);

/**
 * @brief  	Returns the BWT transformation of the input syntax tree.
 *
 * Note that in this case the input text must terminate with the sentinel
 * character. You can call the function ::setSentinel to get the input with
 * the sentinel concatenated.
 *
 * @param[in]   	input		Sequences of bytes with sentinel character at
 * 								the end.
 * @param[in]   	inputLen	Length of the input sequence.
 * @param[in]   	suffixTree	Suffix tree of the input sequence.
 * @return 	The BWT of the syntax tree in input.
 */
Text getBWT(unsigned *const input,
			const 	 size_t	inputLen,
			Node 	 *const	suffixTree);

/**
 * @brief  	Creates the suffix array recursively, starting from the root of the
 * 		   	suffix tree in input and frees the suffix tree.
 *
 * @param[in]  		node		Current pointer to the node of the recursion.
 * @param[in]   	input		Input sequence terminating with the sentinel char.
 * @param[in,out]  	i			Current pointer to the index of the suffix array
 * 								to be filled in.
 * @param[in,out]   suffixArray	Pointer to the suffix array to be created.
 */
void createSuffixArray(Node 	*const 	node,
					   unsigned *const	input,
					   int  	*const  i,
					   int  	*const  suffixArray);

/**
 * @brief  	Set the sentinel at the end of the input Text returning a pointer
 * 		   	to a sequence of unsigned.
 *
 * @param[in]  		input		Sequence of bytes.
 * @return	A pointer to the sequence in input, terminating with the sentinel char.
 */
unsigned *setSentinel(Text input);

/*
 * Function needed to sort the children nodes of one node in the suffix tree.
 *
 * @param  el1, el2 To nodes to be compare
 * @return - -1 if el1 < el2
 * 		   -  1 if el1 > el2
 * 		   -  0 Otherwise
 */
int sortNodesByFirstChar(HashChildren *const el1,
						 HashChildren *const el2);

#endif
