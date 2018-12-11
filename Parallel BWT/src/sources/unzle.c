#include "../headers/unzle.h"

Text zleDecoding(const Text input)
{
	int j;
	size_t len;
	Text output;
	unsigned char *out = (unsigned char *)
						  malloc(sizeof(unsigned char) *(MAX_CHUNK_SIZE + 9));
	unsigned char *runLen = (unsigned char *)
							malloc(sizeof(unsigned char) * MAX_CHUNK_SIZE);

	len = 0;
	j = 0;
	runLen[j] = 255;

	for(unsigned i=0; i<input.len; i++) {

		if(input.text[i] <= 1) {				//Count zeros run
			runLen[j++] = input.text[i];
			runLen[j] = 255;
			continue;

		} else if(runLen[0] <= 1) {				//End of zeros run

			convertBinToDec(runLen, out, &len);
			runLen[0] = 255;					//Restart the zeros run
			j = 0;
		}

		if(input.text[i] == 0xFF) {
			const unsigned nextByte = input.text[++i];

			if(nextByte == 0x00)
				out[len++] = 0xFE;

			else if(nextByte == 0x01)
				out[len++] = 0xFF;

		} else
			out[len++] = input.text[i] - 1;

	}

	if(runLen[0] <= 1)
		convertBinToDec(runLen, out, &len);

	output.len = len;
	output.text = (unsigned char *) malloc(sizeof(unsigned char) * len);

	for(unsigned i=0; i<len; i++)
		output.text[i] = out[i];

	free(input.text);
	free(runLen);
	free(out);

	return output;
}

void convertBinToDec(unsigned char *runLen,
				     unsigned char *out,
					 size_t *index)
{
	int dec = 0;
	unsigned i;

	for(i=0; runLen[i]<=1; i++)
		dec += (POW_2(i) * runLen[i]);

	dec += POW_2(i);

	for(i=0; i<dec-1; i++)
		out[(*index)++] = 0;
}
