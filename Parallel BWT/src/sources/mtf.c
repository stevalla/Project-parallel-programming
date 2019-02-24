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
 * @file 	mtf.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing the move to front transformation.
 */

#include "../headers/mtf.h"

#define SIZE_SYMBOLS_LIST 256		///< Size of the list of symbols.

/**
 * The function scan the input array of bytes and for each byte
 * it performs three actions:
 */
Text mtf(const Text input)
{
	SymbolsList *symbols, *aux;
	Text output;
	int tmp;

	output.id   = input.id;
	output.len  = input.len;
	output.text = (unsigned char *) malloc(sizeof(unsigned char)*input.len);
	symbols     = initListOfSymbols();

	for(unsigned i=0; i<input.len; i++) {

		/// - Search the position of the input byte.
		tmp = search(symbols, input.text[i], &aux);

		/// - Write the position in the output array.
		if(tmp != -1)
			output.text[i] = (unsigned char) tmp;
		else {
			fprintf(stderr, "Error during MTF phase.\n");
			exit(-1);
		}

		/// - Move to the head of the symbol list the read byte.
		if(aux != NULL)
			symbols = mvtElement(symbols, aux);
	}

	freeListOfSymbols(symbols);
	free(input.text);

	return output;
}

/**
 * The function implements a simple search of an element in a
 * list of bytes. It outputs the corresponding position of the element and, at
 * the same time, it stores in the double pointer aux the element before
 * the one searched in the list, so as to make the next operation of moving
 * ahead the element quicker.
 */
int search(SymbolsList *const  symbols,
		   const unsigned char byte,
		   SymbolsList 		   **aux)
{
	SymbolsList *curr, *prev;
	int position = 0;

	// If the byte is at the head of the list return 0.
	if(symbols->symbol == byte) {
		*aux = NULL;
		return position;
	}

	// Otherwise search the byte and return its position in the list.
	position = 1;
	prev=symbols;
	for(curr=symbols->next; curr!=NULL; curr=curr->next) {
		if(curr->symbol == byte) {
			*aux = prev;
			return position;
		}
		position++;
		prev=prev->next;
	}

	return -1;
}

/**
 * This function moves the element pointed by the one given in input to the
 * front of the list of symbols. The function is carried out with two
 * operations:
 */
SymbolsList *mvtElement(SymbolsList *const symbols,
						SymbolsList *el)
{
	SymbolsList *current = el->next;

	/// - Delete element from the list of symbols.
	el->next = el->next->next;

	/// - Re-insert it at the head of the list.
	current->next = symbols;

	return current;
}

/**
 * Populate the list of symbols with characters from 0 to 255
 */
SymbolsList *initListOfSymbols()
{
	SymbolsList *symbols, *tmp;
	int i=0;

	symbols 		= (SymbolsList *) malloc(sizeof(SymbolsList));
	symbols->symbol = (unsigned char)i;
	tmp 			= symbols;

	for(i=1; i<SIZE_SYMBOLS_LIST; i++) {

		tmp->next 	= (SymbolsList *) malloc(sizeof(SymbolsList));
		tmp 		= tmp->next;
		tmp->symbol = (unsigned char)i;
	}
	tmp->next = NULL;

	return symbols;
}

/**
 * Free the list of symbols and deallocate memory.
 */
void freeListOfSymbols(SymbolsList *symbols)
{
	SymbolsList *tmp;

	while(symbols != NULL) {
		tmp = symbols->next;
		free(symbols);
		symbols = tmp;
	}
}
