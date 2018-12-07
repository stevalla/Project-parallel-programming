#include "../headers/bwtZip.h"


Text bwtZip(const Text input)
{

	/***********************************************************************
	 * BWT TRANSFORMATION
	************************************************************************/
	Text bwtOutput = bwtTransformation(input);

	puts("\t-BWT finished");

	/***********************************************************************
	 * MTF TRANSFORMATION
	 ***********************************************************************/
	Text mtfOutput = mtf(bwtOutput);

	puts("\t-MTF finished");
	/***********************************************************************
	 * ZLE ENCODING
	 ***********************************************************************/
	Text zleOutput = zleEncoding(mtfOutput);

	puts("\t-ZLE finished");
	/**********************************************************************
	* ARITHMETIC ENCODING
	***********************************************************************/
	Text compressed = encodingRoutine(zleOutput);

	puts("\t-Arithmetic coding finished");

	return compressed;
}

void compress(FILE *input, FILE *output)
{
	while(1) {

		Text compressed, inZip;
		unsigned char id[1];
		unsigned char length[4];

		inZip = readFile(input, MAX_CHUNK_SIZE);

		if(inZip.len == 0) {
			free(inZip.text);
			break;
		}

		if(inZip.len < MIN_CHUNK_SIZE) {
			compressed = inZip;
			id[0] = 0;
			free(inZip.text);

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
