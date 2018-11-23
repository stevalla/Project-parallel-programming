#ifndef BWT_H
#define BWT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uthash.h"
#include "suffixTree.h"


String bwtTransformation(String);

void createSuffixArray(Node *, int *, int[], String);

String getBWT(String, Node *);

int sortNodesByFirstLetter(HashChildren *, HashChildren *);


#endif
