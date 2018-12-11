
#include "./headers/bwtZip.h"
#include "./headers/bwtUnzip.h"
#include "./headers/util.h"


int main(int argc, char *argv[])
{

	/************************************************************************
	 * ZIP
	 ************************************************************************/
	char *const inputFile = "Examples/Large_corpus/bible.txt";
	char *const encodedFile = "Examples/Large_corpus/encoded.bwt";

	printf("Input file %s\n", inputFile);
	long time[40];

	long start, end;
	struct timeval timecheck;
	long a[5] = {102400, 9*102400, 18*102400, 36*102400, 50*102400};
	int i=0, j=0;
//	for(int j=0; j<1; j++) {
//		printf("Chunk size: %ld\n", a[j]);
//	for(int i=0; i<1; i++) {
		FILE *inputE = openFileRB(inputFile);
		FILE *outputE = openFileWB(encodedFile);

//		puts("------------------ZIP------------------\n");

		//Calculate the wall time
		gettimeofday(&timecheck, NULL);
		start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
		compress(inputE, outputE, a[j]);
		gettimeofday(&timecheck, NULL);
		end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;

//		puts("Finish zip\n");
		if(i==0)
			printf("Size original: %ld comrpessed: %ld deflated: %f \% \n",
					fileSize(inputE),
					fileSize(outputE),
					(1 - (double)(fileSize(outputE) / (double)fileSize(inputE))) * 100);

		time[i] = end - start;

//		printf("Time %ld\n", time[i]);
		fclose(inputE);
		fclose(outputE);
//	}

	long sum = 0;
	for(int i=0; i<1; i++) {
		sum += time[i];
	}

	printf("Average time for compression %f ms\n", (double)sum/1);
//	}
	/************************************************************************
	 * UNZIP
	 ************************************************************************/
	char *const inputDecFile = "Examples/Large_corpus/encoded.bwt";
	char *const decodedFile  = "Examples/decoded.txt";

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



