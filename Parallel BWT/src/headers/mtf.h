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
 * @file 	mtf.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing the move to front transformation.
 *
 * @details This header defines the functions needed to perform the move to
 * 			front (MTF) transformation, that is the second phase of the BWT
 * 			compression. @n
 *			The algorithm takes an input byte and outputs the number of distinct
 *			other bytes that have been seen since its last occurrence. At the
 *			beginning the 0x00 byte is assumed to be the most recently occurred,
 *			with the other bytes in sequence.@n
 *			The implementation is done by means of a list of 256 bytes, where the
 *			most recently seen bytes are kept in order from position 0. When a new
 *			byte is read, its position in the list is output and the byte is moved
 *			in front of the list. The output is still a sequence of bytes
 *			from 0 to 255.
 *
 * @note	If the most recently byte is read, i.e. we have at least two consecutive
 * 			equal characters, the algorithm outputs 0 for the second one.
 */

#ifndef MTF_H
#define MTF_H

#include "util.h"

/**
 * A list of symbols, from 0 to 255.
 */
typedef struct SymbolsList
{
	unsigned char symbol;		///< One symbol.
	struct SymbolsList *next;	///< Pointer to the next symbol in the list.

} SymbolsList;

/**
 * @brief	Transform the input sequence of bytes into an array of output
 * 			bytes, indices of the symbol list.
 *
 * @param[in] 		input	The input sequence of bytes together with its length.
 * @return	The transformed array of bytes.
 */
Text mtf(const Text input);

/**
 * @brief	Returns the position of the searched byte in the list of
 * 			symbols, or -1 if the element is not found.
 *
 * @param[in] 		symbols		Pointer to the head of the list of symbols.
 * @param[in]		byte		The byte to be searched in the list.
 * @param[in,out]	aux			Pointer to the previous element of the searched
 * 								byte in the list, useful later to move ahead the
 * 								returned element.
 * @return	The position of the byte in the list of symbols.
 * 			The function returns -1 (error) if the element is not found.
 */
int search(SymbolsList   *const  symbols,
		   const unsigned char 	 byte,
		   SymbolsList 		   	 **aux);

/**
 * @brief	Move the element pointed by el to the front of the list
 * 			in input.
 *
 * This operation is performed only if the searched element was not at the head
 * of the list.
 *
 * @param[in,out]	symbols		Pointer to the head of the list of symbols.
 * @param[in] 		el			The element before the one to be moved in front of
 * 								the list.
 * @return	The current head of the list.
 */
SymbolsList *mvtElement(SymbolsList *const symbols, SymbolsList *el);

/**
 * @return	The list of symbols initialized.
 */
SymbolsList *initListOfSymbols(void);

/**
 * @param[in] 		symbols		Pointer to the head of the list to be freed.
 */
void freeListOfSymbols(SymbolsList *symbols);


#endif
