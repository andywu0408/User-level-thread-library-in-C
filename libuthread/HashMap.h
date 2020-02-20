//
// Created by Zesheng on 2/17/2020.
//

#ifndef PROJECT3_HASHMAP_H
#define PROJECT3_HASHMAP_H

/**
 * map_t - HashMap pointer
 * designed to be used for storing TPS
 * Map<pid, TSP>, it could support generics, but I'm lazy :)
 * basic chaining map - only support simple operations
 *
 * use get if checking if a key exists is needed.
 */
typedef struct HashMap* map_t;

/**
 * create a map with default parameters
 * @return NULL if creation failed;
 *         map_t otherwise
 */
map_t HashMap_create();

/**
 * add operation
 * @param map
 * @param tid key: pthread id
 * @param tps value: tps pointer
 * @return 0 if add successfully
 *        -1 if key already exist or malloc failed or map/tps is null
 */
int HashMap_add(map_t map, pthread_t tid, void* tps);

/**
 * remove operation
 * @param map
 * @param tid
 * @return 0 if removal success
 *        -1 key tid DNE or map is null
 */
int HashMap_remove(map_t map, pthread_t tid);

/**
 * get operation
 * @param map
 * @param tid
 * @return tps if success
 *         NULL if DNE
 */
void* HashMap_get(map_t map, pthread_t tid);

/**
 * get size of map
 * @param map
 * @return size of HashMap
 */
int HashMap_size(map_t map);

/**
 * HashMap destroy; memo free; this should be called by the library or kernel
 * @param map
 * @return 0 if destroyed
 *        -1 if map is NULL
 */
int HashMap_destroy(map_t map);

/**
 * getKeys operations in HashMap
 * @param map
 * @return array of keys
 */
pthread_t* HashMap_getKeys(map_t map);

/**
 * getValues operation in HashMap
 * @param map
 * @return array of values
 */
void** HashMap_getValues(map_t map);

#endif //PROJECT3_HASHMAP_H
