#include "../headers/mtf.h"

#define SIZE_SYMBOLS_LIST 256

Text mtf(const Text input)
{
	//Variables
	SymbolsList *symbols, *aux;
	Text output;
	int tmp;

	output.len = input.len;
	output.text = (unsigned char *) malloc(sizeof(unsigned char)*input.len);
	symbols = initListOfSymbols();

	for(unsigned i=0; i<input.len; i++) {

		tmp = search(symbols, input.text[i], &aux);

		if(tmp != -1)
			output.text[i] = (unsigned char) tmp;
		else {
			fprintf(stderr, "Error during MTF phase.\n");
			exit(-1);
		}

		symbols = mvtElement(symbols, aux);
	}

	freeListOfSymbols(symbols);
	free(input.text);

	output.id = input.id;

	return output;
}

int search(SymbolsList *const symbols,
		   const unsigned char byte,
		   SymbolsList **aux)
{
	SymbolsList *curr, *prev;
	int i;

	//If the char is at the head of the list
	if(symbols->symbol == byte) {
		*aux = NULL;
		return 0;
	}

	//Otherwise
	i = 1;
	prev=symbols;
	for(curr=symbols->next; curr!=NULL; curr=curr->next) {
		if(curr->symbol == byte) {
			*aux = prev;
			return i;
		}
		i++;
		prev=prev->next;
	}

	return -1;
}

SymbolsList *mvtElement(SymbolsList *const symbols,
						SymbolsList *el)
{

	SymbolsList *current;

	if(el != NULL) {

		current = el->next;

		//Delete element from the list of symbols
		el->next = el->next->next;

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

void freeListOfSymbols(SymbolsList *symbols)
{
	SymbolsList *tmp;

	while(symbols != NULL) {
		tmp = symbols->next;
		free(symbols);
		symbols = tmp;
	}
}
