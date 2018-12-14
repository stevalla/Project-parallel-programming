
#include "./headers/bwtUnzip.h"
#include "./headers/util.h"
#include "headers/parallelBwtZip.h"
#include "headers/sequentialBwtZip.h"

void usage()
{
	puts("Usage:\nExample:\n\t./bwt infile-name outfile-name c/d [--compare]"
		 "[-c chunk_size(MB)] [-m parallel/sequential]\n");
	puts("-\tType c to compress / d to decompress");
	puts("-\tOption '--compare' is to compare two files, "
		 "only the two files must be specified.");
	puts("-\tMax chunk size 5 MB. Write '-c ' before the desired chunk size.");
	puts("-\tDefault mode: parallel. Write '-m ' before the desired running mode.");
	puts("-\tChunk size and mode options are only for compression.\n");

}

int setChunkSize(char *const chunkSize)
{
	long size = atof(chunkSize) * 1024 * 1024;

	if(size > MAX_CHUNK_SIZE) {
		usage();
		return 0;
	}

	return size;
}

char setMode(char *const mode)
{
	if(!strcmp(mode, "parallel"))
		return 'p';

	else if(!strcmp(mode, "sequential"))
		return 's';

	else {
		usage();
		return 0;
	}
}

void zipMain(char *const input,
			 char *const output,
			 const char mode,
			 const long chunkSize)
{
	extern Buffer readin, bwt, arith;
	struct timespec start, end;

	long a[5] = {102400, 9*102400, 18*102400, 36*102400, 50*102400};
	double time[40];

	int k = 0;

	for(int j=0; j<5; j++) {
		printf("Chunk size: %ld\n", a[j]);
		for(int i=0; i<10; i++) {

			if(k++ == 49) j = -1;

			FILE *inputE = openFileRB(input);
			FILE *outputE = openFileWB(output);

			//Calculate the wall time
			if(mode == 'p' && k < 49) {
				clock_gettime(CLOCK_MONOTONIC, &start);
				compressParallel(inputE, outputE, a[j]);
				clock_gettime(CLOCK_MONOTONIC, &end);

			} else {
				clock_gettime(CLOCK_MONOTONIC, &start);
				compressSequential(inputE, outputE, a[j]);
				clock_gettime(CLOCK_MONOTONIC, &end);
			}

			free(readin.queue);
			free(bwt.queue);
			free(arith.queue);

			if(i==0) {
				printf("Mode %c\n", mode);
				printf("Number of blocks: %d\n",
						(int)ceil((float)fileSize(inputE) / (float)chunkSize));
				printf("Input file %s\n", input);
				printf("Size original: %ld comrpessed: %ld deflated: %f %% \n",
				fileSize(inputE),
				fileSize(outputE),
				(1 - (double)(fileSize(outputE) / (double)fileSize(inputE))) * 100);
			}

			time[i] = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;

//			printf("Time %f\n", time[i]);
			fclose(inputE);
			fclose(outputE);

		}
		double sum = 0;
		for(int i=0; i<10; i++) {
			sum += time[i];
		}

		printf("Average time for compression %f sec\n\n", sum/10);
	}
}

void unzipMain(char *const input,
			   char *const output)
{
	FILE *inputD = openFileRB(input);
	FILE *outputD = openFileWB(output);

	decompress(inputD, outputD);

	puts("Finished decompression.");
	fclose(inputD);
	fclose(outputD);
}

void compareMain(char *const input, char *const output)
{
	FILE *original = openFileRB(input);
	FILE *decompress = openFileRB(output);

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
}

int main(int argc, char *argv[])
{

	if(argc < 4 && argc != 6 && argc != 8) {
		usage();
		printf("%d\n", argc);
		return 0;
	}

	long chunkSize;
	char mode;

	if(argc > 4) {

		if(!strcmp(argv[4], "-c") && argc == 6) {

			mode = 'p';
			if((chunkSize = setChunkSize(argv[5])) == 0)
				return 0;

		} else if(!strcmp(argv[4], "-m") && argc == 6) {

			if((mode = setMode(argv[5])) == 0)
				return 0;

			printf("%d	\n", mode);
			chunkSize = DEFAULT_CHUNK_SIZE;

		} else if(!strcmp(argv[4], "-c") && !strcmp(argv[6], "-m")) {

			if((chunkSize = setChunkSize(argv[5])) == 0 ||
			   (mode = setMode(argv[7])) == 0)
				return 0;

		} else {
			usage();
			return 0;
		}

	} else if(!strcmp(argv[3], "--compare")){
		compareMain(argv[1], argv[2]);
		return 0;

	} else {
		chunkSize = DEFAULT_CHUNK_SIZE;
		mode = 'p';
	}

	if(!strcmp(argv[3], "c"))
		zipMain(argv[1], argv[2], mode, chunkSize);

	else if(!strcmp(argv[3], "d"))
		unzipMain(argv[1], argv[2]);

	else {
		usage();
		return 0;
	}

	return 0;
}



