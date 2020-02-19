//
// Created by Zesheng on 2/18/2020.
//

/*
 * a throughout passing test case for self-implemented HashMap
 * test basic @operation add, get, remove, destroy, and create
 * test inner mechanism chaining when collision
 * test inner mechanism resize when map is close to full
 *
 * The tester makes each character in a buffer associate with some integer or
 * pthread_t key and put the pair into the map; Then we will use random keys
 * to get characters back in the map and if we have gotten some characters
 * back, we remove the key value pair.
 *
 * Corner cases, such as map is NULL, are all defined in the header file and
 * handled in the source. For the usage of HashMap, please refer to the
 * header file.
 *
 * expected output matched
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "HashMap.h"

int main() {
    char buffer[] = "abcd efg Hello this is my test buffer";
    map_t my_map = HashMap_create();

    for (int i = 0; i < strlen(buffer); i++) {
        printf("adding key %d, value %c\n", i * 3 + 1, buffer[i]);
        void* temp = malloc(1);
        temp = &buffer[i];
        HashMap_add(my_map, i * 3 + 1, temp);
    }

    pthread_t* curr_keys = HashMap_getKeys(my_map);
    void** curr_values = HashMap_getValues(my_map);

    printf("Keys: ");
    for (int i = 0; i < HashMap_size(my_map); i++) {
        printf("%ld ", curr_keys[i]);
    }
    printf("\n");

    printf("Values: ");
    for (int i = 0; i < HashMap_size(my_map); i++) {
        printf("%c ", *((char*)(curr_values[i])));
    }
    printf("\n");

    for (int i = 0; i < strlen(buffer) * 3 - 1; i++) {
        printf("==== Try to get key %d ====\n", i);
        void* temp = HashMap_get(my_map, i);
        if (temp == NULL) {
            printf("Key doesn't exist (DNE)\n");
        } else {
            printf("Corresponding value: %c\n", *((char*)temp));
        }
        printf("remove key resulting value %d\n", HashMap_remove(my_map, i));
        temp = HashMap_get(my_map, i);
        if (temp == NULL) {
            printf("Key doesn't exist (DNE)\n");
        } else {
            printf("Corresponding value: %c\n", *((char*)temp));
        }
    }

    printf("Destroy map with exiting value %d\n", HashMap_destroy(my_map));
}