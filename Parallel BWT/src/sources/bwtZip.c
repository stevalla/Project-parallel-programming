#include "../headers/bwtZip.h"

Buffer read, bwt, arith;
ResultList *result;
int flag, flag2, flag3;

struct timespec timeout = {.tv_nsec = 0, .tv_sec = 5};

void compress(FILE *input, FILE *output)
{
	int i = 0;
	pthread_t threads[NUM_THREADS];
	Text inZip;

	initBuffer(&read);
	initBuffer(&bwt);
	initBuffer(&arith);
	result = NULL;
	flag = 0;
	flag2 = 0;
	flag3 = 0;

	pthread_create(&threads[0], NULL, bwtStage, NULL);
	pthread_create(&threads[1], NULL, mtfZleStage, NULL);
	pthread_create(&threads[2], NULL, arithStage, NULL);


	while(1) {

		inZip = readFile(input, MAX_CHUNK_SIZE);

		if(inZip.len == 0) {
			flag = 1;
			break;
		}

		inZip.id = i++;

		printf("Block id %d size %ld BYTE\n", inZip.id, inZip.len);

		pthread_mutex_lock(&read.mutex);
		enqueue(inZip, read.queue);
		pthread_cond_broadcast(&read.cond);
		pthread_mutex_unlock(&read.mutex);

	}

	for(int i=0; i<NUM_THREADS; i++)
		pthread_join(threads[i], NULL);

	writeOutput(output);

	free(inZip.text);
	freeBuffer(&read);
	freeBuffer(&bwt);
	freeBuffer(&arith);
}

void *bwtStage(void *arg)
{
	clock_t start;
	Text bwtInput;

	while(1) {

		pthread_mutex_lock(&read.mutex);

		while(empty(read.queue) && !flag)
			pthread_cond_timedwait(&read.cond, &read.mutex, &timeout);

		if(!empty(read.queue))
			bwtInput = dequeue(read.queue);
		else if(flag) {
			pthread_mutex_unlock(&read.mutex);
			break;

		} else {
			pthread_mutex_unlock(&read.mutex);
			continue;
		}

		pthread_mutex_unlock(&read.mutex);

		start = clock();
		Text bwtOutput = bwtTransformation(bwtInput);

		printf("\t-BWT finished                    TIME %f ms\n",
					(((double)clock() - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

		pthread_mutex_lock(&bwt.mutex);
		enqueue(bwtOutput, bwt.queue);
		pthread_cond_signal(&bwt.cond);
		pthread_mutex_unlock(&bwt.mutex);
	}

	flag2 = 1;

	return 0;
}

void *mtfZleStage(void *arg)
{
	clock_t start;
	Text mtfInput;

	while(1) {

		pthread_mutex_lock(&bwt.mutex);

		while(empty(bwt.queue) && !flag2)
			pthread_cond_timedwait(&bwt.cond, &bwt.mutex, &timeout);

		if(!empty(bwt.queue))
			mtfInput = dequeue(bwt.queue);
		else if(flag2) {
			pthread_mutex_unlock(&bwt.mutex);
			break;
		} else {
			pthread_mutex_unlock(&bwt.mutex);
			continue;
		}

		pthread_mutex_unlock(&bwt.mutex);

		start = clock();
		Text mtfOutput = mtf(mtfInput);

		printf("\t-MTF finished                    TIME %f ms\n",
					(((double)clock() - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

		start = clock();
		Text zleOutput = zleEncoding(mtfOutput);

		printf("\t-ZLE finished                    TIME %f ms\n",
					(((double)clock() - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

		pthread_mutex_lock(&arith.mutex);
		enqueue(zleOutput, arith.queue);
		pthread_cond_signal(&arith.cond);
		pthread_mutex_unlock(&arith.mutex);
	}

	flag3 = 1;

	return 0;
}

void *arithStage(void *arg)
{
	clock_t start;
	Text arithInput;

	while(1) {

		pthread_mutex_lock(&arith.mutex);

		while(empty(arith.queue) && !flag3)
			pthread_cond_timedwait(&arith.cond, &arith.mutex, &timeout);

		if(!empty(arith.queue))
			arithInput = dequeue(arith.queue);
		else if(flag3) {
			pthread_mutex_unlock(&arith.mutex);
			break;

		} else {
			pthread_mutex_unlock(&arith.mutex);
			continue;
		}

		pthread_mutex_unlock(&arith.mutex);

		start = clock();
		Text compressed = encodingRoutine(arithInput);

		printf("\t-Arithmetic coding finished      TIME %f ms\n\n",
				(((double)clock() - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

		insertInOrderResult(compressed);
	}

	return 0;
}

void insertInOrderResult(Text res)
{
	ResultList *tmp, *curr, *prec;

	tmp = result;

	if(result == NULL) {
		result = (ResultList *) malloc(sizeof(ResultList));
		result->result = res;
		result->next = NULL;

	} else if(res.id < result->result.id) {

		curr = (ResultList *) malloc(sizeof(ResultList));
		curr->result = res;
		curr->next = tmp;
		result = curr;

	} else {

		while(tmp != NULL && res.id > tmp->result.id) {
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

void initBuffer(Buffer *buf)
{
	buf->queue = (Queue *) malloc(sizeof(Queue));
	initQueue(buf->queue);
	pthread_mutex_init(&buf->mutex, NULL);
	pthread_cond_init(&buf->cond, NULL);
}

void freeBuffer(Buffer *buf)
{
	free(buf->queue);
	pthread_mutex_destroy(&buf->mutex);
	pthread_cond_destroy(&buf->cond);
}

void writeOutput(FILE *output)
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
