#ifndef __ROBOT_H__
#define __ROBOT_H__

#include "../arena/arena.h"
#include <stdint.h>

typedef struct robot_t {
    arena_t *arena;
    uint32_t x;
    uint32_t y;
    uint32_t homeTileX;
    uint32_t homeTileY;
    uint8_t direction;
    uint8_t markerCount;
} robot_t;

robot_t *create_robot(arena_t *arena, uint32_t homeTileX, uint32_t homeTileY, uint8_t initialDirection);
void dispose_robot(robot_t *robot);
uint8_t validate_robot(robot_t *robot);

void forward(robot_t *robot);
void left(robot_t *robot);
void right(robot_t *robot);
int atMarker(robot_t *robot);
int canMoveForward(robot_t *robot);
void pickUpMarker(robot_t *robot);
void dropMarker(robot_t *robot);
int markerCount(robot_t *robot);

#endif
