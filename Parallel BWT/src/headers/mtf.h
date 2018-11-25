#ifndef MTF_H
#define MTF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef char* String;

//Linked list containing the table of symbols
typedef struct ListOfSymbols
{
	int symbol;
	struct ListOfSymbols *next;

} ListOfSymbols;

/*
 * This structure keeps track of the position
 * of the current element in the table of symbols
 * and of the previous element of it in the table
 * of symbols. This make easier perform the move
 * to front operation on the linked list of symbols.
 */
typedef struct MtfAux
{
	int pos;
	ListOfSymbols *prev;

} MtfAux;


MtfAux *search(ListOfSymbols *, int, MtfAux *);

short *mtfEncoding(short *, int, size_t);

ListOfSymbols *moveToFrontElement(ListOfSymbols *, MtfAux *);

ListOfSymbols *moveToFrontElement2(ListOfSymbols *, MtfAux *);

ListOfSymbols *initListOfSymbols();

void freeListOfSymbols(ListOfSymbols *);


#endif
