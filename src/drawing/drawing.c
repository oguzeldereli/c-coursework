#include "../drawing/drawing.h"
#include "../graphics/graphics.h"
#include <math.h>
#include <stdio.h>

// Last drawn arena, there can only be 1 actively drawn arena at a time.
static int _isArenaDrawn = 0;
static arena_draw_parameters_t _currentArenaParameters;

void set_color_from_uint32(uint32_t color)
{
    setRGBColour((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

void create_window_with_background(uint32_t windowWidth, uint32_t windowHeight, uint32_t backgroundColor0RGB)
{
    setWindowSize(windowWidth, windowHeight);
    background();
    set_color_from_uint32(backgroundColor0RGB);
    fillRect(0, 0, windowWidth, windowHeight);
}

void draw_tile(uint8_t tileType, uint32_t pixelPerSide, uint32_t x, uint32_t y, uint32_t backgroundColor0RGB)
{
    if(!pixelPerSide)
    {
        return;
    }

    background();
    switch (tileType)
    {
        case 0x00:
            set_color_from_uint32(0x000000);
            fillRect(x, y, pixelPerSide, pixelPerSide);
            set_color_from_uint32(0xFFFFFF);
            fillRect(x + 1, y + 1, pixelPerSide - 2, pixelPerSide - 2);
            break;
        case 0x01:
            set_color_from_uint32(0x000000);
            fillRect(x, y, pixelPerSide, pixelPerSide);
            break;
        case 0x02:
            // Markers are colored in the foreground with the robot, they are painted white here for completeness
            set_color_from_uint32(0x000000);
            fillRect(x, y, pixelPerSide, pixelPerSide);
            set_color_from_uint32(0xFFFFFF); // Real color will be 0xDEC859
            fillRect(x + 1, y + 1, pixelPerSide - 2, pixelPerSide - 2);
            break;
        case 0xFF:
            set_color_from_uint32(backgroundColor0RGB);
            fillRect(x, y, pixelPerSide, pixelPerSide);
            break;
        default:
            set_color_from_uint32(backgroundColor0RGB);
            fillRect(x, y, pixelPerSide, pixelPerSide);
            break;
    }
}

void draw_grid(arena_t *arena, uint32_t paddingSize, uint32_t backgroundColor0RGB, uint32_t pixelPerSide)
{
    if (!validate_arena(arena) || !pixelPerSide)
    {
        return;
    }

    for (uint32_t y = 0; y < arena->height; y++)
    {
        for (uint32_t x = 0; x < arena->width; x++)
        {
            uint8_t tileType = get_tile(arena, x, y);
            draw_tile(tileType, pixelPerSide, paddingSize + x * pixelPerSide, paddingSize + y * pixelPerSide, backgroundColor0RGB);
        }
    }
}

void draw_arena(arena_draw_parameters_t parameters)
{
    if (!validate_arena(parameters.arena) || !parameters.pixelPerSide)
    {
        return;
    }

    background();
    clear();

    uint32_t windowWidth = parameters.arena->width * parameters.pixelPerSide + parameters.paddingSize * 2;
    uint32_t windowHeight = parameters.arena->height * parameters.pixelPerSide + parameters.paddingSize * 2;

    create_window_with_background(windowWidth, windowHeight, parameters.backgroundColor0RGB);
    draw_grid(parameters.arena, parameters.paddingSize, parameters.backgroundColor0RGB, parameters.pixelPerSide);
    
    _currentArenaParameters = parameters;
    _isArenaDrawn = 1;
}

void update_arena()
{
    if (!_isArenaDrawn || !validate_arena(_currentArenaParameters.arena))
    {   
        return;
    }

    draw_arena(_currentArenaParameters);
}

void clear_arena()
{
    background();
    clear();
    _isArenaDrawn = 0;
}

// Same with robot
static int _isRobotDrawn = 0;
static robot_draw_parameters_t _currentRobotParameters;

void draw_robot_circle(robot_t *robot, uint32_t fillColor, uint32_t borderColor)
{
    if (!validate_robot(robot) || robot->arena != _currentArenaParameters.arena)
    {
        return;
    }

    uint32_t x = _currentArenaParameters.paddingSize + robot->x * _currentArenaParameters.pixelPerSide;
    uint32_t y = _currentArenaParameters.paddingSize + robot->y * _currentArenaParameters.pixelPerSide;

    foreground();
    set_color_from_uint32(borderColor);
    fillOval(x, y, _currentArenaParameters.pixelPerSide, _currentArenaParameters.pixelPerSide);
    set_color_from_uint32(fillColor);
    fillOval(x + 3, y + 3, _currentArenaParameters.pixelPerSide - 6, _currentArenaParameters.pixelPerSide - 6);
}

void draw_direction_arrow(robot_t *robot, uint32_t fillColor)
{
    if (!validate_robot(robot) || robot->arena != _currentArenaParameters.arena)
    {
        return;
    }

    // calculate rotation
    double rotation = M_PI / 2 * robot->direction;

    // get the center of the arrow
    uint32_t start_x = _currentArenaParameters.paddingSize + robot->x * _currentArenaParameters.pixelPerSide;
    uint32_t start_y = _currentArenaParameters.paddingSize + robot->y * _currentArenaParameters.pixelPerSide;
    uint32_t center_x = start_x + _currentArenaParameters.pixelPerSide / 2;
    uint32_t center_y = start_y + _currentArenaParameters.pixelPerSide / 2;
    
    // calculate the coordinates of the tip, the left base point, and the right base point
    uint32_t tip_x = center_x;
    uint32_t tip_y = center_y - _currentArenaParameters.pixelPerSide / 4;
    uint32_t base_left_x = center_x - _currentArenaParameters.pixelPerSide / 8;
    uint32_t base_left_y = center_y + _currentArenaParameters.pixelPerSide / 4;
    uint32_t base_right_x = center_x + _currentArenaParameters.pixelPerSide / 8;
    uint32_t base_right_y = center_y + _currentArenaParameters.pixelPerSide / 4;

    // rotate them
    uint32_t rotated_tip_x = round((tip_x - center_x) * cos(rotation) - (tip_y - center_y) * sin(rotation) + center_x);
    uint32_t rotated_tip_y = round((tip_x - center_x) * sin(rotation) + (tip_y - center_y) * cos(rotation) + center_y);
    uint32_t rotated_base_left_x = round((base_left_x - center_x) * cos(rotation) - (base_left_y - center_y) * sin(rotation) + center_x);
    uint32_t rotated_base_left_y = round((base_left_x - center_x) * sin(rotation) + (base_left_y - center_y) * cos(rotation) + center_y);
    uint32_t rotated_base_right_x = round((base_right_x - center_x) * cos(rotation) - (base_right_y - center_y) * sin(rotation) + center_x);
    uint32_t rotated_base_right_y = round((base_right_x - center_x) * sin(rotation) + (base_right_y - center_y) * cos(rotation) + center_y);
    
    // and voila
    int x_points[3] = {rotated_tip_x, rotated_base_left_x, rotated_base_right_x};
    int y_points[3] = {rotated_tip_y, rotated_base_left_y, rotated_base_right_y};

    foreground();
    set_color_from_uint32(fillColor);
    fillPolygon(3, x_points, y_points);
}

void draw_home_tile_x(robot_t *robot)
{
    if (!validate_robot(robot) || robot->arena != _currentArenaParameters.arena)
    {
        return;
    }

    // get the home tile
    uint32_t x = _currentArenaParameters.paddingSize + robot->homeTileX * _currentArenaParameters.pixelPerSide;
    uint32_t y = _currentArenaParameters.paddingSize + robot->homeTileY * _currentArenaParameters.pixelPerSide;

    foreground();
    set_color_from_uint32(_currentRobotParameters.borderColor);

    // 3 lines bcs we want the X to be thicker than 1 pixel
    drawLine(x, y, x + _currentArenaParameters.pixelPerSide, y + _currentArenaParameters.pixelPerSide);
    drawLine(x + 1, y, x + _currentArenaParameters.pixelPerSide, y + _currentArenaParameters.pixelPerSide - 1);
    drawLine(x, y - 1, x + _currentArenaParameters.pixelPerSide - 1, y + _currentArenaParameters.pixelPerSide);

    drawLine(x + _currentArenaParameters.pixelPerSide, y, x, y + _currentArenaParameters.pixelPerSide);
    drawLine(x + _currentArenaParameters.pixelPerSide, y + 1, x + 1, y + _currentArenaParameters.pixelPerSide);
    drawLine(x + _currentArenaParameters.pixelPerSide - 1, y, x, y + _currentArenaParameters.pixelPerSide - 1);
}

void draw_arena_markers(arena_t *arena)
{
    if (!validate_arena(arena) || arena != _currentArenaParameters.arena)
    {
        return;
    }

    for(int y = 0; y < arena->height; y++)
    {
        for(int x = 0; x < arena->width; x++)
        {
            if(get_tile(arena, x, y) == 0x02)
            {
                int markerX = _currentArenaParameters.paddingSize + x * _currentArenaParameters.pixelPerSide;
                int markerY = _currentArenaParameters.paddingSize + y * _currentArenaParameters.pixelPerSide;

                foreground();
                set_color_from_uint32(0x000000);
                fillRect(markerX, markerY, _currentArenaParameters.pixelPerSide, _currentArenaParameters.pixelPerSide);
                set_color_from_uint32(0xDEC859);
                fillRect(markerX + 1, markerY + 1, _currentArenaParameters.pixelPerSide - 2, _currentArenaParameters.pixelPerSide - 2);
            }
        }
    }    
}

void draw_robot(robot_draw_parameters_t parameters)
{
    if (!validate_robot(parameters.robot) || parameters.robot->arena != _currentArenaParameters.arena)
    {
        return;
    }

    foreground();
    clear();

    _currentRobotParameters = parameters;
    _isRobotDrawn = 1;

    draw_arena_markers(parameters.robot->arena);
    draw_home_tile_x(parameters.robot);
    draw_robot_circle(parameters.robot, parameters.fillColor, parameters.borderColor);
    draw_direction_arrow(parameters.robot, parameters.borderColor);
}

void update_robot()
{
    if (!_isRobotDrawn || !validate_robot(_currentRobotParameters.robot) || _currentRobotParameters.robot->arena != _currentArenaParameters.arena)
    {
        return;
    }

    draw_robot(_currentRobotParameters);
}

void clear_robot()
{
    foreground();
    clear();
    _isRobotDrawn = 0;
}
