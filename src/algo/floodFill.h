#pragma once

#include "maze.h"

void floodFill(MazeCell* srcMazeCells, unsigned int* destFlood, Point destPoints[], unsigned int numPoints, bool open);
void floodFillRecurse(MazeCell* srcMazeCells, unsigned int* destFlood, Point point, unsigned int cost, bool open);