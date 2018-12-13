#include "../headers/parallelBwtZip.h"

Result result = {NULL, PTHREAD_MUTEX_INITIALIZER};
int nBlocks;

struct timespec timeout = {.tv_nsec = 100000, .tv_sec = 0};

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
//	result.text = (Text *) malloc(sizeof(Text) * nBlocks);
	flag = 0;

//	for(int j=0; j<nBlocks; j++)
//		result.text[j].id = -1;

	CPU_ZERO(&cpus);
	CPU_SET(0, &cpus);
	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpus);
//	printf("MAIN THREAD %lu on CPU %d\n", pthread_self(), sched_getcpu());

	for(int j=0; j<5; j++) {

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
	pthread_create(&threads[4], &attr, bwtStage, NULL);
	setAffinity(&cpus, 6, &attr);
	pthread_create(&threads[5], &attr, bwtStage, NULL);
	setAffinity(&cpus, 7, &attr);
	pthread_create(&threads[6], &attr, arithStage, NULL);
	setAffinity(&cpus, 0, &attr);
	pthread_create(&threads[7], &attr, bwtStage, NULL);
//	setAffinity(&cpus, 1, &attr);
//	pthread_create(&threads[8], &attr, mtfZleStage, NULL);

	for(int j=0; j<nBlocks-5 && !flag; j++) {

		usleep(1000);

		inZip = readFile(input, chunkSize);

		inZip.id = i++;

//		printf("MAIN THREAD:\tblock id %ld size %ld BYTE\n", inZip.id, inZip.len);

		pthread_mutex_lock(&readin.mutex);
		enqueue(inZip, readin.queue);
		pthread_cond_signal(&readin.cond);
		pthread_mutex_unlock(&readin.mutex);
	}
//	puts("Ciao 0");

//	for(int j=0; j<3; j++)
//		pthread_join(threads[j], NULL);
//	puts("ciao");
//	pthread_join(threads[4], NULL);
//
//	puts("ciao");
//
//
//	setAffinity(&cpus, 1, &attr);
//	pthread_create(&threads[8], &attr, arithStage, NULL);
//	setAffinity(&cpus, 2, &attr);
//	pthread_create(&threads[9], &attr, arithStage, NULL);
//	setAffinity(&cpus, 3, &attr);
//	pthread_create(&threads[10], &attr, mtfZleStage, NULL);
//	setAffinity(&cpus, 5, &attr);
//	pthread_create(&threads[11], &attr, mtfZleStage, NULL);

	while(index < nBlocks) {

		pthread_mutex_lock(&result.mutex);
		writeOutput(output, &index);
		pthread_mutex_unlock(&result.mutex);
		usleep(1000);
	}

	if(flag)
		free(inZip.text);
//	free(result.text);
}

void setAffinity(cpu_set_t *cpus, int cpu, pthread_attr_t *attr)
{
	CPU_ZERO(cpus);
	CPU_SET(cpu, cpus);
	pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), cpus);
}

void *bwtStage(void *arg)
{
	long start, end;
	struct timeval timecheck;
	Text bwtInput;
	pthread_attr_t attr;
	cpu_set_t cpus;
	pthread_t thread;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

//	printf("BWT THREAD %lu on CPU %d\n", pthread_self(), sched_getcpu());

	while(1) {

		pthread_mutex_lock(&readin.mutex);

		while(empty(readin.queue) && readin.queue->counter < nBlocks)
			pthread_cond_timedwait(&readin.cond, &readin.mutex, &timeout);

		if(!empty(readin.queue)) {

//			printf("BWT THREAD %lu:\t Dequeue\n", pthread_self());
//			gettimeofday(&timecheck, NULL);
//			start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;

			bwtInput = dequeue(readin.queue);
		}

		else if(readin.queue->counter == nBlocks) {
			pthread_mutex_unlock(&readin.mutex);
			break;

		} else {
			pthread_mutex_unlock(&readin.mutex);
			continue;
		}

		pthread_mutex_unlock(&readin.mutex);

		Text bwtOutput = bwtTransformation(bwtInput);

//		gettimeofday(&timecheck, NULL);
//		end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
//		printf("BWT THREAD %lu:\t Finish bwt block %ld time: %f\n", pthread_self(),
//				bwtOutput.id, (double)(end-start));


		pthread_mutex_lock(&bwt.mutex);
//		gettimeofday(&timecheck, NULL);
//		end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
//		printf("BWT THREAD %lu:\t Enqueue at time %f\n", pthread_self(), (double)end);
		enqueue(bwtOutput, bwt.queue);
		pthread_cond_signal(&bwt.cond);
		pthread_mutex_unlock(&bwt.mutex);
	}
//	if(j++ <3 && sched_getcpu() != 0)  {
//		setAffinity(&cpus, sched_getcpu(), &attr);
//		pthread_create(&thread, &attr, arithStage, NULL);
//		puts("ciao arith");
//	} else if(sched_getcpu() != 0) {
//		setAffinity(&cpus, sched_getcpu(), &attr);
//		pthread_create(&thread, &attr, mtfZleStage, NULL);
//		puts("ciao mtf");
//	}
//	puts("ciao 0");
//	puts("Ciao bwt");

	setAffinity(&cpus, sched_getcpu(), &attr);
	pthread_create(&thread, &attr, mtfZleStage, NULL);

	return 0;
}

void *mtfZleStage(void *arg)
{
	long start, end;
	struct timeval timecheck;
	Text mtfInput;
	pthread_attr_t attr;
	cpu_set_t cpus;
	pthread_t thread;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

//	printf("MTF-ZLE THREAD %lu on CPU %d\n", pthread_self(), sched_getcpu());

	while(1) {

		pthread_mutex_lock(&bwt.mutex);

		while(empty(bwt.queue) && bwt.queue->counter < nBlocks)
			pthread_cond_timedwait(&bwt.cond, &bwt.mutex, &timeout);

		if(!empty(bwt.queue)) {

//			printf("MTF-ZLE THREAD %lu:\t Dequeue\n", pthread_self());
//			gettimeofday(&timecheck, NULL);
//			start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;

			mtfInput = dequeue(bwt.queue);
		}

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

//		gettimeofday(&timecheck, NULL);
//		end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
//		printf("MTF-ZLE THREAD %lu:\t Finish zle block %ld time: %f\n", pthread_self(),
//				zleOutput.id, (double)(end-start));

		pthread_mutex_lock(&arith.mutex);
//		gettimeofday(&timecheck, NULL);
//		end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
//		printf("MTF-ZLE THREAD %lu:\t Enqueue at time %f\n", pthread_self(), (double)end);
		enqueue(zleOutput, arith.queue);
		pthread_cond_signal(&arith.cond);
		pthread_mutex_unlock(&arith.mutex);
	}

	setAffinity(&cpus, sched_getcpu(), &attr);
	pthread_create(&thread, &attr, arithStage, NULL);
	return 0;
}

void *arithStage(void *arg)
{
	long start, end;
	struct timeval timecheck;
	Text arithInput;

//	printf("ARITH THREAD %lu on CPU %d\n", pthread_self(), sched_getcpu());

	while(1) {

		pthread_mutex_lock(&arith.mutex);

		while(empty(arith.queue) && arith.queue->counter < nBlocks)
			pthread_cond_timedwait(&arith.cond, &arith.mutex, &timeout);

		if(!empty(arith.queue)) {

//			printf("ARITH THREAD %lu:\t Dequeue\n", pthread_self());
//			gettimeofday(&timecheck, NULL);
//			start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;

			arithInput = dequeue(arith.queue);

		} else if(arith.queue->counter == nBlocks) {
			pthread_mutex_unlock(&arith.mutex);
			break;

		} else {
			pthread_mutex_unlock(&arith.mutex);
			continue;
		}

		pthread_mutex_unlock(&arith.mutex);

		Text compressed = encodingRoutine(arithInput);

//		gettimeofday(&timecheck, NULL);
//		end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
//		printf("ARITH THREAD %lu:\t Finish arith block %ld time: %f\n", pthread_self(),
//				compressed.id, (double)(end-start));

		pthread_mutex_lock(&result.mutex);
//		gettimeofday(&timecheck, NULL);
//		end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
//		printf("ARITH THREAD %lu:\t Enqueue at time %f\n", pthread_self(), (double)end);
//		result.text[compressed.id] = compressed;
		insertInOrderResult(compressed);
		pthread_mutex_unlock(&result.mutex);

	}

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
}

void writeOutput(FILE *output, int *index)
{
	ResultList *curr, *tmp;
	unsigned char length[4];
	int j = 0, end = -2;

	if(arith.queue->counter < nBlocks)
		end = 1;

	for(curr = result.resultList; curr != NULL;) {

		if((j++ == end+1) || result.resultList->result.id != *index)
			break;

		encodeUnsigned(result.resultList->result.len, length, 0);

		writeFile(output, length, 4);
		writeFile(output, result.resultList->result.text, result.resultList->result.len);
//		printf("MAIN THREAD:\tWrite block %ld\n", curr->result.id);

		(*index)++;

		tmp = curr;
		free(curr->result.text);
		curr = tmp->next;
		free(tmp);

	}
	result.resultList = curr;
}

//void writeOutputArray(FILE *output, int *index)
//{
//	unsigned char length[4];
//	int i = *index;
//
//	for(; i<nBlocks; i++) {
//
//		if(result.text[i].id != *index)
//			return;
//
//		encodeUnsigned(result.text[i].len, length, 0);
//
//		writeFile(output, length, 4);
//		writeFile(output, result.text[i].text, result.text[i].len);
//
////		printf("MAIN THREAD:\tWrite block %ld\n", curr->result.id);
//
//		(*index)++;
//	}
//}

