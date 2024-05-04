#include "maze.h"
#include "floodFill.h"
#include "stack.h"
#include <asf.h>
#include <mouse_control_interface/walldetection_mci.h>
#include <mouse_control_interface/movement_mci.h>
#include <mouse_control_interface/time_mci.h>
#include <mouse_control_interface/configswitch_mci.h>

MazeCell     mazeDiscovered [MAZE_LENGTH * MAZE_LENGTH] = {{FALSE, FALSE, FALSE, FALSE}};
unsigned int mazeFlood      [MAZE_LENGTH * MAZE_LENGTH] = {COST_MAX};
bool         mazeVisited    [MAZE_LENGTH * MAZE_LENGTH] = {FALSE};
Direction    moveStack      [STACK_SIZE];
unsigned int stackTop       = 0;
Direction    curDir         = NORTH;
Point        curPoint       = {0, 0};
Point        startPoint     = {0, 0};
Point        lastPoint;
Point        centerPoints[] =
#if MAZE_LENGTH % 2
{
	{MAZE_LENGTH / 2, MAZE_LENGTH / 2}
};
#else
{
	{(MAZE_LENGTH / 2) - 1, (MAZE_LENGTH / 2) - 1},
	{(MAZE_LENGTH / 2) - 1, MAZE_LENGTH / 2},
	{MAZE_LENGTH / 2,       (MAZE_LENGTH / 2) - 1},
	{MAZE_LENGTH / 2,       MAZE_LENGTH / 2}
};
#endif
unsigned int numCenterPoints = sizeof(centerPoints) / sizeof(Point);

bool searchCell(Point goalPoints[], unsigned int numGoalPoints)
{
	MazeCell thisCell;

	if (containsPoint(goalPoints, numGoalPoints, curPoint))
		return TRUE;

	if (!mazeVisited[mazeIdx(curPoint)])
	{
		thisCell = mazeDiscovered[mazeIdx(curPoint)] = checkWalls();
		mazeVisited[mazeIdx(curPoint)] = TRUE;
		floodFill(mazeDiscovered, mazeFlood, goalPoints, numGoalPoints, TRUE);
	}
	else
		thisCell = mazeDiscovered[mazeIdx(curPoint)];

	unsigned int cost = COST_MAX;
	Direction nextDir;
	bool foundUnvisitedCell = FALSE;
	Point northPoint = {curPoint.x, curPoint.y + 1};
	Point eastPoint = {curPoint.x + 1, curPoint.y};
	Point southPoint = {curPoint.x, curPoint.y - 1};
	Point westPoint = {curPoint.x - 1, curPoint.y};

	if (isInRange(northPoint))
		if (!thisCell.northWall && !isExplored(northPoint))
			if (mazeFlood[mazeIdx(northPoint)] < cost)
			{
				nextDir = NORTH;
				cost = mazeFlood[mazeIdx(northPoint)];
				foundUnvisitedCell = TRUE;
			}

	if (isInRange(eastPoint))
		if (!thisCell.eastWall && !isExplored(eastPoint))
			if (mazeFlood[mazeIdx(eastPoint)] < cost)
			{
				nextDir = EAST;
				cost = mazeFlood[mazeIdx(eastPoint)];
				foundUnvisitedCell = TRUE;
			}

	if (isInRange(southPoint))
		if (!thisCell.southWall && !isExplored(southPoint))
			if (mazeFlood[mazeIdx(southPoint)] < cost)
			{
				nextDir = SOUTH;
				cost = mazeFlood[mazeIdx(southPoint)];
				foundUnvisitedCell = TRUE;
			}

	if (isInRange(westPoint))
		if (!thisCell.westWall && !isExplored(westPoint))
			if (mazeFlood[mazeIdx(westPoint)] < cost)
			{
				nextDir = WEST;
				cost = mazeFlood[mazeIdx(westPoint)];
				foundUnvisitedCell = TRUE;
			}

	if (foundUnvisitedCell)
	{
		move(nextDir);
		push(moveStack, &stackTop, nextDir);
	}
	else
	{
		moveBackward(pop(moveStack, &stackTop));
	}

	if (containsPoint(goalPoints, numGoalPoints, curPoint))
	{
		mazeDiscovered[mazeIdx(curPoint)] = checkWalls();
		mazeVisited[mazeIdx(curPoint)] = TRUE;
		return TRUE;
	}

	return FALSE;
}

bool runCell(Point goalPoints[], unsigned int numGoalPoints)
{
	MazeCell thisCell;

	if (containsPoint(goalPoints, numGoalPoints, curPoint))
		return TRUE;

	thisCell = mazeDiscovered[mazeIdx(curPoint)];

	unsigned int cost = COST_MAX;
	Direction nextDir;
	Point northPoint = {curPoint.x, curPoint.y + 1};
	Point eastPoint = {curPoint.x + 1, curPoint.y};
	Point southPoint = {curPoint.x, curPoint.y - 1};
	Point westPoint = {curPoint.x - 1, curPoint.y};

	if (isInRange(northPoint))
		if (!thisCell.northWall && isExplored(northPoint))
			if (mazeFlood[mazeIdx(northPoint)] < cost)
			{
				nextDir = NORTH;
				cost = mazeFlood[mazeIdx(northPoint)];
			}

	if (isInRange(eastPoint))
		if (!thisCell.eastWall && isExplored(eastPoint))
			if (mazeFlood[mazeIdx(eastPoint)] < cost)
			{
				nextDir = EAST;
				cost = mazeFlood[mazeIdx(eastPoint)];
			}

	if (isInRange(southPoint))
		if (!thisCell.southWall && isExplored(southPoint))
			if (mazeFlood[mazeIdx(southPoint)] < cost)
			{
				nextDir = SOUTH;
				cost = mazeFlood[mazeIdx(southPoint)];
			}

	if (isInRange(westPoint))
		if (!thisCell.westWall && isExplored(westPoint))
			if (mazeFlood[mazeIdx(westPoint)] < cost)
			{
				nextDir = WEST;
				cost = mazeFlood[mazeIdx(westPoint)];
			}

	move(nextDir);

	if (containsPoint(goalPoints, numGoalPoints, curPoint))
		return TRUE;

	return FALSE;
}

void beginBacktrack(void)
{
	floodFill(mazeDiscovered, mazeFlood, &startPoint, 1, FALSE);
}

void beginRunToGoal(void)
{
	floodFill(mazeDiscovered, mazeFlood, centerPoints, numCenterPoints, FALSE);
}

void beginGoToLastPoint(void)
{
	floodFill(mazeDiscovered, mazeFlood, &lastPoint, 1, FALSE);
}

void resumeFirstTraversal(void)
{
	floodFill(mazeDiscovered, mazeFlood, centerPoints, numCenterPoints, TRUE);
}

bool firstTraversalItr(void)
{
	return searchCell(centerPoints, numCenterPoints);
}

bool backtrackItr(void)
{
	return runCell(&startPoint, 1);
}

bool runToGoalItr(void)
{
	return runCell(centerPoints, numCenterPoints);
}

bool goToLastPointItr(void)
{
	return runCell(&lastPoint, 1);
}

Direction getCurDir(void)
{
	return curDir;
}

Point getCurPoint(void)
{
	return curPoint;
}

unsigned int mazeIdx(Point point)
{
	return (mirrorY(point.y) * MAZE_LENGTH) + point.x;
}

unsigned int mirrorY(unsigned int y)
{
	return (MAZE_LENGTH - 1) - y;
}

bool isInRange(Point point)
{
	return point.x >= 0 && point.x < MAZE_LENGTH && point.y >= 0 && point.y < MAZE_LENGTH;
}

bool containsPoint(Point pointArr[], unsigned int arrSize, Point point)
{
	for (int i = 0; i < arrSize; i++)
		if (point.x == pointArr[i].x && point.y == pointArr[i].y)
			return TRUE;

	return FALSE;
}

bool isExplored(Point point)
{
	return mazeVisited[mazeIdx(point)];
}

void resetMouse1(void)
{
	MazeCell empty = {FALSE, FALSE, FALSE, FALSE};

	for(int i = 0; i < NUM_BAD_POINTS; i++)
	{
		if(stackTop == 0)
			break;

		mazeVisited[mazeIdx(curPoint)] = FALSE;
		mazeFlood[mazeIdx(curPoint)] = COST_MAX;
		mazeDiscovered[mazeIdx(curPoint)] = empty;
		
		switch (pop(moveStack, &stackTop))
		{
			case NORTH:
				curPoint.y--;
				break;
			case SOUTH:
				curPoint.y++;
				break;
			case EAST:
				curPoint.x--;
				break;
			case WEST:
				curPoint.x++;
				break;
		}
	}

	lastPoint = curPoint;
	curPoint.x = curPoint.y = 0;
	curDir = NORTH;
}

void resetMouse2(void)
{
	curPoint.x = curPoint.y = 0;
	curDir = NORTH;
}

void move(Direction direction)
{
	switch (direction)
	{
		case NORTH:
			moveNorth();
			break;
		case SOUTH:
			moveSouth();
			break;
		case EAST:
			moveEast();
			break;
		case WEST:
			moveWest();
			break;
	}
}

void moveBackward(Direction direction)
{
	switch (direction)
	{
		case NORTH:
			moveSouth();
			break;
		case SOUTH:
			moveNorth();
			break;
		case EAST:
			moveWest();
			break;
		case WEST:
			moveEast();
			break;
	}
}


void moveForward(void)
{
	mci_MoveForward1MazeSquarePid();
	mci_DelayMs(80);
}

void moveBack(void)
{
	mci_TurnRight90DegreesPID();
	mci_DelayMs(100);
	mci_TurnRight90DegreesPID();
	mci_DelayMs(100);
	moveForward();
}

void moveLeft(void)
{
	mci_TurnLeft90DegreesPID();
	mci_DelayMs(100);
	moveForward();
}

void moveRight(void)
{
	mci_TurnRight90DegreesPID();
	mci_DelayMs(100);
	moveForward();
}

void moveNorth(void)
{
	switch (curDir)
	{
		case NORTH:
			moveForward();
			break;
		case SOUTH:
			moveBack();
			break;
		case EAST:
			moveLeft();
			break;
		case WEST:
			moveRight();
			break;
	}
	curPoint.y++;
	curDir = NORTH;
}

void moveSouth(void)
{
	switch (curDir)
	{
		case NORTH:
			moveBack();
			break;
		case SOUTH:
			moveForward();
			break;
		case EAST:
			moveRight();
			break;
		case WEST:
			moveLeft();
			break;
	}
	curPoint.y--;
	curDir = SOUTH;
}

void moveEast(void)
{
	switch (curDir)
	{
		case NORTH:
			moveRight();
			break;
		case SOUTH:
			moveLeft();
			break;
		case EAST:
			moveForward();
			break;
		case WEST:
			moveBack();
			break;
	}
	curPoint.x++;
	curDir = EAST;
}

void moveWest(void)
{
	switch (curDir)
	{
		case NORTH:
			moveLeft();
			break;
		case SOUTH:
			moveRight();
			break;
		case EAST:
			moveBack();
			break;
		case WEST:
			moveForward();
			break;
	}
	curPoint.x--;
	curDir = WEST;
}

MazeCell checkWalls()
{
	MazeCell cell;
	
	cell.northWall = checkNorthWall();
	cell.southWall = checkSouthWall();
	cell.eastWall  = checkEastWall();
	cell.westWall  = checkWestWall();
		
	return cell;
}

bool checkNorthWall(void)
{
	switch(curDir)
	{
		case NORTH:
			return checkFrontWall();
		case SOUTH:
			return checkBackWall();
		case EAST:
			return checkLeftWall();
		case WEST:
			return checkRightWall();
	}	
}

bool checkSouthWall(void)
{
	switch(curDir)
	{
		case NORTH:
			return checkBackWall();
		case SOUTH:
			return checkFrontWall();
		case EAST:
			return checkRightWall();
		case WEST:
			return checkLeftWall();
	}		
}

bool checkEastWall(void)
{
	switch(curDir)
	{
		case NORTH:
			return checkRightWall();
		case SOUTH:
			return checkLeftWall();
		case EAST:
			return checkFrontWall();
		case WEST:
			return checkBackWall();
	}		
}

bool checkWestWall(void)
{
	switch(curDir)
	{
		case NORTH:
			return checkLeftWall();
		case SOUTH:
			return checkRightWall();
		case EAST:
			return checkBackWall();
		case WEST:
			return checkFrontWall();
	}		
}

bool checkFrontWall(void)
{
	if (mci_CheckFrontWall() == MCI_WALL_NOT_FOUND)
		return FALSE;
	
	return TRUE;
}

bool checkBackWall(void)
{
	if(curPoint.x == 0 && curPoint.y == 0)
		return TRUE;
	
	return FALSE;
}

bool checkLeftWall(void)
{
	if (mci_CheckLeftWall() == MCI_WALL_NOT_FOUND)
		return FALSE;
	
	return TRUE;
}

bool checkRightWall(void)
{
	if (mci_CheckRightWall() == MCI_WALL_NOT_FOUND)
		return FALSE;
	
	return TRUE;
}