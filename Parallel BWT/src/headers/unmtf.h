#ifndef UNMTF_H
#define UNMTF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mtf.h"

Text *unmtf(Text *);

unsigned char searchSymbol(SymbolsList *const, const unsigned char,
						   MtfAux *);

#endif
