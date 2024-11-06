#include "arena.h"
#include <stdlib.h>
#include <string.h>

arena_t *create_arena(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0)
    {
        return 0;
    }

    arena_t *arena = malloc(sizeof(arena_t));
    if (!arena)
    {
        return 0;
    }

    arena->width = width;
    arena->height = height;

    arena->grid = calloc(width * height, sizeof(uint8_t));
    if (!arena->grid)
    {
        free(arena);
        return 0;
    }

    return arena;
}

void dispose_arena(arena_t *arena)
{
    if (arena)
    {
        free(arena->grid);
        free(arena);
    }
}

int validate_arena(arena_t *arena)
{
    if (!arena || !arena->grid || arena->width == 0 || arena->height == 0)
    {
        return 0;
    }

    return 1;
}

uint8_t get_tile(arena_t *arena, uint32_t x, uint32_t y)
{
    if (!validate_arena(arena) || x >= arena->width || y >= arena->height)
    {
        return 0xFF;
    }

    return arena->grid[y * arena->width + x];
}

void set_tile(arena_t *arena, uint32_t x, uint32_t y, uint8_t type)
{
    if (!validate_arena(arena) || x >= arena->width || y >= arena->height)
    {
        return;
    }
    
    arena->grid[y * arena->width + x] = type;
}

void set_empty_tile(arena_t *arena, uint32_t x, uint32_t y)
{
    set_tile(arena, x, y, 0x00);
}

void set_obstacle_tile(arena_t *arena, uint32_t x, uint32_t y)
{
    set_tile(arena, x, y, 0x01);
}

void set_marker_tile(arena_t *arena, uint32_t x, uint32_t y)
{
    set_tile(arena, x, y, 0x02);
}

void delete_tile(arena_t *arena, uint32_t x, uint32_t y)
{
    set_tile(arena, x, y, 0xFF);
}

void set_grid(arena_t *arena, uint8_t *grid)
{
    if (arena)
    {
        arena->grid = grid;
    }
}
