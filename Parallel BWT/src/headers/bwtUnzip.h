#ifndef BWT_UNZIP_H
#define BWT_UNZIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parallelBwtZip.h"
#include "unbwt.h"
#include "unmtf.h"
#include "unzle.h"
#include "unarith.h"

Text bwtUnzip(const Text);

void decompress(FILE *, FILE *);


#endif
