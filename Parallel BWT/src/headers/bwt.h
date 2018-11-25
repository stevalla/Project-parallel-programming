#ifndef BWT_H
#define BWT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extern/uthash.h"
#include "suffixTree.h"


typedef unsigned char* Ascii;

//Hold the bwt text and the index of the original string in the bwt
typedef struct ResultBwt
{
	unsigned char *text;
	int index;

} ResultBwt;


ResultBwt *bwtTransformation(Ascii);

void createSuffixArray(Node *, int *, int[], Ascii);

ResultBwt *getBWT(Ascii, Node *);

int sortNodesByFirstChar(HashChildren *, HashChildren *);


#endif
