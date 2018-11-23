#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "suffixTree.h"
#include "bwt.h"

#define MAX_LENGTH 2000

int main(int argc, char *argv[]) {

	char text[MAX_LENGTH] = "mississippi";

	String bwtText = bwtTransformation(text);

	printf("%s\n", bwtText);
	return 0;
}
