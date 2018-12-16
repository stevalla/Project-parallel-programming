#ifndef MTF_H
#define MTF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

//Linked list containing the table of symbols
typedef struct SymbolsList
{
	unsigned char symbol;
	struct SymbolsList *next;

} SymbolsList;


int search(SymbolsList *const, const unsigned char, SymbolsList **);

Text mtf(const Text);

SymbolsList *mvtElement(SymbolsList *const, SymbolsList *);

SymbolsList *initListOfSymbols(void);

void freeListOfSymbols(SymbolsList *);


#endif
