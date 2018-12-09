#ifndef BWT_ZIP_H
#define BWT_ZIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "bwt.h"
#include "mtf.h"
#include "zle.h"
#include "arith.h"

typedef struct ResultList
{
	Text result;
	ResultList *next;

} ResultList;


void compress(FILE *, FILE *);

void writeFile(FILE *);

void insertInOrderResult(Text);


#endif
