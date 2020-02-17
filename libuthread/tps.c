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
}

int tps_destroy(void)
{
	/* TODO: Phase 2 */
}

int tps_read(size_t offset, size_t length, void *buffer)
{
	/* TODO: Phase 2 */
}

int tps_write(size_t offset, size_t length, void *buffer)
{
	/* TODO: Phase 2 */
}

int tps_clone(pthread_t tid)
{
	/* TODO: Phase 2 */
}

