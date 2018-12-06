#include "../headers/bwtUnzip.h"


Text bwtUnzip(const Text input)
{

	printf("Input decoding:\n\t");
//	printResult(input.text, input.len);
	printf("input len %d\n", input.len);
	/***********************************************************************
	 * ARITHMETIC DECODING
	 ***********************************************************************/
	Text decompressed = decodingRoutine(input);

	printf("Arithmetic decoding output:\n\t");
//	printResult(decompressed.text, decompressed.len);
	printf("decoded len %d\n", decompressed.len);
	/***********************************************************************
	 * ZLE DECODING
	 ***********************************************************************/
	Text zleDecoded = zleDecoding(decompressed);

	printf("ZLE decoding output:\n\t");
//	printResult(zleDecoded.text, zleDecoded.len);
	printf("zle len %d\n", zleDecoded.len);

	/***********************************************************************
	 * MTF REVERSE
	 ***********************************************************************/
	Text mtfReverse = unmtf(zleDecoded);

	printf("MTF reverse output:\n\t");
//	printResult(mtfReverse.text, mtfReverse.len);
	printf("mtf len %d\n", mtfReverse.len);
	/***********************************************************************
	 * BWT REVERSE
	 ***********************************************************************/
	Text bwtReverse = unbwt(mtfReverse);

	printf("BWT reverse output:\n\t");
//	printResult(bwtReverse.text, bwtReverse.len);
	printf("bwt len %d\n", bwtReverse.len);
	/***********************************************************************
	 * OUTPUT
	 ***********************************************************************/
	printf("Output decoded:\n\t");
//	printResult(bwtReverse.text, bwtReverse.len);

	return bwtReverse;
}
