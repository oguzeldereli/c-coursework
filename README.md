# Section 1 - Program Explanation

## 1.1 Overview

This program is designed to generate and solve mazes using a simulated robotic agent. The robot navigates through obstacles, locates markers scattered throughout the maze using the A* search algorithm, and collects them before returning to its home position. The program can either generate a random maze based on predefined defaults or accept a maze configuration from a file in a specified format. 

## 1.2 Program Flow

1. The program either receives pre-prepared maze settings from a file or generates a random one using the default values in defaults.h.
    1. If the input is invalid or a internal error occurs during random maze generation, program informs the user and exits.
2. The program creates a maze based on the settings from step (1).
    1. The program informs the user and exits if the maze generated from the settings is invalid.
3. The program attemps to solve the maze.
    1. The program gets the coordinates of the closest marker to the robot.
    2. The program calculates the shortest distance path between the marker and the robot using A* search.
    3. The robot goes to the closest marker by following the pre-calculated path and picks up the marker.
    4. These steps are repeated until all markers are collected or until an error occurs.
    5. Once all markers are collected, the robot returns to the home square and drops the markers, terminating the program.
    6. If any error occurs during this phase, the robot attempts to return to the home square and terminates the program.

# Section 2 - Building and Running

## 2.1 Building and running with python (requires python 3.7 minimum)

To build and run the program, you need:
- ```gcc``` 
- ```drawapp-4.0.jar``` to be located inside the root directory ()
- ```graphics.c``` and ```graphics.h``` to be located inside ```./src/graphics/``` (They are already inside by default to ensure that the program compiles out of the box)

To build, do:

> ```python bulid.py```  

To run the executable, do

> ```bin/c-coursework(.exe) -random               : randomly generates a maze```  
> ```bin/c-coursework(.exe) -file <filename>      : reads from a file format (see section 2.3)```  
> ```bin/c-coursework(.exe) -help                 : displays all possible commands```  

To build and run, do:

> ```python build.py -run                         : defaults to random generation```  
> ```python build.py -run -random                 : randomly generates a maze```  
> ```python build.py -run -file <filename>        : reads from a file format (see section 2.3).```  
> ```python build.py -run -help                   : displays all possible commands```  

To build, run, and pipe the output into ```drawapp```, do:

> ```python build.py -run -draw                   : defaults to random generation```  
> ```python build.py -run -draw -random           : randomly generates a maze```  
> ```python build.py -run -draw -file <filename>  : reads from a file format (see section 2.3)```  

When using the build script, **you need to run your shell inside the same directory as the build script and specify the filenames relative to the build script as well**.

## 2.2 Building and running without python

Compile each source file into an object file:

> ```gcc -Wall -o obj/<filename.o> -c <path_to_c_file.c>```

Link all files with ```gcc``` or ```ld```:

> ```gcc obj/object1.o obj/object2.o ... -o bin/<executable_name(.exe)>```

Run the executable:

> ```bin/<executable_name(.exe)>                  : default to random generation``` 
> ```bin/<executable_name(.exe)> -random          : randomly generates a maze```  
> ```bin/<executable_name(.exe)> -file <filename> : reads from a file format (see section 2.3)```  
> ```bin/<executable_name(.exe)> -help            : displays all possible commands```  

## 2.3 File input format

When inputting through a file, the program uses the following file format, an example is found in ```/testFiles/test_1.txt```:

```
arena_width arena_height arena_padding_size arena_background_color_0rgb arena_pixel_per_block_side max_obstacle_area_percentage max_marker_area_percentage
arena_marker_count
For the next <arena_marker_count> lines: (DO NOT INCLUDE THIS LINE)
nth_marker_x nth_marker_y
arena_obstacle_count
For the next <arena_obstacle_count> lines: (DO NOT INCLUDE THIS LINE)
nth_obstacle_x nth_obstacle_y
arena_non_existent_tile_count
For the next <arena_non_existent_tile_count> lines: (DO NOT INCLUDE THIS LINE)
nth_non_existent_tile_x nth_non_existent_tile_y
robot_home_x robot_home_y robot_start_x robot_start_y robot_initial_direction robot_border_color_0rgb robot_fill_color_0rgb\
```  
