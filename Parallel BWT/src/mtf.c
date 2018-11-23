#include "mtf.h"

#define SIZE_SYMBOLS_LIST 256

int *mtfEncoding(String text)
{
	int i;
	int len = strlen(text);
	ListOfSymbols *symbols;
	int *mtfText;
	MtfAux *mtfAux;

	mtfText = (int *) malloc(sizeof(int)*strlen(text));
	if(!mtfText) {
		fprintf(stderr, "error during allocation of mtfText!");
		return 0;
	}

	symbols = initListOfSymbols();
	mtfAux = (MtfAux *) malloc(sizeof(MtfAux));
	if(!mtfAux) {
		fprintf(stderr, "error during allocation of mtfAux!");
		return 0;
	}

	mtfAux = search(symbols, 'a', mtfAux);

//	printf("pos %d, prev pos%d\n", mtfAux->pos, mtfAux->prev->symbol);
	for(i=0; i<len; i++) {
		mtfAux = search(symbols, text[i], mtfAux);
		mtfText[i] = mtfAux->pos;
		symbols = moveToFrontElement(symbols, mtfAux);
	}

	free(mtfAux);
	freeListOfSymbols(symbols);

	return mtfText;
}

void printList(ListOfSymbols *list)
{
	ListOfSymbols *curr;
	for(curr = list; curr != NULL; curr = curr->next) {
		printf("%d\t", curr->symbol);
		if(curr->symbol%25 == 0)
			printf("\n");
	}
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

	//The element is already at the head of the list
	if(mtfAux->prev == NULL)
		return symbols;

	//Delete element from the list of symbols
	current = mtfAux->prev->next;
	mtfAux->prev->next = mtfAux->prev->next->next;

	//re-insert it at the head of the list
	current->next = symbols;

	return current;
}

ListOfSymbols *initListOfSymbols()
{
	ListOfSymbols *symbols, *tmp;
	int i=0;

	symbols = (ListOfSymbols *) malloc(sizeof(ListOfSymbols));
	if(!symbols) {
		fprintf(stderr, "error during allocation of the head of the list!");
		return 0;
	}

	symbols->symbol = i;
	tmp = symbols;

	for(i=1; i<SIZE_SYMBOLS_LIST; i++) {

		tmp->next = (ListOfSymbols *) malloc(sizeof(ListOfSymbols));
		if(!tmp->next) {
			fprintf(stderr, "error during allocation of a node of the list!");
			return 0;
		}

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
