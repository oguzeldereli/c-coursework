#include "../minheap/minheap.h"
#include <stdlib.h>

uint32_t mh_parent_index(uint32_t i)
{
    return (i - 1) / 2;
}

uint32_t mh_left_index(uint32_t i)
{
    return 2 * i + 1;
}

uint32_t mh_right_index(uint32_t i)
{
    return 2 * i + 2;
}

void swap_nodes(node_t **x, node_t **y)
{
    if(!x || !y)
    {
        return;
    }

    node_t *temp = *x;
    *x = *y;
    *y = temp;

    int tempIndex = (*x)->heapIndex;
    (*x)->heapIndex = (*y)->heapIndex;
    (*y)->heapIndex = tempIndex;
}

min_heap_t* create_min_heap(uint32_t initialCapacity)
{
    if(!initialCapacity)
    {
        return 0;
    }

    min_heap_t* heap = malloc(sizeof(min_heap_t));
    if (heap)
    {
        heap->data = create_dynamic_array(initialCapacity);
        if (!heap->data)
        {
            free(heap);
            return 0;
        }
    }

    return heap;
}

void dispose_min_heap(min_heap_t* heap)
{
    if (heap)
    {
        dispose_dynamic_array(heap->data);
        free(heap);
    }
}

void min_heapify(min_heap_t* heap, uint32_t i)
{
    if(!heap)
    {
        return;
    }

    uint32_t smallest = i;
    uint32_t l = mh_left_index(i);
    uint32_t r = mh_right_index(i);

    if (l < heap->data->size && heap->data->array[l]->f < heap->data->array[smallest]->f)
    {
        smallest = l;
    }

    if (r < heap->data->size && heap->data->array[r]->f < heap->data->array[smallest]->f)
    {
        smallest = r;
    }

    if (smallest != i)
    {
        swap_nodes(&heap->data->array[i], &heap->data->array[smallest]);
        min_heapify(heap, smallest);
    }
}

void mh_insert(min_heap_t* heap, node_t *value)
{
    if (!heap || !value)
    {
        return;
    }

    da_add_element(heap->data, value);
    int i = heap->data->size - 1;
    value->heapIndex = i;

    while (i != 0 && heap->data->array[mh_parent_index(i)]->f > heap->data->array[i]->f)
    {
        swap_nodes(&heap->data->array[i], &heap->data->array[mh_parent_index(i)]);
        i = mh_parent_index(i);
    }
}

node_t *mh_extract_min(min_heap_t* heap)
{
    if (!heap || heap->data->size <= 0)
    {
        return 0;
    }

    node_t *root = heap->data->array[0];
    heap->data->array[0] = heap->data->array[heap->data->size - 1];
    heap->data->array[0]->heapIndex = 0;
    heap->data->size--;

    min_heapify(heap, 0);

    return root;
}

void mh_decrease_key(min_heap_t *heap, uint32_t index, uint32_t new_f)
{
    if (!heap || index >= heap->data->size)
    {
        return;
    }

    heap->data->array[index]->f = new_f;
    int i = index;

    while (i != 0 && heap->data->array[mh_parent_index(i)]->f > heap->data->array[i]->f)
    {
        swap_nodes(&heap->data->array[i], &heap->data->array[mh_parent_index(i)]);
        i = mh_parent_index(i);
    }
}
