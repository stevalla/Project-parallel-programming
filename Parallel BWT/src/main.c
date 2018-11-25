#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./headers/bwt.h"
#include "./headers/mtf.h"
#include "./headers/suffixTree.h"

#define MAX_LENGTH 2000

//Converting from ascii to hex
unsigned int *phex(unsigned char* string, long len)
{
    int i;

    //Allocate memory for array
    unsigned int * hex = (unsigned int *)malloc(sizeof(unsigned int) * len);
    for (i = 0; i < len; ++i) {
        //no special conversion needed
        hex[i] = string[i];
    }

    //Return array with hexadecimal representation for each character in string
    return hex;
}

void printMtfResult(unsigned char *mtfText, int size)
{
	int i;

	//For decimal
	printf("MTF text:\n\t");
	for(i=0; i<size+1; i++)
		printf("%d ", mtfText[i]);
	printf("\n");

	//For hexadecimal example
	printf("MTF output hex:\n\t");
	for(i=0; i<size+1; i++)
		printf("%x ", mtfText[i]);
	printf("\n");

}

int main(int argc, char *argv[])
{

	int i;
	unsigned char text[MAX_LENGTH] = "abracadabraabracadabra";
	unsigned char hexText[MAX_LENGTH];
	unsigned char *mtfText = NULL;
	unsigned char *mtfText2 = NULL;
	unsigned *hex;
	int len = strlen((String)text);

	if(len > 2000) {
		printf("Size too big.\n");
		return 0;
	}

	//For decimal
	printf("Input string:\n\t%s\n", text);

	//For hexadecimal example
	hex = phex(text, len);
	printf("Input hex:\n\t");
	for(i=0; i<len; i++) {
		hexText[i] = hex[i];
		printf("%x ", hexText[i]);
	}
	printf("\n");

	//BWT transformation
	ResultBwt *bwtResult = bwtTransformation(text);
	int bwtLen = strlen((String)bwtResult->text);

	bwtResult->text[bwtLen] = (unsigned char)bwtResult->index;

	//For decimal
	printf("BWT output:\n\ttext: %s\tindex: %d\n", bwtResult->text, bwtResult->index);

	//For hexadecimal example
	printf("BWT output hex:\n\t");
	for(i=0; i<bwtLen; i++)
		printf("%x ", bwtResult->text[i]);
	printf("\n");

	//Move to front coding
	mtfText = mtfEncoding(bwtResult->text, bwtResult->index, 1);
	mtfText2 = mtfEncoding(bwtResult->text, bwtResult->index, 2);

	free(bwtResult->text);
	free(bwtResult);

	printMtfResult(mtfText, bwtLen);
	printMtfResult(mtfText2, bwtLen);

	free(mtfText);
	free(mtfText2);

	return 0;
}


