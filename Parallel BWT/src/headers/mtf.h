#ifndef MTF_H
#define MTF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Linked list containing the table of symbols
typedef struct SymbolsList
{
	unsigned char symbol;
	struct SymbolsList *next;

} SymbolsList;

/*
 * This structure keeps track of the position
 * of the current element in the table of symbols
 * and of the previous element of it in the table
 * of symbols. This make easier perform the move
 * to front operation on the linked list of symbols.
 */
typedef struct MtfAux
{
	unsigned char pos;
	SymbolsList *prev;

} MtfAux;


void search(SymbolsList *const, const unsigned char, MtfAux *const);

unsigned char *mtf(unsigned char *const, const int, const size_t);

SymbolsList *mvtElement(SymbolsList *const, MtfAux *const);

SymbolsList *mvtElement2(SymbolsList *const, MtfAux *const);

SymbolsList *initListOfSymbols(void);

void freeListOfSymbols(SymbolsList *);


#endif
