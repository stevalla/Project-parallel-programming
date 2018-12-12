
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
	double time[40];

	struct timespec start, end;
	long a[5] = {102400, 9*102400, 18*102400, 36*102400, 50*102400};

	for(int j=0; j<5; j++) {
		printf("Chunk size: %ld\n", a[3]);
	for(int i=0; i<1; i++) {
		FILE *inputE = openFileRB(inputFile);
		FILE *outputE = openFileWB(encodedFile);


		//Calculate the wall time
		clock_gettime(CLOCK_MONOTONIC, &start);
		compress(inputE, outputE, a[j]);
		clock_gettime(CLOCK_MONOTONIC, &end);

		free(readin.queue);
		free(bwt.queue);
		free(arith.queue);

		if(i==0)
			printf("Size original: %ld comrpessed: %ld deflated: %f \% \n",
					fileSize(inputE),
					fileSize(outputE),
					(1 - (double)(fileSize(outputE) / (double)fileSize(inputE))) * 100);

		time[i] = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;

		printf("Time %f\n", time[i]);
		fclose(inputE);
		fclose(outputE);
	}

	double sum = 0;
	for(int i=0; i<1; i++) {
		sum += time[i];
	}

	printf("Average time for compression %f sec\n\n", sum/1);
	}
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



