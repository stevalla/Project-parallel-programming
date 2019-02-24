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
 * @file 	unmtf.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing the reverse of the move to front
 * 			transformation.
 *
 * @details This header defines the functions needed to reverse the move
 * 			to front transformation. The main function is ::unmtf that takes
 * 			as input a sequence of bytes and reverses the transformation
 * 			returning the original sequence of bytes. @n
 * 			The utility function ::searchSymbol search the symbol in input
 * 			in the list of symbols having as input the position of the
 * 			symbol.
 */

#ifndef UNMTF_H
#define UNMTF_H

#include "mtf.h"

/**
 * @brief	Reverses the move to front transformation of the input sequence
 * 			of bytes.
 *
 * @param[in] 	input		The input sequence to be reversed.
 * @return	The reversed MTF of the input.
 */
Text unmtf(const Text input);

/**
 * @brief	Search the symbol in the list at the position specified in
 * 			input.
 *
 * @param[in] 	  symbols	Pointer to the head of the list of symbols.
 * @param[in] 	  pos		Position of the symbol to be searched.
 * @param[in,out] aux		Pointer to the previous element of the searched
 * 							symbol in the list, useful later to move ahead
 * 							the returned element.
 * @return	The symbol in the list of symbols at the position specified.
 */
unsigned char searchSymbol(SymbolsList   *const symbols,
						   const unsigned char  pos,
						   SymbolsList        **aux);

#endif
