#include "./maze.h"
#include "../pathfinder/pathfinder.h"
#include "../graphics/graphics.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

int32_t validate_maze_settings(maze_settings_t settings)
{
    if(!settings.width || !settings.height || !settings.pixelPerSide || settings.robotInitialDirection > 3)
    {
        return 0;
    }

    if(settings.markerCount && !(settings.markersX && settings.markersY))
    {
        return 0;
    }

    for(int i = 0; i < settings.markerCount; i++)
    {
        if((settings.markersX[i] >= settings.width || settings.markersY[i] >= settings.height) ||
            (settings.markersX[i] == settings.robotHomeX && settings.markersY[i] == settings.robotHomeY) ||
            (settings.markersX[i] == settings.robotStartX && settings.markersY[i] == settings.robotStartY))
        {
            return 0;
        }
    }

    if(settings.obstacleCount && !(settings.obstaclesX && settings.obstaclesY))
    {
        return 0;
    }

    for(int i = 0; i < settings.obstacleCount; i++)
    {
        if((settings.obstaclesX[i] >= settings.width || settings.obstaclesY[i] >= settings.height) ||
            (settings.obstaclesX[i] == settings.robotHomeX && settings.obstaclesY[i] == settings.robotHomeY) ||
            (settings.obstaclesX[i] == settings.robotStartX && settings.obstaclesY[i] == settings.robotStartY))
        {
            return 0;
        }
    }

    if(settings.nonExistentCount && !(settings.nonExistentX && settings.nonExistentY))
    {
        return 0;
    }

    for(int i = 0; i < settings.nonExistentCount; i++)
    {
        if((settings.nonExistentX[i] >= settings.width || settings.nonExistentY[i] >= settings.height) ||
            (settings.nonExistentX[i] == settings.robotHomeX && settings.nonExistentY[i] == settings.robotHomeY) ||
            (settings.nonExistentX[i] == settings.robotStartX && settings.nonExistentY[i] == settings.robotStartY))
        {
            return 0;
        }
    }

    return 1;
}

maze_t *create_maze(maze_settings_t settings)
{
    if(!validate_maze_settings(settings))
    {
        return 0;
    }

    maze_t *maze = malloc(sizeof(maze_t));
    if (!maze)
    {
        return 0;
    }

    memset(maze, 0, sizeof(maze_t));

    arena_t *mainArena = create_arena(settings.width, settings.height);
    if (!mainArena)
    {
        free(maze);
        return 0;
    }

    maze->arena = mainArena;

    for (uint32_t i = 0; i < settings.markerCount; i++)
    {
        set_marker_tile(mainArena, settings.markersX[i], settings.markersY[i]);
    }

    for (uint32_t i = 0; i < settings.obstacleCount; i++)
    {
        set_obstacle_tile(mainArena, settings.obstaclesX[i], settings.obstaclesY[i]);
    }

    for (uint32_t i = 0; i < settings.nonExistentCount; i++)
    {
        delete_tile(mainArena, settings.nonExistentX[i], settings.nonExistentY[i]);
    }

    arena_draw_parameters_t aParameters = {mainArena, settings.paddingSize, settings.backgroundColor0RGB, settings.pixelPerSide};
    maze->arenaParameters = aParameters;
    draw_arena(aParameters);

    robot_t *mainRobot = create_robot(mainArena, settings.robotHomeX, settings.robotHomeY, settings.robotInitialDirection);
    if (!mainRobot)
    {
        dispose_arena(mainArena);
        free(maze);
        return NULL;
    }
    mainRobot->x = settings.robotStartX;
    mainRobot->y = settings.robotStartY;
    maze->robot = mainRobot;

    robot_draw_parameters_t rParameters = {mainRobot, settings.robotBorderColor0RGB, settings.robotFillColor0RGB};
    maze->robotParameters = rParameters;
    draw_robot(rParameters);

    maze->settings = settings;

    return maze;
}

int32_t validate_maze(maze_t *maze)
{
    return maze && validate_maze_settings(maze->settings) && validate_arena(maze->arena) && validate_robot(maze->robot);
}

void dispose_maze(maze_t *maze)
{
    if (maze)
    {
        dispose_robot(maze->robot);
        dispose_arena(maze->arena);
        memset(maze, 0, sizeof(maze_t));
        free(maze);
    }
}

int32_t modular_distance_diff(int32_t a, int32_t b, int32_t n)
{
    int32_t distance = (b - a + n) % n;
    if (distance > n / 2)
    {
        distance -= n;
    }
    return distance;
}

void move_robot_in_directions(robot_t *robot, uint8_t *directions, int32_t size)
{
    if(!robot || !directions || !size)
    {
        return;
    }

    for (int32_t i = 0; i < size; i++)
    {
        int32_t directionDiff = modular_distance_diff(robot->direction, directions[i], 4);

        while (directionDiff != 0)
        {
            if (directionDiff > 0)
            {
                right(robot);
                update_robot();
                sleep(100);
            }
            else if (directionDiff < 0)
            {
                left(robot);
                update_robot();
                sleep(100);
            }

            directionDiff = modular_distance_diff(robot->direction, directions[i], 4);
        }

        forward(robot);
        update_robot();
        sleep(100);
    }
}

uint32_t manhattan_distance(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

uint32_t get_closest_marker_index(maze_t *maze)
{
    if(!validate_maze(maze))
    {
        return -1;
    }

    uint32_t index = -1;
    uint32_t minDistance = INT_MAX;
    for (uint32_t i = 0; i < maze->settings.markerCount; i++)
    {
        if (get_tile(maze->arena, maze->settings.markersX[i], maze->settings.markersY[i]) != 0x02)
        {
            continue;
        }

        uint32_t distance = manhattan_distance(maze->robot->x, maze->robot->y, maze->settings.markersX[i], maze->settings.markersY[i]);
        if (distance < minDistance)
        {
            minDistance = distance;
            index = i;
        }
    }

    return index;
}

void solve_maze(maze_t *maze)
{
    if(!validate_maze(maze))
    {
        return;
    }

    while (maze->robot->markerCount < maze->settings.markerCount)
    {
        uint32_t index = get_closest_marker_index(maze);
        if (index < 0)
        {
            printf("No valid markers found.\n");
            break;
        }

        node_t *path = astar_search(maze->arena, maze->robot->x, maze->robot->y, maze->settings.markersX[index], maze->settings.markersY[index]);
        if (!path)
        {
            printf("No path found to marker %d.\n", index);
            break;
        }

        uint32_t size = 0;
        uint8_t *directions = path_to_direction_list(path, &size);
        if (!directions || size == 0)
        {
            printf("Invalid direction list for marker %d.\n", index);
            free_path(path);
            break;
        }

        move_robot_in_directions(maze->robot, directions, size);
        
        pickUpMarker(maze->robot);

        update_robot();

        free(directions);
        free_path(path);
    }

    node_t *path = astar_search(maze->arena, maze->robot->x, maze->robot->y, maze->robot->homeTileX, maze->robot->homeTileY);
    if (path)
    {
        uint32_t size = 0;
        uint8_t *directions = path_to_direction_list(path, &size);
        if (directions && size > 0)
        {
            move_robot_in_directions(maze->robot, directions, size);
            free(directions);
        }
        dropMarker(maze->robot);
        free_path(path);
    }
}

uint32_t generate_random_number(uint32_t min, uint32_t max)
{
    return (rand() % (max - min + 1)) + min;
}

maze_settings_t generate_random_maze(uint32_t minWidth, uint32_t maxWidth, uint32_t minHeight, uint32_t maxHeight, uint32_t paddingSize, uint32_t backgroundColor0RGB, uint32_t pixelPerSide, uint32_t robotBorderColor0RGB, uint32_t robotFillColor0RGB)
{
    srand((unsigned int)time(0));

    uint32_t width = generate_random_number(minWidth, maxWidth);
    uint32_t height = generate_random_number(minHeight, maxHeight);

    maze_settings_t settings;
    memset(&settings, 0, sizeof(maze_settings_t));

    if (width == 0 || height == 0) return settings;

    settings.width = width;
    settings.height = height;
    settings.paddingSize = paddingSize;
    settings.backgroundColor0RGB = backgroundColor0RGB;
    settings.pixelPerSide = pixelPerSide;
    settings.robotBorderColor0RGB = robotBorderColor0RGB;
    settings.robotFillColor0RGB = robotFillColor0RGB;

    uint32_t maxObstacles = (width * height) / 4;

    uint32_t numObstacles = rand() % maxObstacles;
    settings.obstacleCount = numObstacles;
    settings.obstaclesX = malloc(numObstacles * sizeof(uint32_t));
    settings.obstaclesY = malloc(numObstacles * sizeof(uint32_t));

    uint32_t maxMarkers = (width * height) / 10;

    uint32_t numMarkers = (maxMarkers > 0) ? (rand() % maxMarkers) : 0; 
    numMarkers = numMarkers == 0 ? 1 : numMarkers; // minimum 1 marker.
    settings.markerCount = numMarkers;
    settings.markersX = malloc(numMarkers * sizeof(uint32_t));
    settings.markersY = malloc(numMarkers * sizeof(uint32_t));

    settings.nonExistentCount = 0;
    settings.nonExistentX = 0;
    settings.nonExistentY = 0;
    
    arena_t *arena = create_arena(width, height);
    for (uint32_t i = 0; i < numObstacles;)
    {
        uint32_t x = rand() % width;
        uint32_t y = rand() % height;

        if (get_tile(arena, x, y) != 0x00)
            continue;

        settings.obstaclesX[i] = x;
        settings.obstaclesY[i] = y;
        set_tile(arena, x, y, 0x01);
        i++;
    }

    for (uint32_t i = 0; i < numMarkers;)
    {
        uint32_t x = rand() % width;
        uint32_t y = rand() % height;

        if (get_tile(arena, x, y) != 0x00)
            continue;

        settings.markersX[i] = x;
        settings.markersY[i] = y;
        set_tile(arena, x, y, 0x02);
        i++;
    }

    uint32_t homeX = rand() % width;
    uint32_t homeY = rand() % height;
    uint32_t startX = rand() % width;
    uint32_t startY = rand() % height;

    while((get_tile(arena, homeX, homeY) != 0x00 || get_tile(arena, startX, startY) != 0x00) || (homeX == startX && homeY == startY))
    {
        homeX = rand() % width;
        homeY = rand() % height;
        startX = rand() % width;
        startY = rand() % height;
    }

    settings.robotHomeX = homeX;
    settings.robotHomeY = homeY;
    settings.robotStartX = startX;
    settings.robotStartY = startY;

    dispose_arena(arena);

    if(!validate_maze_settings(settings)) // if settings are somehow invalid
        return generate_random_maze(minWidth, maxWidth, minHeight, maxHeight, paddingSize, backgroundColor0RGB, pixelPerSide, robotBorderColor0RGB, robotFillColor0RGB);
    return settings;
}

// this can be copy pasted into a file and be run from a file if need be
void print_maze_settings(const maze_settings_t *settings) // for debug purposes when generating random mazes
{
    printf("Maze Settings:\n");
    printf("%u ", settings->width);
    printf("%u ", settings->height);
    printf("%u ", settings->paddingSize);
    printf("0x%x ", settings->backgroundColor0RGB);
    printf("%u\n", settings->pixelPerSide);

    printf("%u\n", settings->markerCount);
    for (uint32_t i = 0; i < settings->markerCount; ++i) {
        printf("%u %u\n", settings->markersX[i], settings->markersY[i]);
    }

    printf("%u", settings->obstacleCount);
    for (uint32_t i = 0; i < settings->obstacleCount; ++i) {
        printf("%u %u\n", settings->obstaclesX[i], settings->obstaclesY[i]);
    }

    printf("%u\n", settings->nonExistentCount);
    for (uint32_t i = 0; i < settings->nonExistentCount; ++i) {
        printf("%u %u\n", settings->nonExistentX[i], settings->nonExistentY[i]);
    }

    printf("%u %u ", settings->robotHomeX, settings->robotHomeY);
    printf("%u %u ", settings->robotStartX, settings->robotStartY);
    printf("%u ", settings->robotInitialDirection);
    printf("%u ", settings->robotBorderColor0RGB);
    printf("%u\n", settings->robotFillColor0RGB);
}