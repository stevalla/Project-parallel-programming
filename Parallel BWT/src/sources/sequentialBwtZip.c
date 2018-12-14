#include "../headers/sequentialBwtZip.h"


Text bwtZip(const Text input)
{
	/***********************************************************************
	 * BWT TRANSFORMATION
	************************************************************************/
	Text bwtOutput = bwtTransformation(input);

	/***********************************************************************
	 * MTF TRANSFORMATION
	 ***********************************************************************/
	Text mtfOutput = mtf(bwtOutput);

	/***********************************************************************
	 * ZLE ENCODING
	 ***********************************************************************/
	Text zleOutput = zleEncoding(mtfOutput);

	/**********************************************************************
	* ARITHMETIC ENCODING
	***********************************************************************/
	Text compressed = encodingRoutine(zleOutput);

	return compressed;
}

void compressSequential(FILE *input, FILE *output, long chunkSize)
{
	while(1) {

		Text compressed, inZip;
		unsigned char length[4];
		unsigned char id[1];

		inZip = readFile(input, chunkSize);

		if(inZip.len == 0) {
			free(inZip.text);
			break;
		}

		if(inZip.len < MIN_CHUNK_SIZE) {
			compressed = inZip;
			id[0] = 0;

		} else {
			compressed = bwtZip(inZip);
			id[0] = 1;
		}

		encodeUnsigned(compressed.len, length, 0);

		writeFile(output, length, 4);
		writeFile(output, id, 1);
		writeFile(output, compressed.text, compressed.len);

		free(compressed.text);
	}

}
