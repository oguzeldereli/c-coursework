#ifndef __ARENA_H__
#define __ARENA_H__

#include <stdint.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t *grid;
} arena_t;

arena_t *create_arena(uint32_t width, uint32_t height);
void dispose_arena(arena_t *arena);
int validate_arena(arena_t *arena);

uint8_t get_tile(arena_t *arena, uint32_t x, uint32_t y);
void set_tile(arena_t *arena, uint32_t x, uint32_t y, uint8_t type);
void set_empty_tile(arena_t *arena, uint32_t x, uint32_t y);
void set_obstacle_tile(arena_t *arena, uint32_t x, uint32_t y);
void set_marker_tile(arena_t *arena, uint32_t x, uint32_t y);
void delete_tile(arena_t *arena, uint32_t x, uint32_t y);
void set_grid(arena_t *arena, uint8_t *grid);

#endif
