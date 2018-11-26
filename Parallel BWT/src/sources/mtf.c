#include "../headers/mtf.h"

#define SIZE_SYMBOLS_LIST 255

short *mtfEncoding(short *inputText, int option, size_t inputLen)
{
	//Variables
	int i;
	ListOfSymbols *symbols;
	short *mtfOutput = (short *) malloc(sizeof(short)*inputLen);
	MtfAux *mtfAux = (MtfAux *) malloc(sizeof(MtfAux));

	symbols = initListOfSymbols();

	for(i=0; i<inputLen; i++) {

		//The sentinel character is not moved on
		if(inputText[i] > 255) {
			mtfOutput[i] = inputText[i];
			continue;
		}

		mtfAux = search(symbols, inputText[i], mtfAux);
		mtfOutput[i] = mtfAux->pos;

		if(option == 1)
			symbols = moveToFrontElement(symbols, mtfAux);
		else
			symbols = moveToFrontElement2(symbols, mtfAux);
	}

	free(mtfAux);
	freeListOfSymbols(symbols);

	return mtfOutput;
}

MtfAux *search(ListOfSymbols *symbols, int byte, MtfAux *mtfAux)
{
	ListOfSymbols *curr, *prev;
	int i=0;

	//If the char is at the head of the list
	if(symbols->symbol == byte) {
		mtfAux->pos = i;
		mtfAux->prev = NULL;
		return mtfAux;
	}

	//Otherwise
	i++;
	for(curr=symbols->next, prev=symbols; curr!=NULL; curr=curr->next, prev=prev->next) {
		if(curr->symbol == byte) {
			mtfAux->pos = i;
			mtfAux->prev = prev;
			break;
		}
		i++;
	}

	return mtfAux;
}

ListOfSymbols *moveToFrontElement(ListOfSymbols *symbols, MtfAux *mtfAux)
{

	ListOfSymbols *current;

	if(mtfAux->prev != NULL) {

		current = mtfAux->prev->next;

		//Delete element from the list of symbols
		mtfAux->prev->next = mtfAux->prev->next->next;

		//Re-insert it at the head of the list
		current->next = symbols;
	}

	return current;
}

/*
 * Optimize version, each symbol is move to position 0 only if
 * it is in position 1, otherwise it is moved to position 1.
 *
 * This aims to have more sequences of zeros.
 */
ListOfSymbols *moveToFrontElement2(ListOfSymbols *symbols, MtfAux *mtfAux)
{

	ListOfSymbols *current;

	if(mtfAux->prev != NULL && mtfAux->prev != symbols) {

		current = mtfAux->prev->next;

		//Delete element from the list of symbols
		mtfAux->prev->next = mtfAux->prev->next->next;

		//Re-insert it at the position 1 of the list
		current->next = symbols->next;
		symbols->next = current;

		return symbols;

	} else if(mtfAux->prev == symbols) {

		current = mtfAux->prev->next;
		symbols->next = current->next;
		current->next = symbols;

	}

	return current;
}

//Populate the list of symbols with characters from 0 to 255
ListOfSymbols *initListOfSymbols()
{
	ListOfSymbols *symbols, *tmp;
	int i=0;

	symbols = (ListOfSymbols *) malloc(sizeof(ListOfSymbols));

	symbols->symbol = i;
	tmp = symbols;

	for(i=1; i<SIZE_SYMBOLS_LIST; i++) {

		tmp->next = (ListOfSymbols *) malloc(sizeof(ListOfSymbols));

		tmp = tmp->next;
		tmp->symbol = i;
	}

	tmp->next = NULL;

	return symbols;
}

void freeListOfSymbols(ListOfSymbols *symbols)
{
	ListOfSymbols *tmp;

	while(symbols != NULL) {
		tmp = symbols->next;
		free(symbols);
		symbols = tmp;
	}
}
