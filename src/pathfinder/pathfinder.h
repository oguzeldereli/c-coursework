#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include "../arena/arena.h"
#include "../minheap/minheap.h"

uint32_t heuristic(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
node_t *astar_search(arena_t *arena, uint32_t startX, uint32_t startY, uint32_t goalX, uint32_t goalY);
void trace_path(node_t *goal);
void free_path(node_t *goal);
uint8_t *path_to_direction_list(node_t *path, uint32_t *pathSize);

#endif
