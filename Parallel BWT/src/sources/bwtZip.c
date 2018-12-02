#include "../headers/bwtZip.h"


void bwtZip(unsigned char *const input, const size_t inputLen)
{

	/***********************************************************************
	 * BWT
	************************************************************************/
	unsigned char *bwtResult = bwtTransformation(input, inputLen);

	size_t bwtLen = inputLen + 9;

	printf("BWT output:\n\t");
	printResult(bwtResult, bwtLen);

	/***********************************************************************
	 * MTF TRANSFORMATION
	 ***********************************************************************/
	unsigned char *mtfOutput = mtf(bwtResult, 1, bwtLen);
	unsigned char *mtfOutput2 = mtf(bwtResult, 2, bwtLen);

	printf("MTF output:\n\t");
	printResult(mtfOutput, bwtLen);
	printf("MTF-2:\n\t");
	printResult(mtfOutput2, bwtLen);

	/***********************************************************************
	 * ZLE ENCODING
	 ***********************************************************************/

	Text *zleOutput = zleEncoding(mtfOutput, bwtLen);

	printf("ZLE output:\n\t");
	printResult(zleOutput->text, zleOutput->len);

	/**********************************************************************
	* ARITHMETIC ENCODING
	***********************************************************************/
	unsigned char *compressed =
			encodingRoutine(zleOutput->text, zleOutput->len);

	printf("Arithmetic coding output:\n\t");
	printResult(compressed, inputLen-2);


	/***********************************************************************
	 * ARITHMETIC DECODING
	 ***********************************************************************/
	Text *decompressed = decodingRoutine(compressed);

	printf("Arithmetic decoding output:\n\t");
	printResult(decompressed->text, decompressed->len);

	/***********************************************************************
	 * ZLE DECODING
	 ***********************************************************************/

	Text *zleDecoded = zleDecoding(decompressed);

	printf("ZLE decoding output:\n\t");
	printResult(zleDecoded->text, zleDecoded->len);

	/************************************************************************
	 * FREE MEMORY
	 ************************************************************************/

	free(bwtResult);
	free(mtfOutput);
	free(mtfOutput2);
	free(zleOutput);
	free(compressed);
	free(decompressed);
	free(zleDecoded);

}

void printResult(unsigned char *const text, const size_t size)
{
	int i;

	for(i=0; i<size; i++)
		printf("%d ", text[i]);
	printf("\n");

}
