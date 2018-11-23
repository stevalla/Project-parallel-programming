#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./headers/bwt.h"
#include "./headers/mtf.h"
#include "./headers/suffixTree.h"

#define MAX_LENGTH 2000

int main(int argc, char *argv[]) {

	char text[MAX_LENGTH] = "mississippi";
	if(strlen(text) > 2000) {
		printf("Size too big.\n");
		return 0;
	}

	//BWT transformation
	ResultBwt *bwtResult = bwtTransformation(text);

	printf("BWT output:\n\ttext: %s\tindex: %d\n", bwtResult->text, bwtResult->index);

	//Move to front coding
	unsigned char *mtfText = mtfEncoding(bwtResult->text, bwtResult->index);
//	free(bwtResult->text);		/* TODO: error, but i should deallocate memory */


	int len = strlen(bwtResult->text);
	free(bwtResult);

	int i;
	printf("MTF text:\n\t");
	for(i=0; i<len; i++)
		printf("%d", mtfText[i]);

	free(mtfText);

	return 0;
}
