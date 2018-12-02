#ifndef BWT_H
#define BWT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extern/uthash.h"
#include "suffixTree.h"
#include "util.h"


Text *bwtTransformation(Text *const);

void createSuffixArray(Node *const, int *const,
					   short *const, short *const);

Text *getBWT(short *const, Node *const, const size_t);

int sortNodesByFirstChar(HashChildren *const, HashChildren *const);

short *initText(unsigned char *const, const size_t);


#endif
