
#include "./headers/bwtUnzip.h"
#include "./headers/util.h"
#include "headers/parallelBwtZip.h"
#include "headers/sequentialBwtZip.h"

void usage()
{
	puts("USAGE:\n\t./PARALBWTZIP [options]\n");
	puts("OPTIONS:\n");
	puts("\t1 input-file-name        \tpath to the file.");
	puts("\t2 output-file-name       \tpath to the file.");
	puts("\t3 c|d|compare            \tc for compression, d for decompression\n"
		 "                                      \tand compare for bitwise comparison of \n"
		 "                                      \tthe two files.");
	puts("\t4 -c chunkSize          \tuse this option to input the size of \n"
		 "                                      \teach chunk, it must be expressed in \n"
		 "                                      \tmegabytes. Max size is 5MB and default\n"
		 "                                      \tis 0.9MB.");
	puts("\t5 -m sequential|parallel\tuse this option to specify the the running \n"
		 "                                      \tmode. Default is parallel.\n");
	puts("OTHERS INFO:\n");
	puts("\t-Chunk size and mode options are only for compression and they are the \n"
		 "\tonly options not required to run properly the program.\n"
		 "\t-The options must be specified in the defined order.");
	puts("------------------------------------------------------------------------------------");

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

	long a[5] = {0.1 * 1024 * 1024,
				 0.9 * 1024 * 1024,
				 1.8 * 1024 * 1024,
				 3.6 * 1024 * 1024,
				 5.0 * 1024 * 1024};
	double time[40];

//	puts("#BWTZIP\n");
//	printf("Input file %s\n", input);

	for(int k=0; k<1; k++)
	for(int j=0; j<1; j++) {
		printf("Chunk size: %ld\n", a[j]);
		for(int i=0; i<1; i++) {

			FILE *inputE = openFileRB(input);
			FILE *outputE = openFileWB(output);

//			printf("Number of blocks: %d\n",
//				   (int)ceil((float)fileSize(inputE) / (float)chunkSize));

			//Calculate the wall time
			if(mode == 'p' && k == 0) {
				if(i == 0) puts("Mode p");
				clock_gettime(CLOCK_MONOTONIC, &start);
				compressParallel(inputE, outputE, a[j]);
				clock_gettime(CLOCK_MONOTONIC, &end);

				free(readin.queue);
				free(bwt.queue);
				free(arith.queue);

			} else {
				if(i == 0) puts("Mode s");
				clock_gettime(CLOCK_MONOTONIC, &start);
				compressSequential(inputE, outputE, a[j]);
				clock_gettime(CLOCK_MONOTONIC, &end);
			}

			if(i==0) {
				printf("Number of blocks: %d\n",
						(int)ceil((float)fileSize(inputE) / (float)a[j]));
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
	puts("------------------------------------------------------------------------------------");
}

void unzipMain(char *const input,
			   char *const output)
{
	FILE *inputD = openFileRB(input);
	FILE *outputD = openFileWB(output);

	puts("#BWTUNZIP\n");

	decompress(inputD, outputD);

	puts("Finished decompression.\n\n");
	fclose(inputD);
	fclose(outputD);

	puts("------------------------------------------------------------------------------------");
}

void compareMain(char *const input, char *const output)
{
	FILE *original = openFileRB(input);
	FILE *decompress = openFileRB(output);

	puts("#BWTCOMPARE\n");

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
	puts("");
	puts("-------------------------------WELCOME TO PARALBWTZIP-------------------------------");
	puts("");

	if(argc < 4 && argc != 6 && argc != 8) {
		usage();
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

			chunkSize = DEFAULT_CHUNK_SIZE;

		} else if(!strcmp(argv[4], "-c") && !strcmp(argv[6], "-m")) {

			if((chunkSize = setChunkSize(argv[5])) == 0 ||
			   (mode = setMode(argv[7])) == 0)
				return 0;

		} else {
			usage();
			return 0;
		}

	} else {
		chunkSize = DEFAULT_CHUNK_SIZE;
		mode = 'p';
	}

	if(!strcmp(argv[3], "c"))
		zipMain(argv[1], argv[2], mode, chunkSize);

	else if(!strcmp(argv[3], "d"))
		unzipMain(argv[1], argv[2]);

	else if(!strcmp(argv[3], "compare"))
		compareMain(argv[1], argv[2]);

	else
		usage();

	return 0;
}



