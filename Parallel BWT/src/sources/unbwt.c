#include "../headers/unbwt.h"

Text unbwt(Text input)
{
	Text output;
	unsigned primary, sentinel;
	unsigned *links = (unsigned *) malloc(sizeof(unsigned) * input.len);
	unsigned freqs[257], mapping[257];

	primary  = readUnsigned(input.text, 0);
	sentinel = readUnsigned(input.text, 4);

	output.text = (unsigned char *) malloc(sizeof(unsigned char)*input.len);
	output.len = input.len - 9;

	input.text += 8;
	input.len -= 8;

	//Init freqs vector
	for(unsigned i=0; i<257; i++)
		freqs[i] = 0;

	//Compute K
	for(unsigned i=0; i<input.len; i++) {
		if(i == sentinel)
			freqs[256]++;
		else
			freqs[input.text[i]]++;
	}

	//Compute M
	mapping[0] = 0;
	for(unsigned i=1; i<257; i++)
		mapping[i] = mapping[i - 1] + freqs[i - 1];

	//Compute L
	for(unsigned i=0; i<input.len; i++) {
		if(i == sentinel)
			links[i] = mapping[256]++;
		else
			links[i] = mapping[input.text[i]]++;
	}

	primary = links[primary];
	for(int i=input.len-2; i>=0; i--) {
		primary = links[primary];
		output.text[i] = input.text[primary];
	}

	free(input.text-8);
	free(links);

	return output;
}
