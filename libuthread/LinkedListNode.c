//
// Created by Zesheng on 1/31/2020.
//
#include <stdlib.h>

#include "LinkedListNode.h"

LinkedListNode_t constructor(void *data) {
    LinkedListNode_t node = (LinkedListNode_t) malloc(sizeof(struct
            LinkedListNode));

    if (node == NULL) {
        return NULL;
    }

    node->data = data;
    node->next = NULL;
    return node;
}

LinkedListNode_t destructor(LinkedListNode_t node) {
    LinkedListNode_t next = node->next;
    free(node);
    return next;
}
