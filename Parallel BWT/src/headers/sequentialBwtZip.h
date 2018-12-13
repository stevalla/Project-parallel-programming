#ifndef SEQUENTIAL_BWT_ZIP_H
#define SEQUENTIAL_BWT_ZIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "bwt.h"
#include "mtf.h"
#include "zle.h"
#include "arith.h"


Text bwtZip(const Text);

void compress(FILE *, FILE *, long);

#endif
