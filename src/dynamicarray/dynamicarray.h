#ifndef __DYNAMICARRAY_H__
#define __DYNAMICARRAY_H__

#include <stdint.h>

typedef struct Node {
    uint32_t x, y;
    uint32_t g, h, f;
    uint32_t heapIndex;
    struct Node *parent;
} node_t;

typedef struct {
    node_t **array;
    uint32_t size;
    uint32_t capacity;
} dynamic_array_t;

node_t *create_node(uint32_t x, uint32_t y, uint32_t g, uint32_t h, node_t *parent);
dynamic_array_t* create_dynamic_array(uint32_t initialCapacity);
void dispose_dynamic_array(dynamic_array_t* dynArray);

void da_add_element(dynamic_array_t* dynArray, node_t *element);
node_t *da_get_element(dynamic_array_t* dynArray, uint32_t index);
void da_set_element(dynamic_array_t* dynArray, uint32_t index, node_t *element);
void da_delete_end(dynamic_array_t* dynArray);
void da_delete_middle(dynamic_array_t* dynArray, uint32_t index);
void da_try_swap_elements(dynamic_array_t* dynArray, uint32_t index1, uint32_t index2, uint32_t *success);

#endif
