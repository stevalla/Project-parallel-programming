#ifndef BWT_ZIP_H
#define BWT_ZIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bwt.h"
#include "mtf.h"
#include "zle.h"
#include "arith.h"


Text bwtZip(const Text);

void printResult(unsigned char *const, const size_t);


#endif
