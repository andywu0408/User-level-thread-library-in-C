#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "queue.h"
#include "thread.h"
#include "tps.h"

struct tps {
	int tid;
	void *page; // TPS_SIZE bytes big
};

static queue_t tps_queue; //queue* that stores bunch of tps

/* TODO: Phase 2 */

/*
 * tps_init - Initialize TPS
 * @segv - Activate segfault handler
 *
 * Initialize TPS API. This function should only be called once by the client
 * application. If @segv is different than 0, the TPS API should install a
 * page fault handler that is able to recognize TPS protection errors and
 * display the message "TPS protection error!\n" on stderr.
 *
 * Return: -1 if TPS API has already been initialized, or in case of failure
 * during the initialization. 0 if the TPS API was successfully initialized.
 */
int tps_init(int segv)
{
	/* TODO: Phase 2 */
	if(segv != 0){
		// install a page fault handler that is able to recognize TPS protection errors 
		// and display the message "TPS protection error!\n" on stderr
		perror("TPS protection error!\n");
		return -1;
	}
	// initialize any internal objects before any TPS can be created
	tps_queue = queue_create();

	return 0;
}

/*
 * tps_create - Create TPS
 *
 * Create a TPS area and associate it to the current thread. The TPS area is
 * initialized to all zeros.
 *
 * Return: -1 if current thread already has a TPS, or in case of failure during
 * the creation (e.g. memory allocation). 0 if the TPS area was successfully
 * created.
 */
int tps_create(void) 
{
	/* TODO: Phase 2 */
	tps *curr;

	curr->tid = queue_length(tps_queue);
	curr->page = mmap(NULL, TPS_SIZE, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); // page is private and anonymous, and can be accessed in reading and writing.

	if(curr == NULL || curr->page == NULL || queue_length(tps_queue) > 0){ // if failure during creation or current thread already has tps
		return -1;
	}

	queue_enqueue(tps_queue, curr);

	return 0;
}

/*
 * tps_destroy - Destroy TPS
 *
 * Destroy the TPS area associated to the current thread.
 *
 * Return: -1 if current thread doesn't have a TPS. 0 if the TPS area was
 * successfully destroyed.
 */
int tps_destroy(void)
{
	/* TODO: Phase 2 */
	if(queue_length(tps_queue) <= 0){
		return -1;
	}

	queue_destroy(tps_queue);
	free(tps_queue)

	return 0;

}

/*
 * queue_func_t - Queue callback function type
 * @data: Data item
 * @arg: Extra argument
 *
 * Return: 0 to continue iterating, 1 to stop iterating at this particular item.
 */
// typedef int (*queue_func_t)(void *data, void *arg);

/*
 * queue_func_t - Queue callback function type
 * @data: Data item
 * @arg: Extra argument
 *
 * Return: 0 to continue iterating, 1 to stop iterating at this particular item.
 */
// typedef int (*queue_func_t)(void *data, void *arg);
 

int _find_target_TPS_page(void *data, int target_tid) { // helper function for tps_read()
	int cur_tid = data->tid;

	if(cur_tid != target_tid){ // if this tps is not our target 
		return 0; // continue queue with iterating
	}

	return 1; // stop queue from iterating at this particular item
}
/*
 * tps_read - Read from TPS
 * @offset: Offset where to read from in the TPS
 * @length: Length of the data to read
 * @buffer: Data buffer receiving the read data
 *
 * Read @length bytes of data from the current thread's TPS at byte offset
 * @offset into data buffer @buffer.
 *
 * Return: -1 if current thread doesn't have a TPS, or if the reading operation
 * is out of bound, or if @buffer is NULL, or in case of internal failure. 0 if 
 * the TPS was successfully read from.
 */
int tps_read(size_t offset, size_t length, void *buffer)
{
	/* TODO: Phase 2 */
	if(queue_length(tps_queue) <= 0 || queue_length(tps_queue) > offset
		|| buffer == NULL) {
		return -1;
	}
	struct tps *temp = (struct tps*)malloc(sizeof(struct tps));

	//if(int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data);)
	if(queue_iterate(tps_queue, _find_target_TPS_page, (void*)offset, &temp) == -1){
		return -1; // internal failure
	} else {
		memcpy(temp->page, buffer, length); // store the target tps's page into buffer
	}

	return 0;
}

/*
 * tps_write - Write to TPS
 * @offset: Offset where to write to in the TPS
 * @length: Length of the data to write
 * @buffer: Data buffer holding the data to be written
 *
 * Write @length bytes located in data buffer @buffer into the current thread's
 * TPS at byte offset @offset.
 *
 * If the current thread's TPS shares a memory page with another thread's TPS,
 * this should trigger a copy-on-write operation before the actual write occurs.
 *
 * Return: -1 if current thread doesn't have a TPS, or if the writing operation
 * is out of bound, or if @buffer is NULL, or in case of failure. 0 if the TPS
 * was successfully written to.
 */
int tps_write(size_t offset, size_t length, void *buffer)
{
	/* TODO: Phase 2 */
	if(queue_length(tps_queue) <= 0 || queue_length(tps_queue) > offset
		|| buffer == NULL){
		return -1;
	}

	struct tps *temp = (struct tps*)malloc(sizeof(struct tps));

	if(queue_iterate(tps_queue, _find_target_TPS_page, (void*)offset, &temp) == -1){ // internal failure
		return -1; 
	} else {
		memcpy(buffer, temp->page, length); // store data buffer to target tps's page
	}

	return 0;
}

int tps_clone(pthread_t tid)
{
	/* TODO: Phase 2 */
	
}

