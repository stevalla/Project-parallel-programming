#include "../headers/util.h"

unsigned char decomposeUnsigned(unsigned ul, int n) {
    return (ul >> (24 - 8 * n)) & 0xFF;
}

void encodeIndex(const unsigned index, unsigned char *const output, int j)
{
	for(unsigned i=0; i<4; i++) {
		output[j++] = decomposeUnsigned(index, i);
	}
}

unsigned readUL(unsigned char *const input, size_t n)
{
	unsigned ret = 0;

	for(unsigned i=0; i<4; i++) {
		unsigned t = input[n + i];
		ret |= t << (24 - 8 * i);
	}

	return ret;
}
