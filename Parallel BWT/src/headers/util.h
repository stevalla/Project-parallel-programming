#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POW_2(N)       (1 << (N))
#define MAX_CHUNK_SIZE (5 * 1048576)


typedef struct Text
{
	unsigned char *text;
	size_t len;

} Text;


#endif
