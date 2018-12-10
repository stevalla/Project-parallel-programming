
#include "./headers/bwtZip.h"
#include "./headers/bwtUnzip.h"
#include "./headers/util.h"


int main(int argc, char *argv[])
{

	/************************************************************************
	 * ZIP
	 ************************************************************************/
	char *const inputFile = "Examples/input.jpg";
	char *const encodedFile = "Examples/encoded.bwt";

	clock_t start, end;

	FILE *inputE = openFileRB(inputFile);
	FILE *outputE = openFileWB(encodedFile);

	puts("------------------ZIP------------------\n");

	start = clock();
	compress(inputE, outputE);
	end = clock();

	puts("Finish zip\n");
	printf("Size original: %ld comrpessed: %ld saved: %ld\n", fileSize(inputE),
					fileSize(outputE), fileSize(inputE) - fileSize(outputE));

	printf("Time for compression: %f ms\n\n",
			(((double)end - (double)start) / CLOCKS_PER_SEC) * 1000);

	fclose(inputE);
	fclose(outputE);

	/************************************************************************
	 * UNZIP
	 ************************************************************************/
	char *const inputDecFile = "Examples/encoded.bwt";
	char *const decodedFile  = "Examples/decoded.jpg";

	FILE *inputD = openFileRB(inputDecFile);
	FILE *outputD = openFileWB(decodedFile);

	puts("------------------UNZIP------------------\n");

	decompress(inputD, outputD);

	puts("Finish unzip phase\n");

	fclose(inputD);
	fclose(outputD);

	/************************************************************************
	 * COMPARING
	 ************************************************************************/

	puts("------------------COMPARING------------------\n");

	FILE *original = openFileRB(inputFile);
	FILE *decompress = openFileRB(decodedFile);

	int result = compareFiles(original, decompress,
				 fileSize(original), fileSize(decompress));

	if(result == 1)
		printf("Compression loseless\n");
	else if(result == 0)
		printf("Loss some data during compression.\n");
	else
		printf("Added some data during compression.\n");

	fclose(original);
	fclose(decompress);

	return 0;
}



