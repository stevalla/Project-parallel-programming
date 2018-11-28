#include "../headers/arith.h"

#define MAX_VALUE_CODE 0xFFFFFFFF
#define END_OF_FILE	   257
#define FIRST_QRT	   (MAX_VALUE_CODE/4 + 1)
#define THIRD_QRT	   (3 * FIRST_QRT)
#define ONE_HALF	   (2 * FIRST_QRT)


short *encodingRoutine(short *input, size_t inputLen)
{
	Encoder *encoder;
	Interval currentInt;
	short model[258];	//The model
	short *output;
	int i, j;

	output = (short *) malloc(sizeof(short)*inputLen);
	encoder = (Encoder *) malloc(sizeof(Encoder));
	j = 0;

	initEncoder(encoder);
	initModel(model);

	for(i=0; input[i] != END_OF_FILE; i++) {

		findInterval(&currentInt, inputLen, model, input[j]);

		encodeSymbol(encoder, currentInt);

		checkForOutput(encoder, output, &j);

		updateModel(model);
	}

	finishEncoding(encoder, output, &j);

	free(encoder);

	return output;
}


void encodeSymbol(Encoder *encoder, Interval interval)
{
	long range;

	range = (long)(encoder->high - encoder->low) + 1;
	encoder->high = encoder->low + (unsigned int)
				       ((range * interval.highCount) / interval.scale - 1);
	encoder->low  = encoder->low + (unsigned int)
				       ((range * interval.lowCount) / interval.scale);
}


void initModel(short *model)
{

}


void initEncoder(Encoder *encoder)
{

	encoder->low = 0;
	encoder->high = MAX_VALUE_CODE;
	encoder->underflow = 0;

}


void findInterval(Interval *interval, size_t len, short *model, short currentChar)
{
	interval->scale = model[len + 1];
	interval->lowCount = model[currentChar];
	interval->highCount = model[currentChar + 1];
}


void checkForOutput(Encoder * encoder, short *output, int *index)
{
	while(1) {

		if(encoder->high < ONE_HALF)
			outputBit(0, output, &encoder->underflow, index);

		else if(encoder->low >= ONE_HALF) {

			outputBit(1, output, &encoder->underflow, index);
			encoder->low -= ONE_HALF;
			encoder->high -= ONE_HALF;

		} else if(encoder->low >= FIRST_QRT && encoder->high < THIRD_QRT) {

			encoder->underflow++;
			encoder->low -= FIRST_QRT;
			encoder->high -= FIRST_QRT;

		} else
			break;

		encoder->low = encoder->low * 2;
		encoder->high = (encoder->high * 2) + 1;
	}
}


void updateModel(short *model)
{

}


void outputBit(unsigned short bit, short *output, unsigned long *underflow, int *ind)
{
	output[(*ind)++] = bit;
	while(*underflow > 0) {

		output[(*ind)++] = !bit;
		(*underflow)--;
	}
}


void finishEncoding(Encoder *encoder, short *output, int *ind)
{
	encoder->underflow++;

	if(encoder->low < FIRST_QRT)
		outputBit(0, output, &encoder->underflow, ind);
	else
		outputBit(1, output, &encoder->underflow, ind);
}
