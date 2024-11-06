#include "./defaults.h"
#include "./maze/maze.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

int interpret_argv(int argc, char **argv)
{
    // 0 is random
    // 1 is file
    int mode = 0;
    char *filename = 0;
    if(argc == 1)
    {
        mode = 0;
    }
    else if(argc == 2)
    {
        if(strcmp(argv[1], "-random") == 0)
        {
            mode = 0;
        }
        else if(strcmp(argv[1], "-file") == 0)
        {
            printf("Missing file name\n");
            return -1;
        }
        else if(strcmp(argv[1], "-help") == 0)
        {
            printf("%s -random          : generates random maze and solves it\n", argv[0]);
            printf("%s -file <filename> : generates maze from filename and solves it\n", argv[0]);
            printf("%s -help            : displays thsi message\n", argv[0]);
            return -1;
        }
        else
        {
            printf("Invalid option: %s\n", argv[1]);
            return -1;
        }
    }
    else if(argc == 3)
    {
        if(strcmp(argv[1], "-file") == 0)
        {
            mode = 1;
            filename = argv[2];
            FILE *file = fopen(filename, "r"); 
            if (file == 0) 
            {
                printf("Invalid file or filename.\n");
                return -1;
            }
            fclose(file);
        }
        else
        {
            printf("Invalid option: %s\n", argv[1]);
            return -1;
        }
    }
    else
    {
        printf("Invalid usage: use -help for commands\n");
        return -1;
    }

    return mode;
}

maze_settings_t read_settings_from_file(char *filename)
{
    maze_settings_t settings;
    FILE *file = fopen(filename, "r"); 
    if (file == 0) 
    {
        printf("Invalid file or filename.\n");
        return settings;
    }

    fscanf(file, "%u %u %u %x %u", &settings.width, &settings.height, &settings.paddingSize, &settings.backgroundColor0RGB, &settings.pixelPerSide);
    
    fscanf(file, "%u", &settings.markerCount);
    if(settings.markerCount > 0)
    {
        settings.markersX = malloc(settings.markerCount * sizeof(uint32_t));
        settings.markersY = malloc(settings.markerCount * sizeof(uint32_t));
    }
    
    for(int i = 0; i < settings.markerCount; i++)
    {
        uint32_t x = 0, y = 0;
        fscanf(file, "%u %u", &x, &y);
        settings.markersX[i] = x;
        settings.markersY[i] = y;
    }

    fscanf(file, "%u", &settings.obstacleCount);
    if(settings.obstacleCount > 0)
    {
        settings.obstaclesX = malloc(settings.obstacleCount * sizeof(uint32_t));
        settings.obstaclesY = malloc(settings.obstacleCount * sizeof(uint32_t));
    }
    
    for(int i = 0; i < settings.obstacleCount; i++)
    {
        uint32_t x = 0, y = 0;
        fscanf(file, "%u %u", &x, &y);
        settings.obstaclesX[i] = x;
        settings.obstaclesY[i] = y;
    }

    fscanf(file, "%u", &settings.nonExistentCount);
    if(settings.nonExistentCount > 0)
    {
        settings.nonExistentX = malloc(settings.nonExistentCount * sizeof(uint32_t));
        settings.nonExistentY = malloc(settings.nonExistentCount * sizeof(uint32_t));
    }
    
    for(int i = 0; i < settings.nonExistentCount; i++)
    {
        uint32_t x = 0, y = 0;
        fscanf(file, "%u %u", &x, &y);
        settings.nonExistentX[i] = x;
        settings.nonExistentY[i] = y;
    }

    fscanf(file, "%u %u %u %u %u %x %x", &settings.robotHomeX, &settings.robotHomeY, &settings.robotStartX, &settings.robotStartY, &settings.robotInitialDirection, &settings.robotBorderColor0RGB, &settings.robotFillColor0RGB);
    
    fclose(file);

    return settings;
}

maze_settings_t get_settings(int mode, char *filename)
{
    maze_settings_t settings;

    if(mode == 0) // Randomly generate
    {
        settings = generate_random_maze(MIN_WIDTH, MAX_WIDTH, MIN_HEIGHT, MAX_HEIGHT, PADDING_SIZE, BACKGROUND_COLOR, PIXEL_PER_SIDE, ROBOT_BORDER_COLOR, ROBOT_FILL_COLOR);
    }
    else if(mode == 1) // from file
    {
        settings = read_settings_from_file(filename);
    }

    if(!validate_maze_settings(settings))
    {
        if(mode == 1)
        {
            printf("Invalid input format.\n");
            exit(0);
        }
        else
        {
            printf("Internal error.\n");
            exit(0);
        }
    }

    return settings;
}

int main(int argc, char **argv)
{  
    // Get input mode
    int mode = interpret_argv(argc, argv);
    char *filename = 0;

    if(mode == 1)
    {
        filename = argv[2];
    }

    maze_settings_t settings = get_settings(mode, filename);
    maze_t * maze = create_maze(settings);
    if(!validate_maze(maze))
    {
        printf("Internal error or invalid input.\n");
        return 0;
    }

    solve_maze(maze);
    dispose_maze(maze);

    return 0;
}