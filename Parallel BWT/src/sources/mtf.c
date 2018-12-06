#include "../headers/mtf.h"

#define SIZE_SYMBOLS_LIST 256

Text mtf(const Text input)
{
	//Variables
	SymbolsList *symbols;
	Text output;
	MtfAux mtfAux;

	output.len = input.len;
	output.text = (unsigned char *) malloc(sizeof(unsigned char)*input.len);
	symbols = initListOfSymbols();

	for(unsigned i=0; i<input.len; i++) {

		search(symbols, input.text[i], &mtfAux);
		output.text[i] = mtfAux.pos;

		symbols = mvtElement(symbols, &mtfAux);
	}

	freeListOfSymbols(symbols);
	free(input.text);

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
