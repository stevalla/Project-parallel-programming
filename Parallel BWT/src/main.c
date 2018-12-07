#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "./headers/bwtZip.h"
#include "./headers/bwtUnzip.h"
#include "./headers/util.h"


int main(int argc, char *argv[])
{

	/************************************************************************
	 * ZIP
	 ************************************************************************/
	char *const inputFile = "input.png";
	char *const encodedFile = "encoded.zip";

	FILE *inputE = openFileRB(inputFile);
	FILE *outputE = openFileWB(encodedFile);

	puts("------------------ZIP------------------");

	compress(inputE, outputE);

	puts("Finish zip.\n");
	printf("Size original: %d comrpessed %d saved %d\n", fileSize(inputE),
					fileSize(outputE), fileSize(inputE) - fileSize(outputE));

	fclose(inputE);
	fclose(outputE);

	/************************************************************************
	 * UNZIP
	 ************************************************************************/
	char *const inputDecFile = "encoded.zip";
	char *const decodedFile = "decoded.png";

	FILE *inputD = openFileRB(inputDecFile);
	FILE *outputD = openFileWB(decodedFile);

	puts("------------------UNZIP------------------");

	decompress(inputD, outputD);

	puts("Finish unzip phase.\n");

	fclose(inputD);
	fclose(outputD);

	/************************************************************************
	 * COMPARING
	 ************************************************************************/

	puts("------------------COMPARING------------------");

	FILE *original = openFileRB(inputFile);
	FILE *decompress = openFileRB(decodedFile);

	int result = compareFiles(original, decompress,
				 fileSize(original), fileSize(decompress));

	if(result == 1)
		printf("Compression loseless\n");
	else if(result == 0)
		printf("Loss some data during compression.\n");
	else
		printf("Added some data during compression.\n");

	fclose(original);
	fclose(decompress);

	return 0;
}



