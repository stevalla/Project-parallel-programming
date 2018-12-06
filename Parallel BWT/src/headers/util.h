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

// Return the nth big-endian byte of an unsigned int. 0 <= n <= 3
unsigned char decomposeUnsigned(unsigned, int);

// Return a vector<unsigned char> that represents u big-endian.
void encodeIndex(const unsigned, unsigned char *const, int);

// Return the unsigned int encoded big-endian by
// the 4 bytes starting at location n.
unsigned readUnsigned(unsigned char *const, size_t);

long fileSize(char *const);

unsigned char *readFile(char *const, long);

void writeFile(char *const, const Text);

FILE *openFileRB(char *const);

FILE *openFileWB(char *const);

int compareFiles(char *const, char *const, long, long);

#endif
