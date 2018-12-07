#include "../headers/util.h"

unsigned char decomposeUnsigned(unsigned u, int n) {
    return (u >> (24 - 8 * n)) & 0xFF;
}

void encodeUnsigned(const unsigned index, unsigned char *const output, int j)
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

long fileSize(FILE *const f)
{
	long origin = ftell(f);
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, origin, SEEK_SET);

	return size;
}

Text readFile(FILE *const f, long size)
{
	Text text;
	text.text = (unsigned char *) malloc(sizeof(unsigned char)*size);

	text.len = fread(&text.text[0], 1, size, f);

	if(text.len < 0) {

		printf("Error reading the file.\n");
		fclose(f);
		free(text.text);
		abort();
		return text;

	} else
		return text;
}

FILE *openFileRB(char *const filename)
{
	FILE *f = fopen(filename, "rb");

	if(f != NULL)
		return f;
	else {
		printf("Error opening the file %s\n", filename);
		abort();
		fclose(f);
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
		fclose(f);
		return NULL;
	}
}

void writeFile(FILE *const f,
			   unsigned char *const result,
			   long len)
{
	if(fwrite(&result[0], 1, len, f) != len || ferror(f)) {
		printf("Error writing the file.\n");
		fclose(f);
		free(result);
		abort();
	}
}

int compareFiles(FILE *const file1, FILE *const file2, long size1, long size2)
{
	if(size1 > size2) {
		printf("The two files have different sizes %ld %ld.\n",size1, size2);
	} else if(size1 < size2)
		return 2;

	unsigned char *f1 = readFile(file1, size1).text;
	unsigned char *f2 = readFile(file2, size2).text;
	int result = 1;
	int i = 0;
	while(i++ < size1) {
		if(f1[i - 1] != f2[i - 1]) {
//			printf("ch1=%d!=ch2=%d i = %d\n", f1[i-1], f2[i-1], i-1);
			result = 0;
			break;
		}
	}

	free(f1);
	free(f2);

	return result;
}
