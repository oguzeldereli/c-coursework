#include "./maze.h"
#include "../pathfinder/pathfinder.h"
#include "../graphics/graphics.h"
#include "../queue/queue.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>

int32_t validate_maze_settings(maze_settings_t settings)
{
    if (!settings.width || !settings.height || !settings.pixelPerSide || settings.robotInitialDirection > 3)
    {
        return 0;
    }

    if (settings.markerCount && !(settings.markersX && settings.markersY))
    {
        return 0;
    }

    for (int i = 0; i < settings.markerCount; i++)
    {
        if ((settings.markersX[i] >= settings.width || settings.markersY[i] >= settings.height) ||
            (settings.markersX[i] == settings.robotHomeX && settings.markersY[i] == settings.robotHomeY) ||
            (settings.markersX[i] == settings.robotStartX && settings.markersY[i] == settings.robotStartY))
        {
            return 0;
        }
    }

    if (settings.obstacleCount && !(settings.obstaclesX && settings.obstaclesY))
    {
        return 0;
    }

    for (int i = 0; i < settings.obstacleCount; i++)
    {
        if ((settings.obstaclesX[i] >= settings.width || settings.obstaclesY[i] >= settings.height) ||
            (settings.obstaclesX[i] == settings.robotHomeX && settings.obstaclesY[i] == settings.robotHomeY) ||
            (settings.obstaclesX[i] == settings.robotStartX && settings.obstaclesY[i] == settings.robotStartY))
        {
            return 0;
        }
    }

    if (settings.nonExistentCount && !(settings.nonExistentX && settings.nonExistentY))
    {
        return 0;
    }

    for (int i = 0; i < settings.nonExistentCount; i++)
    {
        if ((settings.nonExistentX[i] >= settings.width || settings.nonExistentY[i] >= settings.height) ||
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
    if (!validate_maze_settings(settings))
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
    return maze && validate_maze_settings(maze->settings) && validate_arena(maze->arena) && validate_robot(maze->robot) && are_all_spaces_connected(maze->arena);
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
    if (!robot || !directions || !size)
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

        if (atMarker(robot)) // if we happen to be on a marker as we are moving towards another one, pick it up
        {
            pickUpMarker(robot);
        }
    }
}

// Idea to use manhattan distance from https://www.geeksforgeeks.org/a-search-algorithm/
uint32_t manhattan_distance(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

uint32_t get_closest_marker_index(maze_t *maze)
{
    if (!validate_maze(maze))
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
    if (!validate_maze(maze))
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

void set_settings_parameters(maze_settings_t *settings, uint32_t minWidth, uint32_t maxWidth, uint32_t minHeight, uint32_t maxHeight, uint32_t paddingSize, uint32_t backgroundColor0RGB, uint32_t pixelPerSide, double maxObstacleAreaPercentage, double maxMarkerAreaPercentage, uint32_t robotBorderColor0RGB, uint32_t robotFillColor0RGB)
{
    srand((unsigned int)time(0));

    uint32_t width = generate_random_number(minWidth, maxWidth);
    uint32_t height = generate_random_number(minHeight, maxHeight);

    width = width == 0 ? 1 : width;
    height = height == 0 ? 1 : height;

    settings->width = width;
    settings->height = height;
    settings->paddingSize = paddingSize;
    settings->backgroundColor0RGB = backgroundColor0RGB;
    settings->pixelPerSide = pixelPerSide;
    settings->robotBorderColor0RGB = robotBorderColor0RGB;
    settings->robotFillColor0RGB = robotFillColor0RGB;

    uint32_t maxObstacles = (width * height) * maxObstacleAreaPercentage;

    uint32_t numObstacles = rand() % maxObstacles;
    settings->obstacleCount = numObstacles;

    uint32_t maxMarkers = (width * height) * maxMarkerAreaPercentage;

    uint32_t numMarkers = (maxMarkers > 0) ? (rand() % maxMarkers) : 0;
    numMarkers = numMarkers == 0 ? 1 : numMarkers; // minimum 1 marker.
    settings->markerCount = numMarkers;

    settings->nonExistentCount = 0;
    settings->nonExistentX = 0;
    settings->nonExistentY = 0;
}

void set_random_robot(arena_t *arena, maze_settings_t *settings)
{
    uint32_t homeX = rand() % arena->width;
    uint32_t homeY = rand() % arena->height;
    uint32_t startX = rand() % arena->width;
    uint32_t startY = rand() % arena->height;

    while ((get_tile(arena, homeX, homeY) != 0x00 || get_tile(arena, startX, startY) != 0x00) || (homeX == startX && homeY == startY))
    {
        homeX = rand() % arena->width;
        homeY = rand() % arena->height;
        startX = rand() % arena->width;
        startY = rand() % arena->height;
    }

    settings->robotHomeX = homeX;
    settings->robotHomeY = homeY;
    settings->robotStartX = startX;
    settings->robotStartY = startY;
}

void set_random_markers(arena_t *arena, maze_settings_t *settings)
{
    settings->markersX = malloc(settings->markerCount * sizeof(uint32_t));
    settings->markersY = malloc(settings->markerCount * sizeof(uint32_t));

    for (uint32_t i = 0; i < settings->markerCount;)
    {
        uint32_t x = rand() % arena->width;
        uint32_t y = rand() % arena->height;

        if (get_tile(arena, x, y) != 0x00)
            continue;

        settings->markersX[i] = x;
        settings->markersY[i] = y;
        set_tile(arena, x, y, 0x02);

        i++;
    }
}

void bfs(arena_t *arena, uint32_t *visited, uint32_t startX, uint32_t startY)
{
    if (arena == 0 || startX > arena->width - 1 || startY > arena->height - 1 || visited == 0)
    {
        return;
    }

    queue_t *queue = create_queue(arena->width * arena->height);

    enqueue(queue, startX, startY);
    visited[startX + startY * arena->width] = 1;

    while (!is_queue_empty(queue))
    {
        queue_node_t node = dequeue(queue);
        int x = node.x;
        int y = node.y;

        int dX[] = {-1, 1, 0, 0}; // x directions
        int dY[] = {0, 0, -1, 1}; // y directions

        for (int i = 0; i < 4; i++)
        {
            int newX = x + dX[i];
            int newY = y + dY[i];

            if (newX >= 0 && newX < arena->width && newY >= 0 && newY < arena->height &&
                !visited[newX + newY * arena->width] && (get_tile(arena, newX, newY) == 0x02 || get_tile(arena, newX, newY) == 0x00))
            {
                visited[newX + newY * arena->width] = 1;
                enqueue(queue, newX, newY);
            }
        }
    }

    dispose_queue(queue);
}

int32_t are_all_spaces_connected(arena_t *arena)
{
    if (arena == 0)
    {
        return 0;
    }

    uint32_t *visited = calloc(arena->width * arena->height, sizeof(uint32_t));

    for (int x = 0; x < arena->width; x++)
    {
        for (int y = 0; y < arena->height; y++)
        {
            if (get_tile(arena, x, y) == 0x02 || get_tile(arena, x, y) == 0x00)
            {
                bfs(arena, visited, x, y);
                goto bfs_completed; // OMG A VALID GOTO STATEMENT
            }
        }
    }

bfs_completed:

    for (int x = 0; x < arena->width; x++)
    {
        for (int y = 0; y < arena->height; y++)
        {
            if ((get_tile(arena, x, y) == 0x02 || get_tile(arena, x, y) == 0x00) && visited[x + y * arena->width] != 1)
            {
                free(visited);
                return 0;
            }
        }
    }

    free(visited);
    return 1;
}

void set_random_obstacles(arena_t *arena, maze_settings_t *settings)
{
    settings->obstaclesX = malloc(settings->obstacleCount * sizeof(uint32_t));
    settings->obstaclesY = malloc(settings->obstacleCount * sizeof(uint32_t));

    uint32_t trialCount = 0;
    for (uint32_t i = 0; i < settings->obstacleCount;)
    {
        uint32_t x = rand() % arena->width;
        uint32_t y = rand() % arena->height;

        if (get_tile(arena, x, y) != 0x00)
        {
            continue;
        }

        settings->obstaclesX[i] = x;
        settings->obstaclesY[i] = y;
        set_tile(arena, x, y, 0x01);

        if (are_all_spaces_connected(arena) == 0 && trialCount < 1000) // if this obstacle makes it so that we cannot reach a tile, regenerate it, try a 1000 times before giving up.
        {
            // revert the changes
            settings->obstaclesX[i] = 0;
            settings->obstaclesY[i] = 0;
            set_tile(arena, x, y, 0x00);
            trialCount++;
            continue;
        }

        trialCount = 0;
        i++;
    }
}

maze_settings_t generate_random_maze(uint32_t minWidth, uint32_t maxWidth, uint32_t minHeight, uint32_t maxHeight, uint32_t paddingSize, uint32_t backgroundColor0RGB, uint32_t pixelPerSide, double maxObstacleAreaPercentage, double maxMarkerAreaPercentage, uint32_t robotBorderColor0RGB, uint32_t robotFillColor0RGB)
{
    maze_settings_t settings;
    memset(&settings, 0, sizeof(maze_settings_t));

    if (minWidth == 0 || maxWidth == 0 || minWidth > maxWidth || pixelPerSide == 0)
    {
        return settings;
    }

    set_settings_parameters(&settings, minWidth, maxWidth, minHeight, maxHeight, paddingSize, backgroundColor0RGB, pixelPerSide, maxObstacleAreaPercentage, maxMarkerAreaPercentage, robotBorderColor0RGB, robotFillColor0RGB);

    arena_t *arena = create_arena(settings.width, settings.height);

    set_random_markers(arena, &settings);
    set_random_obstacles(arena, &settings);
    set_random_robot(arena, &settings);

    dispose_arena(arena);

    if (!validate_maze_settings(settings)) // if settings are somehow invalid, regenerate
        return generate_random_maze(minWidth, maxWidth, minHeight, maxHeight, paddingSize, backgroundColor0RGB, pixelPerSide, maxObstacleAreaPercentage, maxMarkerAreaPercentage, robotBorderColor0RGB, robotFillColor0RGB);
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
    for (uint32_t i = 0; i < settings->markerCount; ++i)
    {
        printf("%u %u\n", settings->markersX[i], settings->markersY[i]);
    }

    printf("%u", settings->obstacleCount);
    for (uint32_t i = 0; i < settings->obstacleCount; ++i)
    {
        printf("%u %u\n", settings->obstaclesX[i], settings->obstaclesY[i]);
    }

    printf("%u\n", settings->nonExistentCount);
    for (uint32_t i = 0; i < settings->nonExistentCount; ++i)
    {
        printf("%u %u\n", settings->nonExistentX[i], settings->nonExistentY[i]);
    }

    printf("%u %u ", settings->robotHomeX, settings->robotHomeY);
    printf("%u %u ", settings->robotStartX, settings->robotStartY);
    printf("%u ", settings->robotInitialDirection);
    printf("%u ", settings->robotBorderColor0RGB);
    printf("%u\n", settings->robotFillColor0RGB);
}