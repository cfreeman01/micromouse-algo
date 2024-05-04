#pragma once

#include <stdbool.h>

#define MAZE_LENGTH    16
#define FALSE          0
#define TRUE           1
#define COST_MAX       65535
#define STACK_SIZE     1000
#define NUM_BAD_POINTS 5

/* Typedefs */
typedef enum
{
	NORTH,
	SOUTH,
	EAST,
	WEST
} Direction;

typedef struct
{
	unsigned int x;
	unsigned int y;
} Point;

typedef struct
{
	bool northWall;
	bool southWall;
	bool eastWall;
	bool westWall;
} MazeCell;

/* Traversal */
bool searchCell(Point goalPoints[], unsigned int numGoalPoints);
bool runCell(Point goalPoints[], unsigned int numGoalPoints);
void beginBacktrack(void);
void beginRunToGoal(void);
void beginGoToLastPoint(void);
void resumeFirstTraversal(void);
bool firstTraversalItr(void);
bool backtrackItr(void);
bool runToGoalItr(void);
bool goToLastPointItr(void);

/* Utilities */
Direction getCurDir(void);
Point getCurPoint(void);
unsigned int mazeIdx(Point point);
unsigned int mirrorY(unsigned int y);
bool isInRange(Point point);
bool containsPoint(Point pointArr[], unsigned int arrSize, Point point);
bool isExplored(Point point);
void resetMouse1(void);
void resetMouse2(void);

/* Movement */
void move(Direction direction);
void moveBackward(Direction direction);
void moveNorth(void);
void moveSouth(void);
void moveEast(void);
void moveWest(void);
void moveForward(void);
void moveBack(void);
void moveLeft(void);
void moveRight(void);

/* Wall checking */
MazeCell checkWalls(void);
bool checkNorthWall(void);
bool checkSouthWall(void);
bool checkEastWall(void);
bool checkWestWall(void);
bool checkFrontWall(void);
bool checkBackWall(void);
bool checkLeftWall(void);
bool checkRightWall(void);