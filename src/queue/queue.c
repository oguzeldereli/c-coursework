#include "./queue.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Inspiration from https://www.geeksforgeeks.org/queue-in-c/

queue_t *create_queue(uint32_t capacity)
{
    if(capacity == 0)
    {   
        return 0;
    }

    queue_t *queue = malloc(sizeof(queue_t));
    if(!queue)
    {
        return 0;
    }

    memset(queue, 0, sizeof(queue_t));

    queue->capacity = capacity;
    queue->nodes = calloc(capacity, sizeof(queue_node_t));
    if(!queue->nodes)
    {
        free(queue);
        return 0;
    }

    queue->size = 0;
    queue->front = 0;
    queue->rear = -1;

    return queue;
}

void dispose_queue(queue_t *queue)
{
    if(queue == 0)
    {
        return;
    }

    if(queue->nodes != 0)
    {
        free(queue->nodes);
    }

    free(queue);
}


int enqueue(queue_t *queue, int x, int y)
{
    if (queue == 0 || queue->size == queue->capacity)
    {
        return -1; 
    }

    queue->rear = (queue->rear + 1) % queue->capacity; 
    queue->nodes[queue->rear] = (queue_node_t){x, y};
    queue->size++;

    return 0;
}

queue_node_t dequeue(queue_t *queue)
{
    if (queue == 0 || queue->size <= 0)
    {
        return (queue_node_t) {UINT_MAX, UINT_MAX};
    }

    queue_node_t value = queue->nodes[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    return value;
}

int is_queue_empty(queue_t *queue)
{
    if (queue == 0)
    {
        return 1;
    }

    return queue->size == 0;
}