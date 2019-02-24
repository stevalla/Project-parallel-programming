/******************************************************************************
 * Copyright (C) 2018 by Stefano Valladares                                   *
 *                                                                            *
 * This file is part of ParallelBWTzip.                                       *
 *                                                                            *
 *   ParallelBWTzip is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 3 of the       *
 *   License, or (at your option) any later version.                          *
 *                                                                            *
 *   ParallelBWTzip is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public         *
 *   License along with ParallelBWTzip. 									  *
 *   If not, see <http://www.gnu.org/licenses/>.     						  *
 ******************************************************************************/

/**
 * @file 	util.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief 	Source file implementing a util interface for encoding
 * 			bytes and managing files.
 */

#include "../headers/util.h"


unsigned char decomposeUnsigned(unsigned u, int n) {
    return (u >> (24 - 8 * n)) & 0xFF;
}


void encodeUnsigned(const unsigned u, unsigned char *const output, int outIndex)
{
	for(unsigned i=0; i<4; i++) {
		output[outIndex++] = decomposeUnsigned(u, i);
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


Text readFile(FILE *const file, long size)
{
	Text text;
	text.text = (unsigned char *) malloc(sizeof(unsigned char)*size);

	text.len = fread(&text.text[0], 1, size, file);

	if(text.len < 0) {

		printf("Error reading the file.\n");
		fclose(file);
		free(text.text);
		abort();
		return text;

	} else
		return text;
}


FILE *openFileWB(char *const filename)
{
	FILE *f = fopen(filename, "rb");
	char c;

	if(f != NULL) {
		printf("The file %s already exists, do "
			   "you want to overwrite it?[y/n]", filename);
		scanf("%c", &c);

		if(c == 'n') {
			abort();
			fclose(f);
			return NULL;
		}
		fclose(f);
	}

	f = fopen(filename, "wb");

	if(f != NULL)
		return f;
	else {
		printf("Error opening the file %s\n", filename);
		abort();
		fclose(f);
		return NULL;
	}
}


void writeFile(FILE 		 *const file,
			   unsigned char *const buffer,
			   long 				len)
{
	if(fwrite(&buffer[0], 1, len, file) != len || ferror(file)) {
		printf("Error writing the file.\n");
		fclose(file);
		free(buffer);
		abort();
	}
}


long fileSize(FILE *const file)
{
	long origin = ftell(file);
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, origin, SEEK_SET);

	return size;
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
			result = 0;
			break;
		}
	}

	free(f1);
	free(f2);

	return result;
}
