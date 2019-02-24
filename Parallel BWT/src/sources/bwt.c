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
 * @file 	bwt.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief	Sourcefile implementing the Burrows-Wheeler transformation (BWT).
 */

#include "../headers/bwt.h"
#include "../extern/uthash.h"

#define SENTINEL 256;	///< Sentinel character.

/**
 * This function return the BWT transformation of the input text in three phases:
 * 		- First set the sentinel character in the input text. This step is
 * 		  fundamental to obtain the suffix array, since the transformation is
 * 		  reversible if and only if the input text ends with a character different
 * 		  from all the others.
 * 		- Second the suffix tree is built.
 * 		- Finally the BWT is created by calling the function ::getBWT.
 */
Text bwtTransformation(const Text input)
{
	const size_t bwtLen = input.len + 1;
	unsigned *in;

	in = setSentinel(input);

	Node *suffixTree = buildSuffixTree(in, bwtLen);

	Text result = getBWT(in, bwtLen, suffixTree);

	result.id = input.id;

	return result;
}

/**
 * This function encodes the BWT in the output array of bytes. To make the
 * transformation reversible we store in the output array also the index of the
 * sentinel character and the index of the first character of the input. These
 * two index can be bigger than a byte, so they are encoded in two bytes each. @n
 * The output array will have:
 * 		- First two bytes storing the encode unsigned index of the first char.
 * 		- Third and fourth byte storing the encode usigned index of the sentinel.
 * 		- The sentinle character itself is encoded as zero.
 *
 * In this way we can maintain the result in an array of unsigned char. @n
 * The operation is performed in two steps:
 */
Text getBWT(unsigned *const input,
			const 	 size_t inputLen,
			Node 	 *const suffixTree)
{
	int *suffixArray;
	int  i, index;
	Text bwt;

	i = 0;
	suffixArray = (int *)malloc(sizeof(int)*(inputLen));
	bwt.len = inputLen + 8;
	bwt.text = (unsigned char *)malloc(sizeof(unsigned char)*(bwt.len));

	/// - Create suffix array starting from the suffix tree.
	createSuffixArray(suffixTree, input, &i, suffixArray);

	/// - Extract the BWT from the suffix array.
	for(i=0; i<inputLen; i++) {
		index = suffixArray[i];

		if(index == 0) {						//Index of the sentinel character
			bwt.text[i + 8] = 0;				//Dummy character
			encodeUnsigned(i, bwt.text, 4);

		} else if(index == 1) {					//Index of first character
			bwt.text[i + 8] = input[0];
			encodeUnsigned(i, bwt.text, 0);

		} else
			bwt.text[i + 8] = input[index - 1];
	}

	free(suffixArray);
	free(input);

	return bwt;
}

/**
 * Lexicographic depth-first search traversal to create the suffix array.
 * The suffix tree is recursively traversed and as soon as a node and all
 * its children have been visited they are deleted.
 */
void createSuffixArray(Node 	*const 	node,
					   unsigned *const	input,
		   	   	   	   int  	*const  i,
					   int  	*const  suffixArray)
{
	HashChildren *child;
	int num_children = HASH_COUNT(node->children);

	//Base case
	if(num_children == 0) {
		suffixArray[(*i)++] = node->suffixIndex;
		deleteNode(node);							//Delete leaf
		return;
	}

	//Recursion
	HASH_SORT(node->children, sortNodesByFirstChar);

	for(child=node->children; child!=NULL; child=child->hh.next)
		createSuffixArray(child->node, input, i, suffixArray);

	deleteNode(node);								//Delete internal node
}


int sortNodesByFirstChar(HashChildren *const el1,
						 HashChildren *const el2)
{
	if(     el1->firstChar < el2->firstChar)	return -1;
	else if(el1->firstChar > el2->firstChar)	return  1;
	else 										return  0;
}

/**
 * This functions returns the input text with the sentinel character concatenated
 * at the end. The pointer returned is unsigned so that it can contains also
 * the sentinel character (i.e. char 256). Since the input text is of type
 * unsigned char, we cna be sure that the sentinel is the only character in
 * the text.
 */
unsigned *setSentinel(Text input)
{
	unsigned *bwtInput = (unsigned *)malloc(sizeof(unsigned)*(input.len + 1));
	int i;

	for(i=0; i<input.len; i++)
		bwtInput[i] = input.text[i];
	bwtInput[i] = SENTINEL;

	free(input.text);

	return bwtInput;
}
