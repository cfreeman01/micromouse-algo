#include "floodFill.h"

void floodFill(MazeCell* srcMazeCells, unsigned int* destFlood, Point destPoints[], unsigned int numPoints, bool open)
{
	for (int i = 0; i < MAZE_LENGTH * MAZE_LENGTH; i++)
		destFlood[i] = COST_MAX;

	for (unsigned int i = 0; i < numPoints; i++)
		if (open || isExplored(destPoints[i]))
			floodFillRecurse(srcMazeCells, destFlood, destPoints[i], 0, open);
}

void floodFillRecurse(MazeCell* srcMazeCells, unsigned int* destFlood, Point point, unsigned int cost, bool open)
{
	unsigned int x = point.x, y = point.y;
	MazeCell mc = srcMazeCells[mazeIdx(point)];
	destFlood[mazeIdx(point)] = cost;

	Point northPoint = {x, y + 1};
	Point southPoint = {x, y - 1};
	Point eastPoint = {x + 1, y};
	Point westPoint = {x - 1, y};

	if (isInRange(northPoint))
		if (open || isExplored(northPoint))
			if (!mc.northWall)
				if (destFlood[mazeIdx(northPoint)] > cost + 1)
					floodFillRecurse(srcMazeCells, destFlood, northPoint, cost + 1, open);

	if (isInRange(southPoint))
		if (open || isExplored(southPoint))
			if (!mc.southWall)
				if (destFlood[mazeIdx(southPoint)] > cost + 1)
					floodFillRecurse(srcMazeCells, destFlood, southPoint, cost + 1, open);

	if (isInRange(eastPoint))
		if (open || isExplored(eastPoint))
			if (!mc.eastWall)
				if (destFlood[mazeIdx(eastPoint)] > cost + 1)
					floodFillRecurse(srcMazeCells, destFlood, eastPoint, cost + 1, open);

	if (isInRange(westPoint))
		if (open || isExplored(westPoint))
			if (!mc.westWall)
				if (destFlood[mazeIdx(westPoint)] > cost + 1)
					floodFillRecurse(srcMazeCells, destFlood, westPoint, cost + 1, open);
}