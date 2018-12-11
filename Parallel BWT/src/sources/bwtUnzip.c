#include "../headers/bwtUnzip.h"


Text bwtUnzip(const Text input)
{
	/***********************************************************************
	 * ARITHMETIC DECODING
	 ***********************************************************************/
	Text decompressed = decodingRoutine(input);

	puts("\t-Arithmetic decoding finished");
	/***********************************************************************
	 * ZLE DECODING
	 ***********************************************************************/
	Text zleDecoded = zleDecoding(decompressed);

	puts("\t-ZLE decoding finished");
	/***********************************************************************
	 * MTF REVERSE
	 ***********************************************************************/
	Text mtfReverse = unmtf(zleDecoded);

	puts("\t-MTF reverse finished");
	/***********************************************************************
	 * BWT REVERSE
	 ***********************************************************************/
	Text bwtReverse = unbwt(mtfReverse);

	puts("\t-BWT reverse finished\n");

	return bwtReverse;
}

void decompress(FILE *input, FILE *output)
{
	while(1) {

		Text inUnzip, decompressed, length;

		length = readFile(input, 4);

		if(length.len == 0) {
			free(length.text);
			break;
		}

		inUnzip.len = readUnsigned(length.text, 0);
		inUnzip.text = readFile(input, inUnzip.len).text;

		decompressed = bwtUnzip(inUnzip);

		writeFile(output, decompressed.text, decompressed.len);

		free(decompressed.text);
		free(length.text);
	}
}
