#pragma once
#include <algo/maze.h>

#define MAZE_LENGTH_TXT (MAZE_LENGTH * 2) + 1

extern char mazeText[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1];
extern MazeCell mazeFull[MAZE_LENGTH * MAZE_LENGTH];

int mazeInit();
int readMazeTxtFromFile(char* srcFilename, char destMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1]);
int getMazeCells(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], MazeCell* destMaze);
void printMazeTxt(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], Point point);
void printMazeCells(MazeCell* srcMaze);
void printMazeFlood(unsigned int* srcMazeFlood);
void printAndDelay(void);