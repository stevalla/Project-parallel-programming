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
	String text;
	int index;

} ResultBwt;


ResultBwt *bwtTransformation(String);

void createSuffixArray(Node *, int *, int[], String);

ResultBwt *getBWT(String, Node *);

int sortNodesByFirstLetter(HashChildren *, HashChildren *);


#endif
