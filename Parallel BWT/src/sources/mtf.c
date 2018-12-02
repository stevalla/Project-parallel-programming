#include "../headers/mtf.h"

#define SIZE_SYMBOLS_LIST 256

unsigned char *mtf(unsigned char *const input,
			  	   const int option,
				   const size_t inputLen)
{
	//Variables
	int i;
	SymbolsList *symbols;
	unsigned char *output = (unsigned char *)
							 malloc(sizeof(unsigned char)*inputLen);
	MtfAux mtfAux;

	symbols = initListOfSymbols();

	for(i=0; i<inputLen; i++) {

		search(symbols, input[i], &mtfAux);
		output[i] = mtfAux.pos;

		if(option == 1)
			symbols = mvtElement(symbols, &mtfAux);
		else
			symbols = mvtElement2(symbols, &mtfAux);
	}

	freeListOfSymbols(symbols);

	return output;
}

void search(SymbolsList *const symbols,
			const unsigned char byte,
			MtfAux *const mtfAux)
{
	SymbolsList *curr, *prev;
	int i;

	//If the char is at the head of the list
	if(symbols->symbol == byte) {
		mtfAux->pos = 0;
		mtfAux->prev = NULL;
		return;
	}

	//Otherwise
	i = 1;
	prev=symbols;
	for(curr=symbols->next; curr!=NULL; curr=curr->next) {
		if(curr->symbol == byte) {
			mtfAux->pos = i;
			mtfAux->prev = prev;
			break;
		}
		i++;
		prev=prev->next;
	}
}

SymbolsList *mvtElement(SymbolsList *const symbols,
						MtfAux *const mtfAux)
{

	SymbolsList *current;

	if(mtfAux->prev != NULL) {

		current = mtfAux->prev->next;

		//Delete element from the list of symbols
		mtfAux->prev->next = mtfAux->prev->next->next;

		//Re-insert it at the head of the list
		current->next = symbols;

		return current;
	}

	return symbols;
}

/*
 * Optimize version, each symbol is move to position 0 only if
 * it is in position 1, otherwise it is moved to position 1.
 *
 * This aims to have more sequences of zeros.
 */
SymbolsList *mvtElement2(SymbolsList *const symbols,
						 MtfAux *const mtfAux)
{

	SymbolsList *current;

	if(mtfAux->prev != NULL && mtfAux->prev != symbols) {

		current = mtfAux->prev->next;

		//Delete element from the list of symbols
		mtfAux->prev->next = mtfAux->prev->next->next;

		//Re-insert it at the position 1 of the list
		current->next = symbols->next;
		symbols->next = current;


	} else if(mtfAux->prev == symbols) {

		current = mtfAux->prev->next;
		symbols->next = current->next;
		current->next = symbols;

		return current;
	}

	return symbols;
}

//Populate the list of symbols with characters from 0 to 255
SymbolsList *initListOfSymbols()
{
	SymbolsList *symbols, *tmp;
	int i=0;

	symbols = (SymbolsList *) malloc(sizeof(SymbolsList));

	symbols->symbol = (unsigned char)i;
	tmp = symbols;

	for(i=1; i<SIZE_SYMBOLS_LIST; i++) {

		tmp->next = (SymbolsList *) malloc(sizeof(SymbolsList));

		tmp = tmp->next;
		tmp->symbol = (unsigned char)i;
	}

	tmp->next = NULL;

	return symbols;
}

void printList(SymbolsList *const symbols)
{
	SymbolsList *tmp = symbols;

	while(tmp != NULL) {
		printf("%d\n", tmp->symbol);
		tmp = tmp->next;
	}
}

void freeListOfSymbols(SymbolsList *symbols)
{
	SymbolsList *tmp;

	while(symbols != NULL) {
		tmp = symbols->next;
		free(symbols);
		symbols = tmp;
	}
}
