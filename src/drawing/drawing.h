#ifndef __DRAWING_H__
#define __DRAWING_H__

#include <stdint.h>
#include "../arena/arena.h"
#include "../robot/robot.h"

void set_color_from_uint32(uint32_t color);
void create_window_with_background(uint32_t windowWidth, uint32_t windowHeight, uint32_t backgroundColor0RGB);

typedef struct {
    arena_t *arena;
    uint32_t paddingSize;
    uint32_t backgroundColor0RGB;
    uint32_t pixelPerSide;
} arena_draw_parameters_t;

void draw_arena(arena_draw_parameters_t parameters);
void update_arena();
void clear_arena();

typedef struct {
    robot_t *robot;
    uint32_t borderColor;
    uint32_t fillColor;
} robot_draw_parameters_t;

void draw_robot(robot_draw_parameters_t parameters);
void update_robot();
void clear_robot();

#endif
