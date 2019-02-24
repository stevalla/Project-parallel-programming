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
 * @file 	unmtf.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing the reverse of the move to front
 * 			transformation.
 */

#include "../headers/unmtf.h"

/**
 * The function is the symmetrical of the MTF transformation. In fact,
 * it searches in the list of symbols the symbol which position is
 * the one in the input, then it outputs the symbol and updates the
 * list, moving the element in front of that. At the beginning,
 * the symbols list is initialized as for the forward transformation.
 */
Text unmtf(const Text input)
{
	SymbolsList *symbols, *aux;
	Text output;

	output.len = input.len;
	output.text = (unsigned char *) malloc(sizeof(unsigned char)*input.len);

	symbols = initListOfSymbols();

	for(unsigned i=0; i<input.len; i++) {

		output.text[i] = searchSymbol(symbols, input.text[i], &aux);

		if(input.text[i] != 0)
			symbols = mvtElement(symbols, aux);

	}

	freeListOfSymbols(symbols);
	free(input.text);

	return output;
}

/**
 * The function implements a simple search of an element in a
 * list of bytes. It outputs the corresponding element at the
 * specified position and, at the same time, it stores in the double
 * pointer aux the element before the one searched in the list, so
 * as to make the next operation of moving ahead the element quicker.
 */
unsigned char searchSymbol(SymbolsList   *const symbols,
				  	  	   const unsigned char  pos,
						   SymbolsList 		  **aux)
{
	SymbolsList *curr, *prev;
	int i;

	if(pos == 0)
		return symbols->symbol;

	i = 1;
	prev=symbols;
	for(curr=symbols->next; curr!=NULL; curr=curr->next) {

		if(i == pos) {
			*aux = prev;
			break;
		}

		i++;
		prev=prev->next;
	}

	return curr->symbol;
}
