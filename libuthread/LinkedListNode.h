//
// Created by Zesheng on 1/31/2020.
//

#ifndef PROJECT2_LINKEDLIST_H
#define PROJECT2_LINKEDLIST_H

/**
 * helper class for @class queue
 * @test passed for all functions
 */
struct LinkedListNode {
    void *data;
    struct LinkedListNode *next;
};

typedef struct LinkedListNode* LinkedListNode_t;

/**
 * basic @class LinkedListNode constructor
 * @param data
 * @return LinkedListNode pointer; NULL if memory allocation fails
 */
LinkedListNode_t constructor(void *data);

/**
 * fast free memory by returning next node ptr
 * @param node
 * @return next node ptr
 */
LinkedListNode_t destructor(LinkedListNode_t node);
#endif //PROJECT2_LINKEDLIST_H
