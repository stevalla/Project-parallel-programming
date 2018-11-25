#include "../headers/zle.h"

#define LOG2E 1.44269504089

Ascii zleEncoding(Ascii text, int lenText)
{
	//Variables
	int i, j, runLen;
	Ascii output = (Ascii) malloc(lenText);

	//Initialization
	runLen = 0;
	i = 0;
	j = 0;

	while(lenText != 0) {

		if(text[j] != '0' && runLen != 0) { 		//End of zeros run

			output[i++] = encodeZeroRun(runLen);
			output[i++] = text[j] + 1;
			runLen = 0;								//Restart the zeros counter

		} else if(text[j] != '0')   				//No run
				output[i++] = text[j] + 1;
		else										//Zeros run
			runLen++;
		j++;
		lenText--;
	}

	return output;
}

int encodeZeroRun(int len)
{
//	int binarySize = (log(len) * LOG2E) + 1;
//	char binary[binarySize];
//	int index = 0;
//
//	while(len > 1) {
//
//		binary[index++] = (len % 2);
//		len /= 2;
//	}
//
//	return 0;



}
