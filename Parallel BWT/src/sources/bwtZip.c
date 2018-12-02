#include "../headers/bwtZip.h"


void bwtZip(Text *const input)
{

	/***********************************************************************
	 * BWT TRANSFORMATION
	************************************************************************/
	Text *bwtOutput = bwtTransformation(input);

	printf("BWT output:\n\t");
	printResult(bwtOutput->text, bwtOutput->len);

	/***********************************************************************
	 * MTF TRANSFORMATION
	 ***********************************************************************/
	Text *mtfOutput = mtf(bwtOutput);

	printf("MTF output:\n\t");
	printResult(mtfOutput->text, mtfOutput->len);

	/***********************************************************************
	 * ZLE ENCODING
	 ***********************************************************************/
	Text *zleOutput = zleEncoding(mtfOutput);

	printf("ZLE output:\n\t");
	printResult(zleOutput->text, zleOutput->len);

	/**********************************************************************
	* ARITHMETIC ENCODING
	***********************************************************************/
	Text *compressed = encodingRoutine(zleOutput);

	printf("Arithmetic coding output:\n\t");
	printResult(compressed->text, compressed->len);

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

	/***********************************************************************
	 * MTF REVERSE
	 ***********************************************************************/
	Text *mtfReverse = unmtf(zleDecoded);

	printf("MTF reverse output:\n\t");
	printResult(mtfReverse->text, mtfReverse->len);

	/***********************************************************************
	 * BWT REVERSE
	************************************************************************/
	Text *bwtReverse = unbwt(mtfReverse);

	printf("BWT reverse output:\n\t");
	printResult(bwtReverse->text, bwtReverse->len);

	/************************************************************************
	 * FREE MEMORY
	 ************************************************************************/

	free(bwtOutput->text);
	free(bwtOutput);
	free(mtfOutput->text);
	free(mtfOutput);
	free(zleOutput->text);
	free(zleOutput);
	free(compressed->text);
	free(compressed);
	free(decompressed->text);
	free(decompressed);
	free(zleDecoded->text);
	free(zleDecoded);
	free(bwtReverse->text);
	free(bwtReverse);
}

void printResult(unsigned char *const text, const size_t size)
{
	int i;

	for(i=0; i<size; i++)
		printf("%d ", text[i]);
	printf("\n");

}
