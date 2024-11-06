#ifndef __MINHEAP_H__
#define __MINHEAP_H__

#include "../dynamicarray/dynamicarray.h"

typedef struct {
    dynamic_array_t *data;
} min_heap_t;

min_heap_t* create_min_heap(uint32_t initialCapacity);
void dispose_min_heap(min_heap_t* heap);
void mh_insert(min_heap_t* heap, node_t *value);
void min_heapify(min_heap_t* heap, uint32_t i);
node_t *mh_extract_min(min_heap_t* heap);
void mh_decrease_key(min_heap_t *heap, uint32_t index, uint32_t new_f);

uint32_t mh_parent_index(uint32_t i);
uint32_t mh_left_index(uint32_t i);
uint32_t mh_right_index(uint32_t i);

#endif
