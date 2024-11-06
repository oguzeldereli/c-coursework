#ifndef __MAZE_H__
#define __MAZE_H__

#include "../arena/arena.h"
#include "../robot/robot.h"
#include "../drawing/drawing.h"

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t paddingSize;
    uint32_t backgroundColor0RGB;
    uint32_t pixelPerSide;

    uint32_t markerCount;
    uint32_t *markersX;
    uint32_t *markersY;

    uint32_t obstacleCount;
    uint32_t *obstaclesX;
    uint32_t *obstaclesY;

    uint32_t nonExistentCount;
    uint32_t *nonExistentX;
    uint32_t *nonExistentY;

    uint32_t robotHomeX;
    uint32_t robotHomeY;
    uint32_t robotStartX;
    uint32_t robotStartY;
    uint32_t robotInitialDirection;
    uint32_t robotBorderColor0RGB;
    uint32_t robotFillColor0RGB;
} maze_settings_t;

typedef struct {
    maze_settings_t settings;
    arena_t *arena;
    robot_t *robot;
    arena_draw_parameters_t arenaParameters;
    robot_draw_parameters_t robotParameters;
} maze_t;

int32_t validate_maze_settings(maze_settings_t settings);
maze_t *create_maze(maze_settings_t settings);
int32_t validate_maze(maze_t *maze);
void dispose_maze(maze_t *maze);
void solve_maze(maze_t *maze);
maze_settings_t generate_random_maze(uint32_t min_width, uint32_t max_width, uint32_t min_height, uint32_t max_height, uint32_t paddingSize, uint32_t backgroundColor0RGB, uint32_t pixelPerSide, uint32_t robotBorderColor0RGB, uint32_t robotFillColor0RGB);
void print_maze_settings(const maze_settings_t *settings);

#endif
