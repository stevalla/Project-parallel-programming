#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "./headers/bwt.h"
#include "./headers/mtf.h"
#include "./headers/suffixTree.h"
#include "./headers/zle.h"
#include "./headers/arith.h"

#define MAX_LENGTH 2000


void printResult(short *text, int size)
{
	int i;

	for(i=0; i<size; i++)
		printf("%d ", text[i]);
	printf("\n");

}

int main(int argc, char *argv[])
{

	//Variables
//	unsigned char text[MAX_LENGTH] = "abracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabraabracadabra";
//	unsigned char bwtText[MAX_LENGTH];
//
//	short bwtInput[MAX_LENGTH];
//	short *mtfOutput;
//	short *mtfOutput2;
//	ZeroRun *zleOutput;
//	int inputLen = strlen((String)text);
//	int i, bwtLen;
//
//	mtfOutput = (short *) malloc(sizeof(short)*(inputLen+2));
//	mtfOutput2 = (short *) malloc(sizeof(short)*(inputLen+2));
//
//	if(inputLen > 2000) {
//		printf("Size too big.\n");
//		return 0;
//	}


	/************************************************************************
	 * INPUT
	 ************************************************************************/
//	for(i=0; i<inputLen; i++)
//		bwtInput[i] = text[i];
//
//	printf("Input string:\n\t%s\n", text);
//
//	printf("Input:\n\t");
//	printResult(bwtInput, inputLen);


	/************************************************************************
	 * BWT TRANSFORMATION
	 ************************************************************************/
//	ResultBwt *bwtResult = bwtTransformation(bwtInput, inputLen);
//
//	bwtResult->text[inputLen + 1] = (unsigned char)bwtResult->index;
//	bwtLen = inputLen + 2;
//
//	for(i=0; i<bwtLen; i++)
//		bwtText[i] = bwtResult->text[i];
//
//	printf("BWT output string:\n\ttext: %s\tindex: %d\n", bwtText, bwtResult->index);
//
//	printf("BWT output:\n\t");
//	printResult(bwtResult->text, bwtLen);
//
//
//	/************************************************************************
//	 * MTF TRANSFORMATION
//	 ************************************************************************/
//	mtfOutput = mtfEncoding(bwtResult->text, 1, bwtLen);
//	mtfOutput2 = mtfEncoding(bwtResult->text, 2, bwtLen);
//
//	printf("MTF output:\n\t");
//	printResult(mtfOutput, bwtLen);
//	printf("MTF-2:\n\t");
//	printResult(mtfOutput2, bwtLen);
//
//
//	/************************************************************************
//	 * ZLE ENCODING
//	 ************************************************************************/
//	zleOutput = zleEncoding(mtfOutput, bwtLen);
//
//	printf("ZLE output:\n\t");
//	printResult(zleOutput->encoded, zleOutput->len);


	/************************************************************************
	 * ARITHMETIC ENCODING
	 ************************************************************************/

//	short a[] = {0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,4,4,45,4,4,5,5,5,5,5,65,6,6,6,6,6,6,5,4,3,2,2,1,1,1,256};

//	for(i=0; i<59; i++) {
//		printf("%d\t", a[i]);
//	}
//	printf("\n");
//
	FILE *input = fopen("input.txt", "rb");

	puts("Encoding...\n");
	encodingRoutine(input);
//
//	printf("\n");

//	printf("Arithmetic coding output:\n\t");

	FILE *f = fopen("encoded.zip", "rb");

	puts("Decoding...\n");
	decodingRoutine(f);


//	printResult(arithOutput, 30);

//	printf("%d", sizeof(unsigned));
	/************************************************************************
	 * FREE MEMORY
	 ************************************************************************/
//	free(bwtResult->text);
//	free(bwtResult);
//
//	free(mtfOutput);
//	free(mtfOutput2);
//
//	free(zleOutput);

	return 0;
}


