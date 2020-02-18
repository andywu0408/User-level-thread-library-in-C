//
// Created by Zesheng on 2/17/2020.
//

#include <stdlib.h>
#include <pthread.h>

#include "HashMap.h"
#include "LinkedListNode.h"

#define MAP_INIT_LEN 5
#define EXPANSION_RATIO 1.5
#define FULL_RATIO 0.8

struct HashMap {
    LinkedListNode_t *array;
    int capacity;
    int size;
};

/* class utilities */
/**
 * key-value pair
 */
struct pair {
    pthread_t tid;
    void* tps;
};

typedef struct pair* pair_t;

pair_t key_value_pair(pthread_t tid, void* tps) {
    pair_t curr = (pair_t)malloc(sizeof(struct pair));
    curr->tid = tid;
    curr->tps = tps;
    return curr;
}

/**
 * internal use: basic hashCode using module
 * @param map
 * @param pid
 * @return hashCode of pid
 */
int hashCode(pthread_t pid, int cap) {
    return pid % cap;
}

/**
 * internal use
 * @param map
 * @return
 */
void resize(map_t map) {
    int new_size = map->capacity * EXPANSION_RATIO;
    LinkedListNode_t* new_array = (LinkedListNode_t*)malloc(new_size *
             sizeof(LinkedListNode_t));

    for (int i = 0; i < map->capacity; i++) {
        LinkedListNode_t temp = (map->array)[i];
        while (temp != NULL) {
            LinkedListNode_t next = temp->next;
            temp->next = NULL;
            /* re-slot temp LinkedListNode */
            int slot = hashCode(((pair_t)(temp->data))->tid, new_size);
            if (new_array[slot] == NULL) {
                new_array[slot] = temp;
            } else {
                /* traverse til the end */
                LinkedListNode_t prev = NULL;
                LinkedListNode_t curr = new_array[slot];
                while (curr != NULL) {
                    prev = curr;
                    curr = curr->next;
                }
                prev->next = temp;
            }
            temp = next;
        }
    }

    map->capacity = new_size;
    map->array = new_array;
}

/* class public operations */

map_t HashMap_create() {
    map_t map = (map_t)malloc(sizeof(struct HashMap));
    if (map == NULL) {
        return NULL;
    }
    map->size = 0;
    map->array = malloc(MAP_INIT_LEN * sizeof(LinkedListNode_t));
    map->capacity = MAP_INIT_LEN;
    return map->array == NULL ? NULL : map;
}

int HashMap_add(map_t map, pthread_t tid, void* tps) {
    if (map == NULL || tps == NULL) {
        return -1;
    }

    if (map->size + 1 >= map->capacity * FULL_RATIO) {
        resize(map);
    }

    int slot = hashCode(tid, map->capacity);
    pair_t curr = key_value_pair(tid, tps);
    if ((map->array)[slot] == NULL) {
        (map->array)[slot] = constructor((void*)curr);
    } else {
        /* collision, start chaining */
        LinkedListNode_t prev = NULL;
        LinkedListNode_t temp = (map->array)[slot];
        while (temp != NULL) {
            /* find tid, but already in map */
            if (((pair_t)(temp->data))->tid == tid) {
                return -1;
            }
            prev = temp;
            temp = temp->next;
        }
        prev->next = constructor((void*)curr);
    }
    map->size++;
    return 0;
}

int HashMap_remove(map_t map, pthread_t tid) {
    if (map == NULL) {
        return -1;
    }

    int slot = hashCode(tid, map->capacity);
    if ((map->array)[slot] == NULL) {
        return -1;
    } else {
        /* collision, start searching */
        LinkedListNode_t prev = NULL;
        LinkedListNode_t temp = (map->array)[slot];
        while (temp != NULL) {
            /* find tid */
            if (((pair_t)(temp->data))->tid == tid) {
                free(temp->data);
                if (prev == NULL) {
                    (map->array)[slot] == NULL;
                } else {
                    prev->next = destructor(prev->next);
                }
                map->size--;
                return 0;
            }
            prev = temp;
            temp = temp->next;
        }
    }
    return -1;
}

void* HashMap_get(map_t map, pthread_t tid) {
    if (map == NULL) {
        return NULL;
    }

    int slot = hashCode(tid, map->capacity);
    if ((map->array)[slot] == NULL) {
        return NULL;
    } else {
        LinkedListNode_t prev = NULL;
        LinkedListNode_t temp = (map->array)[slot];
        while (temp != NULL) {
            /* find tid in map */
            if (((pair_t)(temp->data))->tid == tid) {
                return ((pair_t)(temp->data))->tps;
            }
            prev = temp;
            temp = temp->next;
        }
    }
    return NULL;
}

int HashMap_size(map_t map) {
    if (map == NULL) {
        return -1;
    }
    return map->size;
}
