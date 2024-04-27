#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

/* Variables */
char mazeText[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1];
MazeCell mazeFull[MAZE_LENGTH * MAZE_LENGTH];

MouseState   state          = FIRST_TRAVERSAL;
MazeCell     mazeDiscovered [MAZE_LENGTH * MAZE_LENGTH] = {{FALSE, FALSE, FALSE, FALSE}};
unsigned int mazeFlood      [MAZE_LENGTH * MAZE_LENGTH] = {UINT_MAX};
bool         mazeVisited    [MAZE_LENGTH * MAZE_LENGTH] = {FALSE};
Direction    moveStack      [STACK_SIZE];
unsigned int stackTop       = 0;
Direction    curDir         = NORTH;
Point        curPoint       = {0, 0};
Point        startPoint     = {0, 0};
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

/* Maze reading and display */
int readMazeTxtFromFile(char* srcFilename, char destMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1]);
int getMazeCells(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], MazeCell* destMaze);
void printMazeTxt(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], Point point);
void printMazeCells(MazeCell* srcMaze);
void printMazeFlood(unsigned int* srcMazeFlood);
void printAndDelay(void);

/* Traversal */
bool searchCell(Point goalPoints[], unsigned int numGoalPoints);
bool runCell(Point goalPoints[], unsigned int numGoalPoints);

/* Flood fill */
void floodFill(Point destPoints[], unsigned int numPoints, bool open);
void floodFillRecurse(Point point, unsigned int cost, bool open);

/* Utilities */
unsigned int mazeIdx(Point point);
unsigned int mirrorY(unsigned int y);
bool isInRange(Point point);
bool containsPoint(Point pointArr[], unsigned int arrSize, Point point);
bool isExplored(Point point);

/* Stack */
char pop (Direction* stack, unsigned int* top);
void push(Direction* stack, unsigned int* top, char data);

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

int main(void)
{
    if(readMazeTxtFromFile("C:/Users/Chris/Documents/code/nai_micromouse_algo/mazes/test.txt", mazeText))
        return 1;

    getMazeCells(mazeText, mazeFull);

    while(!searchCell(centerPoints, numCenterPoints));

	printf("\nPress any key to start backtracking.\n");
    getch();
    system("cls");

	while(!runCell(&startPoint, 1));

	printf("\nPress any key to run to goal.\n");
    getch();
    system("cls");

	while(!runCell(centerPoints, numCenterPoints));

    printf("\nPress any key to exit program.\n");
    getch();
    system("cls");
    return 0;
}

int readMazeTxtFromFile(char* srcFilename, char destMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1]){
    FILE* file  = fopen(srcFilename, "r");
    if(file == NULL){
        printf("Could not open maze file!!!\n");
        return 1;
    }

    for(int i = 0; i < MAZE_LENGTH_TXT; i++){
        fgets(destMazeTxt[i], MAZE_LENGTH_TXT + 1, file);
        fgetc(file);
    }

    return 0;
}

int getMazeCells(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], MazeCell* destMaze){
    for(int i = 0; i < MAZE_LENGTH; i++){
        for(int j = 0; j < MAZE_LENGTH; j++){
            int mazei = 2 * i;
            int mazej = 2 * j;
            bool northWall = srcMazeTxt[mazei][mazej + 1]     == ' ' ? FALSE:TRUE;
            bool southWall = srcMazeTxt[mazei + 2][mazej + 1] == ' ' ? FALSE:TRUE;
            bool eastWall  = srcMazeTxt[mazei + 1][mazej + 2] == ' ' ? FALSE:TRUE;
            bool westWall  = srcMazeTxt[mazei + 1][mazej]     == ' ' ? FALSE:TRUE;
            MazeCell mazeCell = {northWall, southWall, eastWall, westWall};
            destMaze[(MAZE_LENGTH * i) + j] = mazeCell;
        }
    }

    return 0;
}

void printMazeTxt(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], Point point){
    int xTxt, yTxt;

    if(isInRange(point)){
        xTxt = (2 * point.x)          + 1;
        yTxt = (2 * mirrorY(point.y)) + 1;
    }
    else{
        xTxt = -1;
        yTxt = -1;
    }

    for(int i = 0; i < MAZE_LENGTH_TXT; i++){
        for(int j = 0; j < MAZE_LENGTH_TXT; j++){
            if(i == yTxt && j == xTxt)
                printf("*");
            else
                printf("%c", srcMazeTxt[i][j]);
        }
        printf("\n");
    }
}

void printMazeCells(MazeCell* srcMaze){
    for(int i = 0; i < MAZE_LENGTH * MAZE_LENGTH; i++){
        MazeCell mc = srcMaze[i];

        if(i % MAZE_LENGTH == 0) printf("\n");

        printf("%c%c%c%c ",
            mc.northWall ? 'N':'0',
            mc.southWall ? 'S':'0',
            mc.eastWall  ? 'E':'0',
            mc.westWall  ? 'W':'0'
        );
    }
}

void printMazeFlood(unsigned int* srcMazeFlood){
    for(int i = 0; i < MAZE_LENGTH * MAZE_LENGTH; i++){
        if(i % MAZE_LENGTH == 0) printf("\n");

        printf("%-11u", srcMazeFlood[i]);
    }
}

void printAndDelay(void)
{
    system("cls");
    printMazeTxt(mazeText, curPoint);
    Sleep(PRINT_DELAY);
}

bool searchCell(Point goalPoints[], unsigned int numGoalPoints)
{
	MazeCell thisCell;

	if(containsPoint(goalPoints, numGoalPoints, curPoint))
		return TRUE;

	if(!mazeVisited[mazeIdx(curPoint)]){
		thisCell = mazeDiscovered[mazeIdx(curPoint)] = checkWalls();
		mazeVisited[mazeIdx(curPoint)] = TRUE;
		floodFill(goalPoints, numGoalPoints, TRUE);
	}
	else
		thisCell = mazeDiscovered[mazeIdx(curPoint)];

	unsigned int cost = UINT_MAX;
	Direction nextDir;
	bool foundUnvisitedCell = FALSE;
	Point northPoint = {curPoint.x, curPoint.y + 1};
	Point eastPoint  = {curPoint.x + 1, curPoint.y};
	Point southPoint = {curPoint.x, curPoint.y - 1};
	Point westPoint  = {curPoint.x -1, curPoint.y};

	if(isInRange(northPoint))
		if(!thisCell.northWall && !isExplored(northPoint))
			if(mazeFlood[mazeIdx(northPoint)] < cost){
				nextDir = NORTH;
				cost = mazeFlood[mazeIdx(northPoint)];
				foundUnvisitedCell = TRUE;
			}

	if(isInRange(eastPoint))
		if(!thisCell.eastWall && !isExplored(eastPoint))
			if(mazeFlood[mazeIdx(eastPoint)] < cost){
				nextDir = EAST;
				cost = mazeFlood[mazeIdx(eastPoint)];
				foundUnvisitedCell = TRUE;
			}

	if(isInRange(southPoint))
		if(!thisCell.southWall && !isExplored(southPoint))
			if(mazeFlood[mazeIdx(southPoint)] < cost){
				nextDir = SOUTH;
				cost = mazeFlood[mazeIdx(southPoint)];
				foundUnvisitedCell = TRUE;
			}

	if(isInRange(westPoint))
		if(!thisCell.westWall && !isExplored(westPoint))
			if(mazeFlood[mazeIdx(westPoint)] < cost){
				nextDir = WEST;
				cost = mazeFlood[mazeIdx(westPoint)];
				foundUnvisitedCell = TRUE;
			}

	if(foundUnvisitedCell){
		move(nextDir);
		push(moveStack, &stackTop, nextDir);
	}
	else{
		Direction poppedMove = pop(moveStack, &stackTop);
		moveBackward(poppedMove);
	}

    printAndDelay();

	if(containsPoint(goalPoints, numGoalPoints, curPoint))
	{
		mazeDiscovered[mazeIdx(curPoint)] = checkWalls();
		mazeVisited[mazeIdx(curPoint)] = TRUE;
		return TRUE;
	}
	
	return FALSE;
}

bool runCell(Point goalPoints[], unsigned int numGoalPoints)
{
	static bool firstItr = TRUE;
	MazeCell thisCell;

	if(containsPoint(goalPoints, numGoalPoints, curPoint))
		return TRUE;

	if(firstItr)
	{
		floodFill(goalPoints, numGoalPoints, FALSE);
		firstItr = FALSE;
	}

	thisCell = mazeDiscovered[mazeIdx(curPoint)];

	unsigned int cost = UINT_MAX;
	Direction nextDir;
	Point northPoint = {curPoint.x, curPoint.y + 1};
	Point eastPoint  = {curPoint.x + 1, curPoint.y};
	Point southPoint = {curPoint.x, curPoint.y - 1};
	Point westPoint  = {curPoint.x -1, curPoint.y};

	if(isInRange(northPoint))
		if(!thisCell.northWall && isExplored(northPoint))
			if(mazeFlood[mazeIdx(northPoint)] < cost){
				nextDir = NORTH;
				cost = mazeFlood[mazeIdx(northPoint)];
			}

	if(isInRange(eastPoint))
		if(!thisCell.eastWall && isExplored(eastPoint))
			if(mazeFlood[mazeIdx(eastPoint)] < cost){
				nextDir = EAST;
				cost = mazeFlood[mazeIdx(eastPoint)];
			}

	if(isInRange(southPoint))
		if(!thisCell.southWall && isExplored(southPoint))
			if(mazeFlood[mazeIdx(southPoint)] < cost){
				nextDir = SOUTH;
				cost = mazeFlood[mazeIdx(southPoint)];
			}

	if(isInRange(westPoint))
		if(!thisCell.westWall && isExplored(westPoint))
			if(mazeFlood[mazeIdx(westPoint)] < cost){
				nextDir = WEST;
				cost = mazeFlood[mazeIdx(westPoint)];
			}
			
	move(nextDir);

    printAndDelay();

	if(containsPoint(goalPoints, numGoalPoints, curPoint))
	{
		firstItr = TRUE;
		return TRUE;
	}
	
	return FALSE;
}

void floodFill(Point destPoints[], unsigned int numPoints, bool open){
	for(int i = 0; i < MAZE_LENGTH * MAZE_LENGTH; i++)
		mazeFlood[i] = UINT_MAX;

	for(unsigned int i = 0; i < numPoints; i++)
		if(open || mazeVisited[mazeIdx(destPoints[i])])
			floodFillRecurse(destPoints[i], 0, open);
}

void floodFillRecurse(Point point, unsigned int cost, bool open){
	unsigned int x = point.x, y = point.y;
	MazeCell mc = mazeDiscovered[mazeIdx(point)];
	mazeFlood[mazeIdx(point)] = cost;

	Point northPoint = {x, y+1};
	Point southPoint = {x, y-1};
	Point eastPoint  = {x+1, y};
	Point westPoint  = {x-1, y};

	if(isInRange(northPoint))
		if(open || mazeVisited[mazeIdx(northPoint)])
			if(!mc.northWall)
				if(mazeFlood[mazeIdx(northPoint)] > cost+1)
					floodFillRecurse(northPoint, cost+1, open);

	if(isInRange(southPoint))
		if(open || mazeVisited[mazeIdx(southPoint)])
			if(!mc.southWall)
				if(mazeFlood[mazeIdx(southPoint)] > cost+1)
					floodFillRecurse(southPoint, cost+1, open);

	if(isInRange(eastPoint))
		if(open || mazeVisited[mazeIdx(eastPoint)])
			if(!mc.eastWall)
				if(mazeFlood[mazeIdx(eastPoint)] > cost+1)
					floodFillRecurse(eastPoint, cost+1, open);

	if(isInRange(westPoint))
		if(open || mazeVisited[mazeIdx(westPoint)])
			if(!mc.westWall)
				if(mazeFlood[mazeIdx(westPoint)] > cost+1)
					floodFillRecurse(westPoint, cost+1, open);
}

unsigned int mazeIdx(Point point){
	return (mirrorY(point.y) * MAZE_LENGTH) + point.x;
}

unsigned int mirrorY(unsigned int y){
	return (MAZE_LENGTH - 1) - y;
}

bool isInRange(Point point){
	return point.x >= 0 && point.x < MAZE_LENGTH && point.y >= 0 && point.y < MAZE_LENGTH;
}

bool containsPoint(Point pointArr[], unsigned int arrSize, Point point){
	for(int i = 0; i < arrSize; i++)
		if(point.x == pointArr[i].x && point.y == pointArr[i].y)
			return TRUE;

	return FALSE;
}

bool isExplored(Point point){
	return mazeVisited[mazeIdx(point)];
}

char pop(Direction* stack, unsigned int* top){
	if(*top == 0){
		printf("ERROR: Popping empty stack!\n\r");
		exit(1);
	}

	(*top)--;
	return stack[*top];
}

void push(Direction* stack, unsigned int* top, char data){
	if(*top == STACK_SIZE){
		printf("ERROR: Pushing full stack!\n\r");
		exit(1);
	}

	stack[*top] = data;
	(*top)++;
}

void move(Direction direction)
{
	switch(direction){
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
	switch(direction){
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

void moveNorth(void)
{
	switch(curDir){
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
	switch(curDir){
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
	switch(curDir){
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
	switch(curDir){
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

void moveForward(void)
{

}

void moveBack(void)
{

}

void moveLeft(void)
{

}

void moveRight(void)
{

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
	return mazeFull[mazeIdx(curPoint)].northWall;
}

bool checkSouthWall(void)
{
	return mazeFull[mazeIdx(curPoint)].southWall;
}

bool checkEastWall(void)
{
	return mazeFull[mazeIdx(curPoint)].eastWall;
}

bool checkWestWall(void)
{
	return mazeFull[mazeIdx(curPoint)].westWall;
}
