#include "../headers/bwtZip.h"


long fileSize(FILE *f)
{
	long origin = ftell(f);
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, origin, SEEK_SET);

	return size;
}

unsigned char *readFile(FILE *f, long size)
{
	unsigned char *input = (unsigned char *)
							malloc(sizeof(unsigned char)*size);

	if(fread(&input[0], 1, size, f) != size) {

		printf("Error opening the file.\n");
		return NULL;

	} else {

		fclose(f);
		return input;
	}
}

void bwtZip(unsigned char *const input, const size_t inputLen)
{
//	unsigned char *vector = readFile(input, fileSize(input));

	/************************************************************************
	 * BWT
	************************************************************************/
	unsigned char *bwtResult = bwtTransformation(input, inputLen);

	size_t bwtLen = inputLen + 9;

	printf("BWT output:\n\t");
	printResult(bwtResult, bwtLen);

	/************************************************************************
	 * MTF TRANSFORMATION
	 ************************************************************************/
	unsigned char *mtfOutput = mtf(bwtResult, 1, bwtLen);
	unsigned char *mtfOutput2 = mtf(bwtResult, 2, bwtLen);

	printf("MTF output:\n\t");
	printResult(mtfOutput, bwtLen);
	printf("MTF-2:\n\t");
	printResult(mtfOutput2, bwtLen);

	/************************************************************************
	 * ZLE ENCODING
	 ************************************************************************/

	ZeroRun *zleOutput = zleEncoding(mtfOutput, bwtLen);

	printf("ZLE output:\n\t");
	printResult(zleOutput->encoded, zleOutput->len);

	/************************************************************************
	* ARITHMETIC ENCODING
	************************************************************************/
	unsigned char *compressed =
			encodingRoutine(zleOutput->encoded, zleOutput->len);

	printf("Arithmetic coding output:\n\t");
	printResult(compressed, inputLen-2);


	/************************************************************************
	 * ARITHMETIC DECODING
	 ************************************************************************/
	unsigned char *decompressed = decodingRoutine(compressed);

	printf("Arithmetic decoding output:\n\t");
	printResult(decompressed, zleOutput->len);


	/************************************************************************
	 * FREE MEMORY
	 ************************************************************************/

	free(bwtResult);
	free(mtfOutput);
	free(mtfOutput2);
	free(zleOutput);
	free(compressed);
	free(decompressed);

}

void printResult(unsigned char *const text, const size_t size)
{
	int i;

	for(i=0; i<size; i++)
		printf("%d ", text[i]);
	printf("\n");

}
