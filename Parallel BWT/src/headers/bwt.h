#ifndef BWT_H
#define BWT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extern/uthash.h"
#include "suffixTree.h"


unsigned char *bwtTransformation(unsigned char *const, const size_t);

void createSuffixArray(Node *const, int *const,
					   short *const, short *const);

unsigned char *getBWT(short *const, Node *const, const size_t);

int sortNodesByFirstChar(HashChildren *const, HashChildren *const);

short *initText(unsigned char *const, const size_t);

// Return a vector<unsigned char> that represents u big-endian.
void encodeIndex(const unsigned, unsigned char *const, int);

// Return the nth big-endian byte of an unsigned int. 0 <= n <= 3
unsigned char decomposeUnsigned(unsigned ,int);

#endif
