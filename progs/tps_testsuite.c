#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tps.h>
#include <sem.h>

//This test file try to override mmap function and get TPS address illegally.
//The library will be tested if a signal is triggered by illegally accessing
//private TPS
//
//expected output:
//Catching TPS Protection Error...
//Get segfault and end program now...
//Segmentation fault (core dumped)

static char msg1[TPS_SIZE] = "Hello world!\n";
static char msg2[TPS_SIZE] = "hello world!\n";
void *latest_mmap_addr; // global variable to make address returned by mmap accessible

static sem_t sem1, sem2;

void *__real_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
void *__wrap_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off){
    latest_mmap_addr = __real_mmap(addr, len, prot, flags, fildes, off);
    return latest_mmap_addr;
}

void *thread2(__attribute__((unused)) void *arg)
{
	char *buffer = malloc(TPS_SIZE);

	/* Create TPS and initialize with *msg1 */
	tps_create();
    
    /* Get TPS page address as allocated via mmap() */
    char *tps_addr = latest_mmap_addr;
    
    /* Cause an intentional TPS protection error */
    printf("Catching TPS Protection Error...\n");
    printf("Get segfault and end program now...\n");
    tps_addr[0] = 0;
	tps_write(0, TPS_SIZE, msg1);

	/* Read from TPS and make sure it contains the message */
	memset(buffer, 0, TPS_SIZE);
	tps_read(0, TPS_SIZE, buffer);
	assert(!memcmp(msg1, buffer, TPS_SIZE));
	printf("thread2: read OK!\n");

	/* Transfer CPU to thread 1 and get blocked */
	sem_up(sem1);
	sem_down(sem2);

	/* When we're back, read TPS and make sure it sill contains the original */
	memset(buffer, 0, TPS_SIZE);
	tps_read(0, TPS_SIZE, buffer);
	assert(!memcmp(msg1, buffer, TPS_SIZE));
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
	char *buffer = malloc(TPS_SIZE);

	/* Create thread 2 and get blocked */
	pthread_create(&tid, NULL, thread2, NULL);
	sem_down(sem1);

	/* When we're back, clone thread 2's TPS */
	tps_clone(tid);

	/* Read the TPS and make sure it contains the original */
	memset(buffer, 0, TPS_SIZE);
	tps_read(0, TPS_SIZE, buffer);
	assert(!memcmp(msg1, buffer, TPS_SIZE));
	printf("thread1: read OK!\n");

	/* Modify TPS to cause a copy on write */
	buffer[0] = 'h';
	tps_write(0, 1, buffer);

	/* Transfer CPU to thread 2 and get blocked */
	sem_up(sem2);
	sem_down(sem1);

	/* When we're back, make sure our modification is still there */
	memset(buffer, 0, TPS_SIZE);
	tps_read(0, TPS_SIZE, buffer);
	assert(!strcmp(msg2, buffer));
	printf("thread1: read OK!\n");

	/* Transfer CPU to thread 2 */
	sem_up(sem2);

	/* Wait for thread2 to die, and quit */
	pthread_join(tid, NULL);
	tps_destroy();
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
