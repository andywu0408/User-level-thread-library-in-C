//
// Created by Zesheng on 2/18/2020.
//

/*
 * a throughout passing test case for self-implemented HashMap
 * test basic @operation add, get, remove, destroy, and create
 * test inner mechanism chaining when collision
 * test inner mechanism resize when map is close to full
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