#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./headers/bwt.h"
#include "./headers/mtf.h"
#include "./headers/suffixTree.h"
#include "./headers/zle.h"

#define MAX_LENGTH 2000

void printResult(short *mtfText, int size)
{
	int i;

	for(i=0; i<size; i++)
		printf("%d ", mtfText[i]);
	printf("\n");

}

int main(int argc, char *argv[])
{

	//Variables
	unsigned char text[MAX_LENGTH] = "abracadabraabracadabra";
	unsigned char bwtText[MAX_LENGTH];
//	unsigned char *zleText = NULL;

	short bwtInput[MAX_LENGTH];
	short *mtfOutput;
	short *mtfOutput2;
	int inputLen = strlen((String)text);
	int i, bwtLen;

	mtfOutput = (short *) malloc(sizeof(short)*(inputLen+2));
	mtfOutput2 = (short *) malloc(sizeof(short)*(inputLen+2));

	if(inputLen > 2000) {
		printf("Size too big.\n");
		return 0;
	}

	/************************************************************************
	 * INPUT
	 ************************************************************************/
	for(i=0; i<inputLen; i++)
		bwtInput[i] = text[i];

	printf("Input string:\n\t%s\n", text);

	printf("Input:\n\t");
	printResult(bwtInput, inputLen);

	/************************************************************************
	 * BWT TRANSFORMATION
	 ************************************************************************/
	ResultBwt *bwtResult = bwtTransformation(bwtInput, inputLen);

	bwtResult->text[inputLen + 1] = (unsigned char)bwtResult->index;
	bwtLen = inputLen + 2;

	//String BWT output
	for(i=0; i<bwtLen; i++)
		bwtText[i] = bwtResult->text[i];

	printf("BWT output string:\n\ttext: %s\tindex: %d\n", bwtText, bwtResult->index);

	//BWT output in decimal
	printf("BWT output:\n\t");
	printResult(bwtResult->text, bwtLen);

	/************************************************************************
	 * MTF TRANSFORMATION
	 ************************************************************************/
	mtfOutput = mtfEncoding(bwtResult->text, 1, bwtLen);
	mtfOutput2 = mtfEncoding(bwtResult->text, 2, bwtLen);

	printf("MTF output:\n\t");
	printResult(mtfOutput, bwtLen);
	printf("MTF-2:\n\t");
	printResult(mtfOutput2, bwtLen);

	/************************************************************************
	 * ZLE ENCODING
	 ************************************************************************/
////	zleText = zleEncoding(mtfText);
//
//	int zleLen = strlen((String)zleText);
//
//	//For decimal
//	printf("ZLE output text:\n\t");
//	for(i=0; i<zleLen; i++)
//		printf("%d ", zleText[i]);
//	printf("\n");
//
//	//For hexadecimal example
//	printf("ZLE output hex:\n\t");
//	for(i=0; i<zleLen; i++)
//		printf("%x ", zleText[i]);
//	printf("\n");


	/************************************************************************
	 * ARITHMETIC ENCODING
	 ************************************************************************/



	/************************************************************************
	 * FREE MEMORY
	 ************************************************************************/
	free(bwtResult->text);
	free(bwtResult);

	free(mtfOutput);
	free(mtfOutput2);

//	free(zleText);

	return 0;
}


