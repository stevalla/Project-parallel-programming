#include "../headers/zle.h"

#define LOG2E 1.44269504089

Text zleEncoding(const Text input)
{
	//Variables
	size_t len, runLen;
	unsigned char *output;
	Text result;

	//Initialization
	len = 0;
	runLen = 0;
	output = (unsigned char *) malloc(sizeof(unsigned char) * input.len*2);

	for(unsigned j=0; j<input.len; j++) {

		if(input.text[j] == 0) {							//Zeros run
			runLen++;
			continue;

		} else if(runLen != 0) {					//End of zeros run
			countZeroRun(runLen + 1, &output[0], &len);
			runLen = 0;								//Restart the zeros counter

		}

		//No run
		if(input.text[j] == 0xFE) {						//ch = 254
			output[len++] = 0xFF;
			output[len++] = 0x00;

		} else if(input.text[j] == 0xFF) {				//ch = 255
			output[len++] = 0xFF;
			output[len++] = 0x01;

		} else
			output[len++] = input.text[j] + 1;

	}

	if(runLen != 0)
		countZeroRun(runLen + 1, &output[0], &len);

	result.text = output;
	result.len = len;

	free(input.text);

	return result;
}

void countZeroRun(const double runLen,
				  unsigned char *const output,
				  size_t *const len)
{
	int k;
	unsigned *zeroRun;

	const int binarySize = (log(runLen) * LOG2E) + 1;

	zeroRun = decToBin(runLen, binarySize);

	for(k=0; k<binarySize-1; k++)
		output[(*len)++] = zeroRun[k];

	free(zeroRun);
}

unsigned *decToBin(size_t runLen, const size_t size)
{
	unsigned *binary = (unsigned *) malloc(sizeof(unsigned)*(size - 1));
	int index = 0;

	while(runLen > 1) {
		binary[index++] = (runLen % 2);
		runLen /= 2;
	}

	return binary;
}
