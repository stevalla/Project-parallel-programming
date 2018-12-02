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
//	unsigned char text[] = "abracadabraabracadabra";
//	int inputLen = strlen((char *)text);
//
//	if(inputLen > 2000) {
//		printf("Size too big.\n");
//		return 0;
//	}
//	/************************************************************************
//	 * INPUT
//	 ************************************************************************/
//
//	printf("Input string:\n\t%s\n", text);
//
//	printf("Input:\n\t");
//	printResult(text, inputLen);


	//Variables
	FILE *in, *out;

	/************************************************************************
	 * ZIP
	 ************************************************************************/
	puts("ZIP");

	in = openFileRB("input.png");

	Text *inZip = (Text *) malloc(sizeof(Text));
	inZip->len = fileSize(in);
	inZip->text = readFile(in, inZip->len);

	fclose(in);

	Text *compressed = bwtZip(inZip);

	puts("Finish zip.");

	out = openFileWB("encoded.zip");

	writeFile(out, compressed);

	fclose(out);
	free(compressed->text);
	free(compressed);
	free(inZip->text);
	free(inZip);

	/************************************************************************
	 * UNZIP
	 ************************************************************************/
	puts("UNZIP");

	in = openFileRB("encoded.zip");

	Text *inUnzip = (Text *) malloc(sizeof(Text));
	inUnzip->len = fileSize(in);
	inUnzip->text = readFile(in, inUnzip->len);

	fclose(in);

	Text *decompressed = bwtUnzip(inUnzip);

	puts("Finish unzip phase.");

	out = openFileWB("decoded.png");

	writeFile(out, decompressed);

	fclose(out);
	free(decompressed->text);
	free(decompressed);
	free(inUnzip->text);
	free(inUnzip);

	puts("Closing..");

	return 0;
}



