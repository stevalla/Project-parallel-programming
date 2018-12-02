#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "./headers/bwtZip.h"

int main(int argc, char *argv[])
{

	//Variables
	unsigned char text[] = "abracadabraabracadabra";
	int inputLen = strlen((char *)text);

	if(inputLen > 2000) {
		printf("Size too big.\n");
		return 0;
	}

	/************************************************************************
	 * INPUT
	 ************************************************************************/

	printf("Input string:\n\t%s\n", text);

	printf("Input:\n\t");
	printResult(text, inputLen);


	/************************************************************************
	 * ZIP
	 ************************************************************************/

	bwtZip(text, inputLen);

	return 0;
}



