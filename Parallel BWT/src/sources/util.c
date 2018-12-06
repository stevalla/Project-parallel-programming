#include "../headers/util.h"

unsigned char decomposeUnsigned(unsigned u, int n) {
    return (u >> (24 - 8 * n)) & 0xFF;
}

void encodeIndex(const unsigned index, unsigned char *const output, int j)
{
	for(unsigned i=0; i<4; i++) {
		output[j++] = decomposeUnsigned(index, i);
	}
}

unsigned readUnsigned(unsigned char *const input, size_t n)
{
	unsigned ret = 0;

	for(unsigned i=0; i<4; i++) {
		unsigned t = input[n + i];
		ret |= t << (24 - 8 * i);
	}

	return ret;
}

long fileSize(char *const filename)
{
	FILE *f = openFileRB(filename);

	long origin = ftell(f);
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, origin, SEEK_SET);

	fclose(f);

	return size;
}

unsigned char *readFile(char *const filename, long size)
{
	FILE *f = openFileRB(filename);
	unsigned char *input = (unsigned char *)
							malloc(sizeof(unsigned char)*size);

	if(fread(&input[0], 1, size, f) != size) {

		printf("Error reading the file %s.\n", filename);
		fclose(f);
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

void writeFile(char *const filename, const Text result)
{
	FILE *f = openFileWB(filename);

	if(fwrite(&result.text[0], 1, result.len, f) != result.len ||
			  ferror(f)) {
		printf("Error writing the file %s.\n", filename);
		fclose(f);

	} else
		fclose(f);
}

int compareFiles(char *const file1, char *const file2, long size1, long size2)
{
	if(size1 > size2) {
		printf("The two files have different sizes %d %d.\n",size1, size2);
	} else if(size1 < size2)
		return 2;

	unsigned char *f1 = readFile(file1, size1);
	unsigned char *f2 = readFile(file2, size2);
	int result = 1;
	int i = 0;
	printf("size1 %d size2 %d\n", size1, size2);
	while(i++ < size1) {
		if(f1[i - 1] != f2[i - 1]) {
			printf("ch1=%d!=ch2=%d i = %d\n", f1[i-1], f2[i-1], i-1);
			result = 0;
			break;
		}
	}

	free(f1);
	free(f2);

	return result;
}
