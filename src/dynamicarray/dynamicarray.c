#include "../dynamicarray/dynamicarray.h"
#include <stdlib.h>

node_t *create_node(uint32_t x, uint32_t y, uint32_t g, uint32_t h, node_t *parent)
{
    node_t *node = malloc(sizeof(node_t));
    if (node)
    {
        node->x = x;
        node->y = y;
        node->g = g;
        node->h = h;
        node->f = g + h;
        node->heapIndex = -1;
        node->parent = parent;
    }

    return node;
}

dynamic_array_t* create_dynamic_array(uint32_t initialCapacity)
{
    if(!initialCapacity)
    {
        return 0;
    }

    dynamic_array_t* dynArray = malloc(sizeof(dynamic_array_t));
    if (dynArray)
    {
        dynArray->array = malloc(initialCapacity * sizeof(node_t *));
        if (!dynArray->array)
        {
            free(dynArray);
            return NULL;
        }
        dynArray->size = 0;
        dynArray->capacity = initialCapacity;
    }

    return dynArray;
}

void dispose_dynamic_array(dynamic_array_t* dynArray)
{
    if (dynArray)
    {
        if(dynArray->array)
        {
            free(dynArray->array);
        }
        free(dynArray);
    }
}

void resize_dynamic_array(dynamic_array_t* dynArray, int32_t isShrink)
{
    int newCapacity = isShrink ? dynArray->capacity / 2 : dynArray->capacity * 2;
    if (newCapacity < 1)
    {
        newCapacity = 1;
    }

    node_t **newArray = realloc(dynArray->array, newCapacity * sizeof(node_t *));
    if (newArray)
    {
        dynArray->array = newArray;
        dynArray->capacity = newCapacity;
    }
}

void da_add_element(dynamic_array_t* dynArray, node_t *element)
{
    if(!dynArray || !element)
    {
        return;
    }

    if (dynArray->size == dynArray->capacity)
    {
        resize_dynamic_array(dynArray, 0);
    }

    dynArray->array[dynArray->size++] = element;
}

node_t *da_get_element(dynamic_array_t* dynArray, uint32_t index)
{
    if(!dynArray)
    {
        return 0;
    }

    if (index < 0 || index >= dynArray->size)
    {
        return 0;
    }

    return dynArray->array[index];
}

void da_set_element(dynamic_array_t* dynArray, uint32_t index, node_t *element)
{
    if(!dynArray || !element)
    {
        return;
    }

    if (index >= 0 && index < dynArray->size)
    {
        dynArray->array[index] = element;
    }
}

void da_delete_end(dynamic_array_t* dynArray)
{
    if(!dynArray)
    {
        return;
    }

    if (dynArray->size > 0)
    {
        dynArray->array[--(dynArray->size)] = 0;
        if (dynArray->size <= dynArray->capacity / 4) // shrink if the new size is smaller than a quarter of the capacity
        {
            resize_dynamic_array(dynArray, 1);
        }
    }
}

void da_delete_middle(dynamic_array_t* dynArray, uint32_t index)
{
    if(!dynArray)
    {
        return;
    }

    if (index >= 0 && index < dynArray->size)
    {
        for (int32_t i = index; i < dynArray->size - 1; i++)
        {
            dynArray->array[i] = dynArray->array[i + 1];
        }
        
        dynArray->array[--(dynArray->size)] = 0;
        if (dynArray->size <= dynArray->capacity / 4)
        {
            resize_dynamic_array(dynArray, 1);
        }
    }
}

void da_try_swap_elements(dynamic_array_t* dynArray, uint32_t index1, uint32_t index2, uint32_t *success)
{
    if(!dynArray)
    {
        *success = 0;
        return;
    }

    if (index1 >= 0 && index1 < dynArray->size && index2 >= 0 && index2 < dynArray->size)
    {
        node_t *temp = dynArray->array[index1];
        dynArray->array[index1] = dynArray->array[index2];
        dynArray->array[index2] = temp;
        *success = 1;
    }
    else
    {
        *success = 0;
    }
}
