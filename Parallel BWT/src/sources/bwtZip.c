#include "../headers/bwtZip.h"


Text bwtZip(const Text input)
{
	clock_t start, end;

	/***********************************************************************
	 * BWT TRANSFORMATION
	************************************************************************/
	start = clock();
	Text bwtOutput = bwtTransformation(input);
	end = clock();

	printf("\t-BWT finished                    TIME %f ms\n",
			(((double)end - (double)start) / CLOCKS_PER_SEC) * 1000);

	/***********************************************************************
	 * MTF TRANSFORMATION
	 ***********************************************************************/
	start = clock();
	Text mtfOutput = mtf(bwtOutput);
	end = clock();

	printf("\t-MTF finished                    TIME %f ms\n",
			(((double)end - (double)start) / CLOCKS_PER_SEC) * 1000);
	/***********************************************************************
	 * ZLE ENCODING
	 ***********************************************************************/
	start = clock();
	Text zleOutput = zleEncoding(mtfOutput);
	end = clock();

	printf("\t-ZLE finished                    TIME %f ms\n",
			(((double)end - (double)start) / CLOCKS_PER_SEC) * 1000);
	/**********************************************************************
	* ARITHMETIC ENCODING
	***********************************************************************/
	start = clock();
	Text compressed = encodingRoutine(zleOutput);
	end = clock();

	printf("\t-Arithmetic coding finished      TIME %f ms\n\n",
			(((double)end - (double)start) / CLOCKS_PER_SEC) * 1000);

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

		printf("Block size %ld BYTE\n", inZip.len);

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
