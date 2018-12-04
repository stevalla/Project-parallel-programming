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

		printf("Error opening the file.\n");
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

void writeFile(char *const filename, Text *const result)
{
	FILE *f = openFileWB(filename);

	for(unsigned i=0; i<result->len; i++)
		fputc(result->text[i], f);

	fclose(f);
}

int compareFiles(char *const file1, char *const file2, long size)
{
	FILE *f1 = openFileRB(file1);
	FILE *f2 = openFileRB(file2);
	unsigned char ch1, ch2;
	int result;

	while(size > 0) {
		fread(&ch1, sizeof(ch1), 1, f1);
		fread(&ch2, sizeof(ch2), 1, f2);
		printf("ch1=%d!=ch2=%d\n", ch1, ch2);

		if(ch1 != ch2) {
			printf("ch1=%d!=ch2=%d\n", ch1, ch2);
			result = 0;
			break;
		}
		size--;
		puts("ciao");
	}

	if(feof(f1) && feof(f2))
		result = 1;
	else
		result = 0;


	fclose(f1);
	fclose(f2);

	return result;
}
