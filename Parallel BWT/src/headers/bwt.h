#ifndef BWT_H
#define BWT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extern/uthash.h"
#include "suffixTree.h"


//Hold the bwt text and the index of the original string in the bwt
typedef struct ResultBwt
{
	short *text;
	int index;

} ResultBwt;


ResultBwt *bwtTransformation(short *, size_t);

void createSuffixArray(Node *, int *, int[], short *);

ResultBwt *getBWT(short *, Node *, size_t);

int sortNodesByFirstChar(HashChildren *, HashChildren *);


#endif
