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
 * @file 	unbwt.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Header file implementing the reverse Burrows-Wheeler
 * 			transformation.
 *
 * @details	The unique function offered by the interface allows to
 * 			reverse the BWT of the string. @n
 * 			The input string must have the index of the first char of
 * 			the original string, as well as the index of the sentinel char
 * 			encoded in 4 bytes each at the beginning of the input.
 */

#ifndef UNBWT_H
#define UNBWT_H

#include "util.h"

/**
 * @brief	Returns the reverse BWT of the input sequence of bytes.
 *
 * @param[in] input	The sequence of bytes to be reversed.
 * @return	The reverse BWT of the input string.
 */
Text unbwt(Text input);

#endif
