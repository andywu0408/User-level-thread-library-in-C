#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tps.h>
#include <sem.h>

static char msg1[5] = "Hello";

static sem_t sem1, sem2;

void *thread2(__attribute__((unused)) void *arg)
{
	char *buffer = malloc(5);

	/* Create TPS and initialize with *msg1 */
	tps_create();
	tps_write(0, 5, msg1);
	tps_write(10, 5, msg1);
	tps_write(20, 5, msg1);
	tps_write(30, 5, msg1);

	/* Read from TPS and make sure it contains the message */
	memset(buffer, 0, 5);
	tps_read(0, 5, buffer);
	assert(!memcmp(msg1, buffer, 5));
	printf("thread2: read OK!\n");

	/* Read from TPS and make sure it contains the message */
	memset(buffer, 0, 5);
	tps_read(10, 5, buffer);
	assert(!memcmp(msg1, buffer, 5));
	printf("thread2: read OK!\n");

	/* Read from TPS and make sure it contains the message */
	memset(buffer, 0, 5);
	tps_read(20, 5, buffer);
	assert(!memcmp(msg1, buffer, 5));
	printf("thread2: read OK!\n");

	/* Read from TPS and make sure it contains the message */
	memset(buffer, 0, 5);
	tps_read(30, 5, buffer);
	assert(!memcmp(msg1, buffer, 5));
	printf("thread2: read OK!\n");

	/* Transfer CPU to thread 1 and get blocked */
	sem_up(sem1);
	sem_down(sem2);

	/* When we're back, read TPS and make sure it sill contains the original */
	memset(buffer, 0, 5);
	tps_read(0, 5, buffer);
	assert(!memcmp(msg1, buffer, 5));
	printf("thread2: read OK!\n");

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
	char *buffer = malloc(5);

	/* Create thread 2 and get blocked */
	pthread_create(&tid, NULL, thread2, NULL);
	sem_down(sem1);

	/* Transfer CPU to thread 2 and get blocked */
	sem_up(sem2);
	sem_down(sem1);

	/* Transfer CPU to thread 2 */
	sem_up(sem2);

	/* Wait for thread2 to die, and quit */
	pthread_join(tid, NULL);
	free(buffer);
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
