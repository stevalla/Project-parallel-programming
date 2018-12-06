#ifndef BWT_H
#define BWT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extern/uthash.h"
#include "suffixTree.h"
#include "util.h"


Text bwtTransformation(const Text);

void createSuffixArray(Node *const, int *const,
					   int *const, unsigned *const);

Text getBWT(unsigned *const, Node *const, const size_t);

int sortNodesByFirstChar(HashChildren *const, HashChildren *const);

unsigned *initText(unsigned char *const, const size_t);


#endif
