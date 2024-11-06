#include "../robot/robot.h"
#include <stdlib.h>
#include <string.h>

robot_t *create_robot(arena_t *arena, uint32_t homeTileX, uint32_t homeTileY, uint8_t initialDirection)
{
    if (!validate_arena(arena) || get_tile(arena, homeTileX, homeTileY) != 0x00 || initialDirection > 3)
    {
        return 0;
    }

    robot_t *robot = malloc(sizeof(robot_t));
    if (!robot)
    {
        return 0;
    }

    robot->arena = arena;
    robot->homeTileX = homeTileX;
    robot->homeTileY = homeTileY;
    robot->x = homeTileX;
    robot->y = homeTileY;
    robot->direction = initialDirection;
    robot->markerCount = 0;

    return robot;
}

void dispose_robot(robot_t *robot)
{
    if (robot)
    {
        free(robot);
    }
}

uint8_t validate_robot(robot_t *robot)
{
    if (!robot || !validate_arena(robot->arena) || get_tile(robot->arena, robot->x, robot->y) == 0xFF || robot->direction > 3)
    {
        return 0;
    }
    return 1;
}

void left(robot_t *robot)
{
    if (validate_robot(robot))
    {
        robot->direction = (robot->direction + 3) % 4;
    }
}

void right(robot_t *robot)
{
    if (validate_robot(robot))
    {
        robot->direction = (robot->direction + 1) % 4;
    }
}

int atMarker(robot_t *robot)
{
    if (validate_robot(robot))
    {
        return get_tile(robot->arena, robot->x, robot->y) == 0x02;
    }
    
    return 0;
}

int markerCount(robot_t *robot)
{
    if (validate_robot(robot))
    {
        return robot->markerCount;
    }

    return 0;
}

void pickUpMarker(robot_t *robot)
{
    if (validate_robot(robot) && atMarker(robot))
    {
        robot->markerCount += 1;
        set_empty_tile(robot->arena, robot->x, robot->y);
    }
}

void dropMarker(robot_t *robot)
{
    if (validate_robot(robot) && robot->markerCount > 0 && get_tile(robot->arena, robot->x, robot->y) == 0x00)
    {
        if (robot->x == robot->homeTileX && robot->y == robot->homeTileY)
        {
            robot->markerCount = 0;
        }
        else
        {
            robot->markerCount -= 1;
            set_marker_tile(robot->arena, robot->x, robot->y);
        }
    }
}

int canMoveForward(robot_t *robot)
{
    if (!validate_robot(robot))
    {
        return 0;
    }

    int32_t x_shift = (robot->direction == 1) - (robot->direction == 3);
    int32_t y_shift = (robot->direction == 2) - (robot->direction == 0);

    uint32_t new_x = robot->x + x_shift;
    uint32_t new_y = robot->y + y_shift;

    if (new_x >= robot->arena->width || new_y >= robot->arena->height)
    {
        return 0;
    }

    uint8_t result = get_tile(robot->arena, new_x, new_y);

    return result == 0x00 || result == 0x02;
}

void forward(robot_t *robot)
{
    if (canMoveForward(robot)) // validation inside
    {
        int32_t x_shift = (robot->direction == 1) - (robot->direction == 3);
        int32_t y_shift = (robot->direction == 2) - (robot->direction == 0);

        robot->x += x_shift;
        robot->y += y_shift;
    }
}
