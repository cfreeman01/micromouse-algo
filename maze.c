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
Direction    curDir         = NORTH;
unsigned int stackTop       = 0;
unsigned int numMoves;
unsigned int x = 0, y = 0;

/* Maze reading and display */
int readMazeTxtFromFile(char* srcFilename, char destMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1]);
int getMazeCells(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], MazeCell* destMaze);
void printMazeTxt(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], unsigned int x, unsigned int y);
void printMazeCells(MazeCell* srcMaze);
void printMazeFlood(unsigned int* srcMazeFlood);

/* Traversal */
bool searchCell(void);
bool backtrackCell(void);
bool runCell(void);

/* Flood fill */
void floodFill       (MazeCell* srcMazeCells, unsigned int* destFlood);
void floodFillRecurse(MazeCell* srcMazeCells, unsigned int x, unsigned int y, unsigned int cost, unsigned int* destFlood);

/* Utilities */
unsigned int mazeIdx   (unsigned int x, unsigned int y);
unsigned int mirrorY   (unsigned int y);
bool         isInRange (unsigned int x, unsigned int y);
bool         isGoal    (unsigned int x, unsigned int y);
bool         isExplored(unsigned int x, unsigned int y);

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

    while(!searchCell());
    numMoves = stackTop;

    printf("\nPress any key to start backtracking.\n");
    getch();
    system("cls");

    while(!backtrackCell());

    printf("\nPress any key to run to goal.\n");
    getch();
    system("cls");

    while(!runCell());

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

void printMazeTxt(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], unsigned int x, unsigned int y){
    int xTxt, yTxt;

    if(isInRange(x, y)){
        xTxt = (2 * x)          + 1;
        yTxt = (2 * mirrorY(y)) + 1;
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

bool searchCell(void)
{
	MazeCell thisCell;

	if(isGoal(x,y))
		return TRUE;

	if(!mazeVisited[mazeIdx(x, y)]){
		thisCell = mazeDiscovered[mazeIdx(x, y)] = checkWalls();
		mazeVisited[mazeIdx(x, y)] = TRUE;
		floodFill(mazeDiscovered, mazeFlood);
	}
	else
		thisCell = mazeDiscovered[mazeIdx(x, y)];

	unsigned int cost = UINT_MAX;
	Direction nextDir;
	bool foundUnvisitedCell = FALSE;

	//north
	if(isInRange(x, y+1))
		if(!thisCell.northWall && !isExplored(x, y+1))
			if(mazeFlood[mazeIdx(x, y+1)] < cost){
				nextDir = NORTH;
				cost = mazeFlood[mazeIdx(x, y+1)];
				foundUnvisitedCell = TRUE;
			}

	//east
	if(isInRange(x+1, y))
		if(!thisCell.eastWall && !isExplored(x+1, y))
			if(mazeFlood[mazeIdx(x+1, y)] < cost){
				nextDir = EAST;
				cost = mazeFlood[mazeIdx(x+1, y)];
				foundUnvisitedCell = TRUE;
			}

	//south
	if(isInRange(x, y-1))
		if(!thisCell.southWall && !isExplored(x, y-1))
			if(mazeFlood[mazeIdx(x, y-1)] < cost){
				nextDir = SOUTH;
				cost = mazeFlood[mazeIdx(x, y-1)];
				foundUnvisitedCell = TRUE;
			}

	//west
	if(isInRange(x-1, y))
		if(!thisCell.westWall && !isExplored(x-1, y))
			if(mazeFlood[mazeIdx(x-1, y)] < cost){
				nextDir = WEST;
				cost = mazeFlood[mazeIdx(x-1, y)];
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

    system("cls");
    printMazeTxt(mazeText, x, y);
    Sleep(100);
	
	return isGoal(x,y);
}

bool backtrackCell(void)
{
	moveBackward(moveStack[--stackTop]);

    system("cls");
    printMazeTxt(mazeText, x, y);
    Sleep(100);
	
	if(stackTop == 0)
		return TRUE;
	
	return FALSE;
}

bool runCell(void)
{
	move(moveStack[stackTop++]);

    system("cls");
    printMazeTxt(mazeText, x, y);
    Sleep(100);
	
	if(stackTop == numMoves)
		return TRUE;
	
	return FALSE;
}

void floodFill(MazeCell* srcMazeCells, unsigned int* destFlood){
	for(int i = 0; i < MAZE_LENGTH * MAZE_LENGTH; i++)
		destFlood[i] = UINT_MAX;

	if(MAZE_LENGTH % 2)
		floodFillRecurse(srcMazeCells, MAZE_LENGTH / 2, MAZE_LENGTH / 2, 0, destFlood);
	else{
		floodFillRecurse(srcMazeCells, (MAZE_LENGTH / 2) - 1, (MAZE_LENGTH / 2) - 1, 0, destFlood);
		floodFillRecurse(srcMazeCells, (MAZE_LENGTH / 2) - 1, MAZE_LENGTH / 2,       0, destFlood);
		floodFillRecurse(srcMazeCells, MAZE_LENGTH / 2,       (MAZE_LENGTH / 2) - 1, 0, destFlood);
		floodFillRecurse(srcMazeCells, MAZE_LENGTH / 2,       MAZE_LENGTH / 2,       0, destFlood);
	}
}

void floodFillRecurse(MazeCell* srcMazeCells, unsigned int x, unsigned int y, unsigned int cost, unsigned int* destFlood){
	MazeCell mc = srcMazeCells[mazeIdx(x, y)];
	destFlood[mazeIdx(x, y)] = cost;

	//north
	if(isInRange(x, y+1))
		if(!mc.northWall)
			if(destFlood[mazeIdx(x, y+1)] > cost+1)
				floodFillRecurse(srcMazeCells, x, y+1, cost+1, destFlood);
	//south
	if(isInRange(x, y-1))
		if(!mc.southWall)
			if(destFlood[mazeIdx(x, y-1)] > cost+1)
				floodFillRecurse(srcMazeCells, x, y-1, cost+1, destFlood);
	//east
	if(isInRange(x+1, y))
		if(!mc.eastWall)
			if(destFlood[mazeIdx(x+1, y)] > cost+1)
				floodFillRecurse(srcMazeCells, x+1, y, cost+1, destFlood);
	//west
	if(isInRange(x-1, y))
		if(!mc.westWall)
			if(destFlood[mazeIdx(x-1, y)] > cost+1)
				floodFillRecurse(srcMazeCells, x-1, y, cost+1, destFlood);
}

unsigned int mazeIdx(unsigned int x, unsigned int y){
	return (mirrorY(y) * MAZE_LENGTH) + x;
}

unsigned int mirrorY(unsigned int y){
	return (MAZE_LENGTH - 1) - y;
}

bool isInRange(unsigned int x, unsigned int y){
	return x >= 0 && x < MAZE_LENGTH && y >= 0 && y < MAZE_LENGTH;
}

bool isGoal(unsigned int x, unsigned int y){
	if(MAZE_LENGTH % 2){
		return ((x == (MAZE_LENGTH / 2)) && (y == (MAZE_LENGTH / 2)));
	}
	else{
		return (
		( x == ((MAZE_LENGTH / 2) - 1) || x == (MAZE_LENGTH / 2) ) &&
		( y == ((MAZE_LENGTH / 2) - 1) || y == (MAZE_LENGTH / 2) )
		);
	}
}

bool isExplored(unsigned int x, unsigned int y){
	return mazeVisited[mazeIdx(x, y)];
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
	y++;
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
	y--;
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
	x++;
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
	x--;
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
	return mazeFull[mazeIdx(x, y)].northWall;
}

bool checkSouthWall(void)
{
	return mazeFull[mazeIdx(x, y)].southWall;
}

bool checkEastWall(void)
{
	return mazeFull[mazeIdx(x, y)].eastWall;
}

bool checkWestWall(void)
{
	return mazeFull[mazeIdx(x, y)].westWall;
}
