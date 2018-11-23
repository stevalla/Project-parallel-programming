#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "suffixTree.h"
#include "bwt.h"
#include "mtf.h"

#define MAX_LENGTH 2000

int main(int argc, char *argv[]) {

	char text[MAX_LENGTH] = "zzzzzzyyyyyxxxxwwwvvu";

	//BWT transformation
//	String bwtText = bwtTransformation(text);

	//Move to front coding
	int *mtfText = mtfEncoding(text);
	int i;
	for(i=0; i<strlen(text); i++)
		printf("%d\n", mtfText[i]);
	return 0;
}
