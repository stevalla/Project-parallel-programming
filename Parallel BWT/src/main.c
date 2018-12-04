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

	Text *inputt = (Text *) malloc(sizeof(Text));
	inputt->text = text;
	inputt->len = inputLen;

	/************************************************************************
	 * ZIP
	 ************************************************************************/
	char *const inputFile = "input.txt";
	char *const encodedFile = "encoded.zip";

	puts("ZIP");

	Text *inZip = (Text *) malloc(sizeof(Text));
	inZip->len = fileSize(inputFile);
	inZip->text = readFile(inputFile, inZip->len);

	inZip->len--;

	Text *compressed = bwtZip(inZip);

	puts("Finish zip.");

	writeFile(encodedFile, compressed);

	free(compressed->text);
	free(compressed);

	/************************************************************************
	 * UNZIP
	 ************************************************************************/
	char *const inputDecFile = "encoded.zip";
	char *const decodedFile = "decoded.txt";

	puts("UNZIP");

	Text *inUnzip = (Text *) malloc(sizeof(Text));
	inUnzip->len = fileSize(inputDecFile);
	inUnzip->text = readFile(inputDecFile, inUnzip->len);

	Text *decompressed = bwtUnzip(inUnzip);

	puts("Finish unzip phase.");

	writeFile(decodedFile, decompressed);

	free(decompressed->text);
	free(decompressed);

	puts("Comparing..\n");

	printf("%d",
		compareFiles(inputFile, decodedFile, fileSize(inputFile)));

	return 0;
}



