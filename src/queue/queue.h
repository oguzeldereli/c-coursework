#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>

typedef struct {
    uint32_t x;
    uint32_t y;
} queue_node_t;

typedef struct {
    uint32_t capacity;
    uint32_t size;
    int32_t front;
    int32_t rear;
    queue_node_t *nodes;
} queue_t;


queue_t *create_queue(uint32_t capacity);
void dispose_queue(queue_t *queue);

int enqueue(queue_t *queue, int x, int y);
queue_node_t dequeue(queue_t *queue);
int is_queue_empty(queue_t *queue);


#endif