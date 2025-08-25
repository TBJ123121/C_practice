//創建迷宮地圖並且初始化
//和將其印出來

//牆壁: '#'
//路徑: ' '
//玩家: 'P'
//出口: 'E'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/maze.h"

#define MAZE_WIDTH 10;
#define MAZE_HEIGHT 10;

void create_maze(char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    // Maze dimensions
    for(int i = 0; i < MAZE_HEIGHT; i++) {
        for(int j = 0; j < MAZE_WIDTH; j++){
            if(i == 0 || i == MAZE_HEIGHT - 1 || j == 0 || j == MAZE_WIDTH - 1) {
                maze[i][j] = '#';
            }
            else{
                maze[i][j] = ' ';
            }
        }
    }
}

void display_maze(char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    printf("Maze Layout:\n");
    for(int i = 0; i < MAZE_HEIGHT; i++) {
        for(int j = 0; j < MAZE_WIDTH; j++) {
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
}

