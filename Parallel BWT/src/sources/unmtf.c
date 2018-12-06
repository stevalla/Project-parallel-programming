#include "../headers/unmtf.h"


Text unmtf(const Text input)
{
	SymbolsList *symbols;
	Text output;
	MtfAux mtfAux;

	output.len = input.len;
	output.text = (unsigned char *) malloc(sizeof(unsigned char)*input.len);

	symbols = initListOfSymbols();

	for(unsigned i=0; i<input.len; i++) {

		output.text[i] = searchSymbol(symbols, input.text[i], &mtfAux);

		if(input.text[i] != 0)
			symbols = mvtElement(symbols, &mtfAux);

	}

	freeListOfSymbols(symbols);
	free(input.text);

	return output;
}

unsigned char searchSymbol(SymbolsList *const symbols,
				  	  	   const unsigned char pos,
						   MtfAux *mtfAux)
{
	SymbolsList *curr, *prev;
	int i;

	//Position 0
	if(pos == 0)
		return symbols->symbol;

	//Otherwise
	i = 1;
	prev=symbols;
	for(curr=symbols->next; curr!=NULL; curr=curr->next) {

		if(i == pos) {
			mtfAux->prev = prev;
			break;
		}

		i++;
		prev=prev->next;
	}

	return curr->symbol;
}
