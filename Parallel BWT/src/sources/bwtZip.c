#include "../headers/bwtZip.h"


Text *bwtZip(Text *const input)
{

	/***********************************************************************
	 * BWT TRANSFORMATION
	************************************************************************/
	Text *bwtOutput = bwtTransformation(input);

//	printf("BWT output:\n\t");
//	printResult(bwtOutput->text, bwtOutput->len);

	/***********************************************************************
	 * MTF TRANSFORMATION
	 ***********************************************************************/
	Text *mtfOutput = mtf(bwtOutput);

//	printf("MTF output:\n\t");
//	printResult(mtfOutput->text, mtfOutput->len);

	/***********************************************************************
	 * ZLE ENCODING
	 ***********************************************************************/
	Text *zleOutput = zleEncoding(mtfOutput);

//	printf("ZLE output:\n\t");
//	printResult(zleOutput->text, zleOutput->len);

	/**********************************************************************
	* ARITHMETIC ENCODING
	***********************************************************************/
	Text *compressed = encodingRoutine(zleOutput);

//	printf("Arithmetic coding output:\n\t");
//	printResult(compressed->text, compressed->len);

	return compressed;
}

void printResult(unsigned char *const text, const size_t size)
{
	int i;

	for(i=0; i<size; i++)
		printf("%d ", text[i]);
	printf("\n");

}
