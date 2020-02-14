#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"

struct semaphore {
    /* the preserved data count */
	size_t count;
	/* the queue of blocked threads */
	queue_t waiting_list;
};

sem_t sem_create(size_t count)
{
	sem_t curr = (sem_t)malloc(sizeof(struct semaphore));
	if (curr == NULL) {
        return NULL;
	}
	curr->count = count;
	curr->waiting_list = queue_create();
	if (curr->waiting_list == NULL) {
        return NULL;
	}
    return curr;
}

int sem_destroy(sem_t sem)
{
	if (sem == NULL || queue_length(sem->waiting_list) <= 0) {
        return -1;
	}

	enter_critical_section();
	queue_destroy(sem->waiting_list);
	free(sem);
	exit_critical_section();
    return 0;
}

int sem_down(sem_t sem)
{
	if (sem == NULL) {
        return -1;
	}
    enter_critical_section();
    while (sem->count == 0) {
	    /* nothing in the sem */
        pthread_t* tid = malloc(sizeof(pthread_t));
        *tid = pthread_self();
        queue_enqueue(sem->waiting_list, tid);

        /* start waiting in the queue */
        thread_block();

        /* back */
        free(tid);
	}

	sem->count--;
	exit_critical_section();
    return 0;
}

int sem_up(sem_t sem)
{
	if (sem == NULL) {
        return -1;
	}
	enter_critical_section();
	sem->count++;

	if (queue_length(sem->waiting_list) >= 1) {
        void** data = malloc(sizeof(void*));
        queue_dequeue(sem->waiting_list, data);
        pthread_t* next = (pthread_t*)(*data);
        thread_unblock(*next);
	}

	exit_critical_section();
    return 0;
}

int sem_getvalue(sem_t sem, int *sval)
{
	if (sem == NULL || sval == NULL) {
        return -1;
	}

	enter_critical_section();
	*sval = sem->count == 0 ? -queue_length(sem->waiting_list) : sem->count;
	exit_critical_section();
    return 0;
}

