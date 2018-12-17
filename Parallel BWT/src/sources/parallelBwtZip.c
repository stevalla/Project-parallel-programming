#include "../headers/parallelBwtZip.h"

Buffer readin = {NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
Buffer bwt = {NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
Buffer arith = {NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
Result result = {NULL, PTHREAD_MUTEX_INITIALIZER};
int nBlocks;

struct timespec timeout = {.tv_nsec = 100000, .tv_sec = 0};

void compressParallel(FILE *input,
					  FILE *output,
					  const long chunkSize)
{
	int i = 0, index = 0, flag = 0, littleChunk = 0;
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
	result.text = (Text *) malloc(sizeof(Text) * nBlocks);

	for(int j=0; j<nBlocks; j++)
		result.text[j].id = -1;

	if(fileSize(input) - (chunkSize * (nBlocks - 1)) < MIN_CHUNK_SIZE) {
		nBlocks--;
		littleChunk = 1;
	}

	CPU_ZERO(&cpus);
	CPU_SET(0, &cpus);
	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpus);

	for(int j=0; j<6; j++) {

		inZip = readFile(input, chunkSize);

		inZip.id = i++;

		if(inZip.len == 0) {
			flag = 1;
			break;
		}

		if(inZip.len < MIN_CHUNK_SIZE) {
			result.text[inZip.id] = inZip;
			flag = 1;
			break;
		}

		enqueue(inZip, readin.queue);
	}

	for(int j=0; j<5; j++) {
		setAffinity(&cpus, j+1, &attr);
		pthread_create(&threads[j], &attr, bwtStage, NULL);
	}
	setAffinity(&cpus, 0, &attr);
	pthread_create(&threads[7], &attr, bwtStage, NULL);
	setAffinity(&cpus, 6, &attr);
	pthread_create(&threads[5], &attr, mtfZleStage, NULL);
	setAffinity(&cpus, 7, &attr);
	pthread_create(&threads[6], &attr, arithStage, NULL);


	for(int j=0; j<nBlocks-6 && !flag; j++) {

		usleep(10000);

		inZip = readFile(input, chunkSize);

		inZip.id = i++;

		pthread_mutex_lock(&readin.mutex);
		enqueue(inZip, readin.queue);
		pthread_cond_signal(&readin.cond);
		pthread_mutex_unlock(&readin.mutex);
	}

	if(littleChunk) {
		inZip = readFile(input, chunkSize);
		inZip.id++;

		result.text[inZip.id] = inZip;
	}

	while(index < nBlocks) {
		writeOutput(output, &index, littleChunk);
		usleep(1000);
	}

	if(flag)
		free(inZip.text);
	free(result.text);
	pthread_attr_destroy(&attr);
}

void setAffinity(cpu_set_t *const cpus,
				 const int cpu,
				 pthread_attr_t *const attr)
{
	CPU_ZERO(cpus);
	CPU_SET(cpu, cpus);
	pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), cpus);
}

void *bwtStage(void *arg)
{
	Text bwtInput;

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
	}

	return 0;
}

void *mtfZleStage(void *arg)
{
	Text mtfInput;

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

		Text zleOutput = zleEncoding(mtfOutput);

		pthread_mutex_lock(&arith.mutex);
		enqueue(zleOutput, arith.queue);
		pthread_cond_signal(&arith.cond);
		pthread_mutex_unlock(&arith.mutex);
	}

	return 0;
}

void *arithStage(void *arg)
{
	Text arithInput;

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

		result.text[compressed.id] = compressed;
	}

	return 0;
}

void initBuffer(Buffer *const buf)
{
	buf->queue = (Queue *) malloc(sizeof(Queue));
	initQueue(buf->queue);
}

void writeOutput(FILE *output, int *const index, const int littleChunk)
{
	unsigned char length[4];
	unsigned char id[1];
	int i = *index;

	for(; i<nBlocks; i++) {

		if(result.text[i].id != *index)
			return;

		encodeUnsigned(result.text[i].len, length, 0);

		if(i == nBlocks - 1 && littleChunk)
			id[0] = 0;
		else
			id[0] = 1;

		writeFile(output, length, 4);
		writeFile(output, id, 1);
		writeFile(output, result.text[i].text, result.text[i].len);

		free(result.text[i].text);
		(*index)++;
	}
}

