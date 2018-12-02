#ifndef BWT_ZIP_H
#define BWT_ZIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/bwt.h"
#include "../headers/unbwt.h"
#include "../headers/mtf.h"
#include "../headers/unmtf.h"
#include "../headers/zle.h"
#include "../headers/unzle.h"
#include "../headers/arith.h"
#include "../headers/unarith.h"


void bwtZip(Text *const);

void printResult(unsigned char *const, const size_t);


#endif
