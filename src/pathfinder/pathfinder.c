#include "pathfinder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint32_t heuristic(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

node_t *astar_search(arena_t *arena, uint32_t startX, uint32_t startY, uint32_t goalX, uint32_t goalY)
{
    uint32_t width = arena->width;
    uint32_t height = arena->height;

    node_t ***nodes = malloc(width * sizeof(node_t **));
    uint32_t **closedList = malloc(width * sizeof(uint32_t *));
    if (!nodes || !closedList)
    {
        return 0;
    }

    for (int i = 0; i < width; i++)
    {
        nodes[i] = calloc(height, sizeof(node_t *));
        closedList[i] = calloc(height, sizeof(uint32_t));
        if (!nodes[i] || !closedList[i])
        {
            return 0;
        }
    }

    min_heap_t *openList = create_min_heap(64);
    if (!openList)
    {
        return 0;
    }

    node_t *startNode = create_node(startX, startY, 0, heuristic(startX, startY, goalX, goalY), 0);
    mh_insert(openList, startNode);
    nodes[startX][startY] = startNode;

    node_t *current = 0;
    while (openList->data->size > 0)
    {
        current = mh_extract_min(openList);
        if (!current)
        {
            break;
        }
        closedList[current->x][current->y] = 1;

        if (current->x == goalX && current->y == goalY)
        {
            break;
        }

        int32_t neighbors[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
        for (int32_t i = 0; i < 4; i++)
        {
            int32_t nx = current->x + neighbors[i][0];
            int32_t ny = current->y + neighbors[i][1];

            if (nx < 0 || ny < 0 || nx >= width || ny >= height)
                continue;

            if (closedList[nx][ny] || get_tile(arena, nx, ny) == 0x01 || get_tile(arena, nx, ny) == 0xFF)
                continue;

            uint32_t g = current->g + 1;
            uint32_t h = heuristic(nx, ny, goalX, goalY);
            uint32_t f = g + h;

            node_t *neighbor = nodes[nx][ny];
            if (neighbor == 0)
            {
                neighbor = create_node(nx, ny, g, h, current);
                nodes[nx][ny] = neighbor;
                mh_insert(openList, neighbor);
            }
            else
            {
                if (g < neighbor->g)
                {
                    neighbor->g = g;
                    neighbor->h = h;
                    neighbor->f = f;
                    neighbor->parent = current;
                    mh_decrease_key(openList, neighbor->heapIndex, neighbor->f);
                }
            }
        }
    }

    node_t *path = 0;
    if (current && current->x == goalX && current->y == goalY)
    {
        path = current;
    }

    for (uint32_t i = 0; i < width; i++)
    {
        for (uint32_t j = 0; j < height; j++)
        {
            int32_t isInPath = 0;
            node_t *path_iterator = path;
            while(path_iterator != 0)
            {
                if(nodes[i][j] && nodes[i][j] == path_iterator)
                {
                    isInPath = 1;
                    break;
                }
                path_iterator = path_iterator->parent;
            }

            if (isInPath == 0)
            {
                free(nodes[i][j]);
            }
        }
        free(nodes[i]);
        free(closedList[i]);
    }

    free(nodes);
    free(closedList);
    dispose_min_heap(openList);

    return path;
}

void free_path(node_t *goal)
{
    if(!goal)
    {
        return;
    }

    node_t *path_iterator = goal;
    while(path_iterator != 0)
    {
        node_t * next = path_iterator->parent;
        free(path_iterator);
        path_iterator = next;
    }
}

uint32_t get_direction(int32_t x_shift, int32_t y_shift)
{
    if (x_shift == 1 && y_shift == 0)
    {
        return 1; // Right
    }
    else if (x_shift == -1 && y_shift == 0)
    {
        return 3; // Left
    }
    else if (x_shift == 0 && y_shift == -1)
    {
        return 0; // Up
    }
    else if (x_shift == 0 && y_shift == 1)
    {
        return 2; // Down
    }
    else
    {
        return -1;
    }
}

uint8_t *path_to_direction_list(node_t *path, uint32_t *pathSize)
{
    if (!path)
    {
        *pathSize = 0;
        return 0;
    }

    node_t *iterator = path;
    *pathSize = 0;
    while (iterator->parent)
    {
        (*pathSize)++;
        iterator = iterator->parent;
    }

    uint8_t *directions = malloc(*pathSize * sizeof(uint8_t));
    if (!directions)
    {
        *pathSize = 0;
        return 0;
    }

    iterator = path;
    for (int i = *pathSize - 1; i >= 0; i--)
    {
        int32_t x_shift = iterator->x - iterator->parent->x;
        int32_t y_shift = iterator->y - iterator->parent->y;
        directions[i] = get_direction(x_shift, y_shift);
        iterator = iterator->parent;
    }

    return directions;
}
