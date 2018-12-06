#include "../headers/bwtZip.h"


Text bwtZip(const Text input)
{
	printf("Input:\n\t");
//	printResult(input.text, input.len);
	printf("input len %d\n", input.len);

	/***********************************************************************
	 * BWT TRANSFORMATION
	************************************************************************/
	Text bwtOutput = bwtTransformation(input);

	printf("BWT output:\n\t");
//	printResult(bwtOutput.text, bwtOutput.len);
	printf("Bwt len %d\n", bwtOutput.len);

	/***********************************************************************
	 * MTF TRANSFORMATION
	 ***********************************************************************/
	Text mtfOutput = mtf(bwtOutput);

	printf("MTF output:\n\t");
//	printResult(mtfOutput.text, mtfOutput.len);
	printf("MTf len %d\n", mtfOutput.len);

	/***********************************************************************
	 * ZLE ENCODING
	 ***********************************************************************/
	Text zleOutput = zleEncoding(mtfOutput);

	printf("ZLE output:\n\t");
//	printResult(zleOutput.text, zleOutput.len);
	printf("ZLE len %d\n", zleOutput.len);

	/**********************************************************************
	* ARITHMETIC ENCODING
	***********************************************************************/
	Text compressed = encodingRoutine(zleOutput);

	printf("Arithmetic coding output:\n\t");
//	printResult(compressed.text, compressed.len);
	printf("Arith len %d\n", compressed.len);

	return compressed;
}

void printResult(unsigned char *const text, const size_t size)
{
	int i;

	for(i=0; i<size; i++)
		printf("%d ", text[i]);
	printf("\n");
}
