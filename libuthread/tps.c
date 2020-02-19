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
#include "HashMap.h"

/* global map<tid, tps> hidden in library */
map_t map = NULL;

/**
 * TPS class with
 * protected page;
 * flag is set to 1 if a target will be cloned, 0 otherwise
 * a potential target to clone;
 */
struct TPS {
    int flag_clone_target;
    pthread_t cloned_thread_TPS;
    void* page;
};

typedef struct TPS* tps_t;

/**
 * Signal handler that distinguish between real segfaults and TPS protection faults
 */
static void segv_handler(int sig, siginfo_t *si, __attribute__((unused)) void *context)
{
    /*
     * Get the address corresponding to the beginning of the page where the
     * fault occurred
     */
    void *p_fault = (void*)((uintptr_t)si->si_addr & ~(TPS_SIZE - 1));

    void **tps_list = HashMap_getValues(map);

    /* iterate through all TPS pages to find a TPS page that matches p_fault */
    for (int i = 0; i < HashMap_size(map); i++) { 
        void* curr_tps_page = *((void*)(tps_list[i]));
        if (!memcmp(curr_tps_page, p_fault, TPS_SIZE)){ 
            /* if there is p_fault inside this TPS page, there is protection error */
            printf(stderr, "TPS protection error!\n");
        }
    }

    /* In any case, restore the default signal handlers */
    signal(SIGSEGV, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    /* And transmit the signal again in order to cause the program to crash */
    raise(sig);
}

int tps_init(int segv)
{
    if (map != NULL) {
        return -1;
    }

    map = HashMap_create();
    if (map == NULL) {
        return -1;
    }

    if (segv) {
       struct sigaction sa;

       sigemptyset(&sa.sa_mask);
       sa.sa_flags = SA_SIGINFO;
       sa.sa_sigaction = segv_handler;
       sigaction(SIGBUS, &sa, NULL);
       sigaction(SIGSEGV, &sa, NULL);
    }

    return 0;
}

/**
 * utility function used for creating a TPS object
 * @return NULL in case of failure
 *         tps_t pointer otherwise
 */
tps_t TPS_create() {
    tps_t curr = (tps_t)malloc(sizeof(struct TPS));
    curr->page = mmap(NULL, TPS_SIZE, PROT_NONE , MAP_PRIVATE |
    MAP_ANONYMOUS, -1, 0);
    if (curr->page == NULL) {
        return NULL;
    }
    curr->flag_clone_target = 0;
    return curr;
}

int tps_create(void)
{
    enter_critical_section();
    pthread_t curr_tid = pthread_self();

    /* check if it has been created */
    if (HashMap_get(map, curr_tid)) {
        exit_critical_section();
        return -1;
    }
    tps_t curr = TPS_create();
    HashMap_add(map, curr_tid, (void*)curr);
    exit_critical_section();
    return 0;
}

int tps_destroy(void)
{
    enter_critical_section();
    pthread_t curr_tid = pthread_self();
    int ret_val = HashMap_remove(map, curr_tid);
    exit_critical_section();
    return ret_val;
}

int tps_read(size_t offset, size_t length, void *buffer)
{
    enter_critical_section();
    pthread_t curr_tid = pthread_self();
    if (buffer == NULL || offset + length > TPS_SIZE || HashMap_get(map,
            curr_tid) == NULL) {
        exit_critical_section();
        return -1;
    }

    tps_t curr_tps = (tps_t)HashMap_get(map, curr_tid);

    if (curr_tps->flag_clone_target) {
        tps_t cloned_read = (tps_t)HashMap_get(map, curr_tps->cloned_thread_TPS);
        mprotect(cloned_read->page, TPS_SIZE, PROT_READ);
        memcpy(buffer, cloned_read->page + offset, length);
        mprotect(cloned_read->page, TPS_SIZE, PROT_NONE);
    } else {
        mprotect(curr_tps->page, TPS_SIZE, PROT_READ);
        memcpy(buffer, curr_tps->page + offset, length);
        mprotect(curr_tps->page, TPS_SIZE, PROT_NONE);
    }
    exit_critical_section();

    return 0;
}

int tps_write(size_t offset, size_t length, void *buffer)
{
    enter_critical_section();
    pthread_t curr_tid = pthread_self();
    if (buffer == NULL || offset + length > TPS_SIZE || HashMap_get(map,
            curr_tid) == NULL) {
        exit_critical_section();
        return -1;
    }

    tps_t curr_tps = (tps_t)HashMap_get(map, curr_tid);
    /* check if curr tps is sharing */
    if (curr_tps->flag_clone_target) {
        tps_t tps_to_clone = HashMap_get(map, curr_tps->cloned_thread_TPS);
        /* open access */
        mprotect(tps_to_clone->page, TPS_SIZE, PROT_READ);
        mprotect(curr_tps->page, TPS_SIZE, PROT_WRITE);
        memcpy(curr_tps->page, tps_to_clone->page, TPS_SIZE);
        /* close access */
        mprotect(curr_tps->page, TPS_SIZE, PROT_NONE);
        mprotect(tps_to_clone->page, TPS_SIZE, PROT_NONE);
        curr_tps->flag_clone_target = 0;
    }

    /* copying */
    mprotect(curr_tps->page, TPS_SIZE, PROT_WRITE);
    memcpy(curr_tps->page + offset, buffer, length);
    mprotect(curr_tps->page, TPS_SIZE, PROT_NONE);
    exit_critical_section();
    return 0;
}

int tps_clone(pthread_t tid)
{
    enter_critical_section();
    pthread_t curr_tid = pthread_self();
    if (HashMap_get(map, curr_tid) != NULL || HashMap_get(map, tid) == NULL) {
        exit_critical_section();
        return -1;
    }

    tps_t curr_tps = TPS_create();
    curr_tps->flag_clone_target = 1;
    curr_tps->cloned_thread_TPS = tid;

    HashMap_add(map, curr_tid, curr_tps);
    exit_critical_section();
    return 0;
}

