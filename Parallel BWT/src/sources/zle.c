#include "../headers/zle.h"

#define MAX_LENGTH 2000
#define LOG2E 1.44269504089

ZeroRun *zleEncoding(short *input, int lenText)
{
	//Variables
	int j, runLen, totalLen;
	short output[MAX_LENGTH];
	ZeroRun *result;

	//Initialization
	totalLen = 0;
	runLen = 0;
	j = 0;

	while(lenText > 0) {

		if(input[j] != 0 && runLen != 0) { 			//End of zeros run

			countZeroRun(runLen + 1, &output[0], &totalLen);

			output[totalLen++] = input[j] + 1;
			runLen = 0;								//Restart the zeros counter

		} else if(input[j] != 0)     				//No run
			output[totalLen++] = input[j] + 1;

		else										//Zeros run
			runLen++;

		j++;
		lenText--;
	}

	if(runLen != 0)
		countZeroRun(runLen + 1, &output[0], &totalLen);

	result = (ZeroRun *) malloc(sizeof(ZeroRun));
	result->encoded = output;
	result->len = totalLen;

	return result;
}

void countZeroRun(double runLen, short *output, int *totalLen)
{
	int k;
	short *zeroRun;

	int binarySize = (log(runLen) * LOG2E) + 1;

	zeroRun = decToBin(runLen, binarySize);

	for(k=0; k<binarySize-1; k++)
		output[(*totalLen)++] = zeroRun[k];

	free(zeroRun);
}

short *decToBin(int runLen, int size)
{
	short *binary = (short *) malloc(sizeof(short)*(size-1));
	int index = 0;

	while(runLen > 1) {
		binary[index++] = (runLen % 2);
		runLen /= 2;
	}

	return binary;
}
