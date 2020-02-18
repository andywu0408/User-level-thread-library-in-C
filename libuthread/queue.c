#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "LinkedListNode.h"

struct queue {
    LinkedListNode_t head;
    LinkedListNode_t tail;
    int size;
};

queue_t queue_create(void)
{
    queue_t queue = (queue_t) malloc(sizeof(queue));

    /* malloc failed check */
    if (queue == NULL) {
        return NULL;
    }

    /* init members */
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;

    return queue;
}

int queue_destroy(queue_t queue)
{
    if (queue == NULL || queue->size != 0) {
        return -1;
    }

    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL) {
        return -1;
    }

    LinkedListNode_t node = constructor(data);
    if (node == NULL) {
        return -1;
    }

    if (queue->size == 0) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->size++;
    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    if (queue == NULL || data == NULL || queue->size == 0) {
        return -1;
    }

    *data = queue->head->data;
    queue->head = destructor(queue->head);
    queue->size--;

    if (queue->size == 0) {
        queue->tail = NULL;
    }

    return 0;
}

int queue_delete(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL || queue->size == 0) {
        return -1;
    }

    /* go thru the queue */

    if (queue->head->data == data) {
        queue->head = destructor(queue->head);
        if (queue->size == 1) {
            queue->tail = NULL;
        }
        queue->size--;
        return 0;
    }

    LinkedListNode_t temp = queue->head;
    while (temp->next != NULL) {
        if (temp->next->data == data) {
            if (temp->next == queue->tail) {
                queue->tail = temp;
                temp->next = destructor(temp->next);
            } else {
                temp->next = destructor(temp->next);
            }
            queue->size--;
            return 0;
        }
        temp = temp->next;
    }

    return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
    if (queue == NULL || func == NULL) {
        return -1;
    }

    LinkedListNode_t temp = queue->head;

    while (temp != NULL) {
        if (func(temp->data, arg)) {
            if (data != NULL) {
                *data = temp->data;
            }
            return 0;
        }
        temp = temp->next;
    }

    return 0;
}

int queue_length(queue_t queue) {
    return queue == NULL ? -1 : queue->size;
}
