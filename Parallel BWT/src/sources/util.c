#include "../headers/util.h"

unsigned char decomposeUnsigned(unsigned ul, int n) {
    return (ul >> (24 - 8 * n)) & 0xFF;
}

void encodeIndex(const unsigned index, unsigned char *const output, int j)
{
	for(unsigned i=0; i<4; i++) {
		output[j++] = decomposeUnsigned(index, i);
	}
}

unsigned readUL(unsigned char *const input, size_t n)
{
	unsigned ret = 0;

	for(unsigned i=0; i<4; i++) {
		unsigned t = input[n + i];
		ret |= t << (24 - 8 * i);
	}

	return ret;
}

long fileSize(FILE *f)
{
	long origin = ftell(f);
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, origin, SEEK_SET);

	return size;
}

unsigned char *readFile(FILE *f, long size)
{
	unsigned char *input = (unsigned char *)
							malloc(sizeof(unsigned char)*size);

	if(fread(&input[0], 1, size, f) != size) {

		printf("Error opening the file.\n");
		return NULL;

	} else {

		fclose(f);
		return input;
	}
}

FILE *openFileRB(char *const filename)
{
	FILE *f = fopen(filename, "rb");

	if(f != NULL)
		return f;
	else {
		printf("Error opening the file %s\n", filename);
		abort();
		return NULL;
	}
}

FILE *openFileWB(char *const filename)
{
//	FILE *f = fopen(filename, "rb");
//	char c;
//
//	if(f != NULL) {
//		printf("The file %s already exists, do "
//			   "you want to overwrite it?[y/n]", filename);
//		scanf("%c", &c);
//
//		if(c == 'n') {
//			abort();
//			fclose(f);
//			return NULL;
//		}
//		fclose(f);
//	}

	FILE *f = fopen(filename, "wb");

	if(f != NULL)
		return f;
	else {
		printf("Error opening the file %s\n", filename);
		abort();
		return NULL;
	}
}

void writeFile(FILE *out, Text *const result)
{
	for(unsigned i=0; i<result->len; i++)
		fputc(result->text[i], out);
}
