#include "../headers/bwtUnzip.h"


void bwtUnzip(Text *const input)
{

	/***********************************************************************
	 * ARITHMETIC DECODING
	 ***********************************************************************/
	Text *decompressed = decodingRoutine(input);

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
	 ***********************************************************************/
	Text *bwtReverse = unbwt(mtfReverse);

	printf("BWT reverse output:\n\t");
	printResult(bwtReverse->text, bwtReverse->len);

	/***********************************************************************
	 * OUTPUT
	 ***********************************************************************/
	printf("Input string:\n\t%s\n", bwtReverse->text);

	free(bwtReverse->text);
	free(bwtReverse);
}
