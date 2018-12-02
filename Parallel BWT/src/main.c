#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "./headers/bwtZip.h"

long fileSize(FILE *f)
{
	long origin = ftell(f);
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, origin, SEEK_SET);

	return size;
}

unsigned char *readFile(FILE *f, long size)
{
	unsigned char *input = (unsigned char *)
							malloc(sizeof(unsigned char)*size);

	if(fread(&input[0], 1, size, f) != size) {

		printf("Error opening the file.\n");
		return NULL;

	} else {

		fclose(f);
		return input;
	}
}

int main(int argc, char *argv[])
{

//	unsigned char *vector = readFile(input, fileSize(input));

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



