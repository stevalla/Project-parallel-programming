#include "../headers/bwtZip.h"

Buffer readin, bwt, arith;
Result result;
int nBlocks;

struct timespec timeout = {.tv_nsec = 500000000, .tv_sec = 0};

void compress(FILE *input, FILE *output, long chunkSize)
{
	int i = 0, index = 0, flag = 0;
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	cpu_set_t cpus;
	Text inZip;

	nBlocks = ceil((float)fileSize(input) / (float)chunkSize);
	initBuffer(&readin);
	initBuffer(&bwt);
	initBuffer(&arith);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	result.resultList = NULL;
	pthread_mutex_init(&result.mutex, NULL);
	flag = 0;

	CPU_ZERO(&cpus);
	CPU_SET(0, &cpus);
	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpus);
//	printf("MAIN THREAD %lu on CPU %d\n", pthread_self(), sched_getcpu());

	for(int j=0; j<3; j++) {

		inZip = readFile(input, chunkSize);

		if(inZip.len == 0) {
			flag = 1;
			break;
		}

		inZip.id = i++;

		enqueue(inZip, readin.queue);
	}

	printf("Number of blocks: %d\n", nBlocks);

	for(int j=0; j<3; j++) {
		setAffinity(&cpus, j+1, &attr);
		pthread_create(&threads[j], &attr, bwtStage, NULL);
	}

	setAffinity(&cpus, 4, &attr);
	pthread_create(&threads[3], &attr, mtfZleStage, NULL);
	setAffinity(&cpus, 5, &attr);
	pthread_create(&threads[4], &attr, mtfZleStage, NULL);
	setAffinity(&cpus, 6, &attr);
	pthread_create(&threads[5], &attr, arithStage, NULL);
	setAffinity(&cpus, 7, &attr);
	pthread_create(&threads[6], &attr, arithStage, NULL);

	for(int j=0; j<nBlocks-3 && !flag; j++) {

		sleep(1);

		inZip = readFile(input, MAX_CHUNK_SIZE);

		inZip.id = i++;

//		printf("MAIN THREAD:\tblock id %ld size %ld BYTE\n", inZip.id, inZip.len);

		pthread_mutex_lock(&readin.mutex);
		enqueue(inZip, readin.queue);
		pthread_cond_broadcast(&readin.cond);
		pthread_mutex_unlock(&readin.mutex);
	}

	while(index < nBlocks) {

		pthread_mutex_lock(&result.mutex);
		writeOutput(output, &index);
		pthread_mutex_unlock(&result.mutex);
		sleep(1);
	}


	if(flag)
		free(inZip.text);
	freeBuffer(&readin);
	freeBuffer(&bwt);
	freeBuffer(&arith);
	pthread_mutex_destroy(&result.mutex);
}

void setAffinity(cpu_set_t *cpus, int cpu, pthread_attr_t *attr)
{
	CPU_ZERO(cpus);
	CPU_SET(cpu, cpus);
	pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), cpus);
}

void *bwtStage(void *arg)
{
	clock_t start = clock();
	Text bwtInput;

//	printf("BWT THREAD %lu on CPU %d\n", pthread_self(), sched_getcpu());

	while(1) {

		pthread_mutex_lock(&readin.mutex);

		while(empty(readin.queue) && readin.queue->counter < nBlocks)
			pthread_cond_timedwait(&readin.cond, &readin.mutex, &timeout);

		if(!empty(readin.queue))
			bwtInput = dequeue(readin.queue);

		else if(readin.queue->counter == nBlocks) {
			pthread_mutex_unlock(&readin.mutex);
			break;

		} else {
			pthread_mutex_unlock(&readin.mutex);
			continue;
		}

		pthread_mutex_unlock(&readin.mutex);

		Text bwtOutput = bwtTransformation(bwtInput);

		pthread_mutex_lock(&bwt.mutex);
		enqueue(bwtOutput, bwt.queue);
		pthread_cond_signal(&bwt.cond);
		pthread_mutex_unlock(&bwt.mutex);

//		printf("BWT THREAD:\tblock %ld completed\n", bwtOutput.id);
	}

//	printf("BWT THREAD:\tBWT finished                    TIME %f ms\n",
//			(((double)clock() - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

	return 0;
}

void *mtfZleStage(void *arg)
{
	clock_t start = clock();
	Text mtfInput;

//	printf("MTF THREAD %lu on CPU %d\n", pthread_self(), sched_getcpu());

	while(1) {

		pthread_mutex_lock(&bwt.mutex);

		while(empty(bwt.queue) && bwt.queue->counter < nBlocks)
			pthread_cond_timedwait(&bwt.cond, &bwt.mutex, &timeout);

		if(!empty(bwt.queue))
			mtfInput = dequeue(bwt.queue);

		else if(bwt.queue->counter == nBlocks) {
			pthread_mutex_unlock(&bwt.mutex);
			break;

		} else {
			pthread_mutex_unlock(&bwt.mutex);
			continue;
		}

		pthread_mutex_unlock(&bwt.mutex);

		Text mtfOutput = mtf(mtfInput);

//		printf("\t-MTF finished                    TIME %f ms\n",
//			(((double)clock() - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

		Text zleOutput = zleEncoding(mtfOutput);

		pthread_mutex_lock(&arith.mutex);
		enqueue(zleOutput, arith.queue);
		pthread_cond_signal(&arith.cond);
		pthread_mutex_unlock(&arith.mutex);

//		printf("MTF-ZLE THREAD:\tblock %ld completed\n", zleOutput.id);
	}

//	printf("MTF-ZLE THREAD:\tZLE finished                    TIME %f ms\n",
//		(((double)clock() - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

	return 0;
}

void *arithStage(void *arg)
{
	clock_t start = clock();
	Text arithInput;

//	printf("ARITH THREAD %lu on CPU %d\n", pthread_self(), sched_getcpu());

	while(1) {

		pthread_mutex_lock(&arith.mutex);

		while(empty(arith.queue) && arith.queue->counter < nBlocks)
			pthread_cond_timedwait(&arith.cond, &arith.mutex, &timeout);

		if(!empty(arith.queue))
			arithInput = dequeue(arith.queue);
		else if(arith.queue->counter == nBlocks) {
			pthread_mutex_unlock(&arith.mutex);
			break;

		} else {
			pthread_mutex_unlock(&arith.mutex);
			continue;
		}

		pthread_mutex_unlock(&arith.mutex);

		Text compressed = encodingRoutine(arithInput);

		pthread_mutex_lock(&result.mutex);
		insertInOrderResult(compressed);
		pthread_mutex_unlock(&result.mutex);

//		printf("ARITH THREAD:\tblock %ld completed\n", compressed.id);
	}

//	printf("ARITH THREAD:\tArithmetic coding finished      TIME %f ms\n",
//	  (((double)clock() - (double)start) / (double)CLOCKS_PER_SEC) * 1000);

	return 0;
}

void insertInOrderResult(Text res)
{
	ResultList *tmp, *curr, *prec;

	tmp = result.resultList;

	if(result.resultList == NULL) {
		result.resultList = (ResultList *) malloc(sizeof(ResultList));
		result.resultList->result = res;
		result.resultList->next = NULL;

	} else if(res.id < result.resultList->result.id) {

		curr = (ResultList *) malloc(sizeof(ResultList));
		curr->result = res;
		curr->next = tmp;
		result.resultList = curr;

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

void writeOutput(FILE *output, int *index)
{
	ResultList *curr, *tmp;
	unsigned char length[4];

	curr = result.resultList;
	while(curr != NULL) {

		if(curr->result.id != *index)
			break;

		encodeUnsigned(curr->result.len, length, 0);

		writeFile(output, length, 4);
		writeFile(output, curr->result.text, curr->result.len);

//		printf("MAIN THREAD:\tWrite block %ld\n", curr->result.id);

		(*index)++;
		free(curr->result.text);

		tmp = curr;
		curr = tmp->next;
		free(tmp);
	}

	result.resultList = curr;
}
