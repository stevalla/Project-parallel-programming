#include "../headers/bwtZip.h"

Queue *read, *bwt;
ResultList *result;

void compress(FILE *input, FILE *output)
{
	int i = 0, flag = 1;
	clock_t start, end;

	initQueue(read);
	initQueue(bwt);
	result = NULL;

	Text inZip = readFile(input, MAX_CHUNK_SIZE);

	#pragma omp parallel
	#pragma omp master
	while(flag) {

		/*********************************************************************
		 * INPUT
		 *********************************************************************/

		inZip = readFile(input, MAX_CHUNK_SIZE);

		if(inZip.len == 0)
			flag = 0;

		inZip.id = i++;

		printf("Block id %d size %ld BYTE\n", inZip.id, inZip.len);

		enqueue(inZip, read);

		/*********************************************************************
		 * BWT TRANSFORMATION
		 *********************************************************************/
		while(empty(read)) continue;

		start = clock();
		enqueue(bwtTransformation(dequeue(read)), bwt);
		end = clock();

		printf("\t-BWT finished                    TIME %f ms\n",
				(((double)end - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

		/*********************************************************************
		 * MTF TRANSFORMATION
		 *********************************************************************/
		while(empty(bwt)) continue;

		start = clock();
		Text mtfOutput = mtf(dequeue(bwt));
		end = clock();

		printf("\t-MTF finished                    TIME %f ms\n",
				(((double)end - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

		/*********************************************************************
		 * ZLE ENCODING
		 *********************************************************************/
		start = clock();
		Text zleOutput = zleEncoding(mtfOutput);
		end = clock();

		printf("\t-ZLE finished                    TIME %f ms\n",
				(((double)end - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

		/********************************************************************
		 * ARITHMETIC ENCODING
		 ********************************************************************/
		start = clock();
		Text compressed = encodingRoutine(zleOutput);
		end = clock();

		printf("\t-Arithmetic coding finished      TIME %f ms\n\n",
				(((double)end - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

		insertInOrderResult(compressed);
	}


	/**********************************************************************
	 * OUTPUT
	 **********************************************************************/
	writeFile(output);

	free(inZip.text);
	free(read);
	free(bwt);
}

void insertInOrderResult(Text res)
{
	ResultList *tmp, *curr, *prec;

	tmp = result;

	if(result == NULL) {
		result = (ResultList *) malloc(sizeof(ResultList));
		result->result = res;
		result->next = NULL;

	} else if(res->id < result->result->id) {

		curr = (ResultList *) malloc(sizeof(ResultList));
		curr->result = res;
		curr->next = tmp;
		result = curr;

	} else {

		while(tmp != NULL && res->id > tmp->result->id) {
			prec = tmp;
			tmp = tmp->next;
		}

		if(tmp == NULL) {
			tmp = (ResultList *) malloc(sizeof(ResultList));
			tmp->result = res;
			tmp->next = NULL;
			prec->next = tmp;

		} else {
			curr = (ResultList *) malloc(sizeof(ResultList));
			curr->result = res;
			curr->next = tmp;
			prec->next = curr;
		}
	}
}

void writeFile(FILE *output)
{
	ResultList *curr;
	unsigned char length[4];

	for(curr = result; curr != NULL; curr = curr->next) {

		encodeUnsigned(curr->result.len, length, 0);

		writeFile(output, length, 4);
		writeFile(output, curr->result.text, curr->result.len);

		free(curr);
	}
}
