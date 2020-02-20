#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tps.h>
#include <sem.h>

static sem_t sem1, sem2;

void *thread2(__attribute__((unused)) void *arg)
{
	char *buffer = NULL; 

	/* Check if writing to NULL buffer raises error */
	tps_create();
	assert(tps_write(0, TPS_SIZE, buffer) == -1);
	printf("thread2: passed check for writing to NULL buffer\n");

	/* Check if reading from NULL buffer raises error */
	assert(tps_read(0, TPS_SIZE, buffer)== -1);
	printf("thread2: passed check for reading from NULL buffer\n");

	/* Transfer CPU to thread 1 and get blocked */
	sem_up(sem1);
	sem_down(sem2);

	/* Transfer CPU to thread 1 and get blocked */
	sem_up(sem1);
	sem_down(sem2);

	/* Destroy TPS and quit */
	tps_destroy();
	free(buffer);
	return NULL;
}

void *thread1(__attribute__((unused)) void *arg)
{
	pthread_t tid;
	char *buffer = NULL;

	/* Create thread 2 and get blocked */
	pthread_create(&tid, NULL, thread2, NULL);
	sem_down(sem1);

	/* When we're back, clone thread 2's TPS */
	tps_clone(tid);

	/* Check if reading from NULL buffer raises error */
	assert(tps_read(0, TPS_SIZE, buffer)== -1);
	printf("thread1: passed check for reading from NULL buffer\n");

	/* Check if writing to NULL buffer raises error */
	assert(tps_write(0, 1, buffer) == -1);
	printf("thread1: passed check for writing to NULL buffer\n");

	/* Transfer CPU to thread 2 and get blocked */
	sem_up(sem2);
	sem_down(sem1);

	/* Transfer CPU to thread 2 */
	sem_up(sem2);

	/* Wait for thread2 to die, and quit */
	pthread_join(tid, NULL);
	tps_destroy();
	if(buffer){
		free(buffer);
	}
	return NULL;
}

int main(void)
{
	pthread_t tid;

	/* Create two semaphores for thread synchro */
	sem1 = sem_create(0);
	sem2 = sem_create(0);

	/* Init TPS API */
	tps_init(1);

	/* Create thread 1 and wait */
	pthread_create(&tid, NULL, thread1, NULL);
	pthread_join(tid, NULL);

	/* Destroy resources and quit */
	sem_destroy(sem1);
	sem_destroy(sem2);
	return 0;
}
