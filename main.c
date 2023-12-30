/** Includes **/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

/** Macro definitions **/
#define MAZE_LENGTH     16
#define MAZE_LENGTH_TXT (MAZE_LENGTH * 2) + 1
#define NORTH 0
#define SOUTH 1
#define EAST  2
#define WEST  3
#define STACK_SIZE 1000000

/** Type definitions **/
typedef unsigned char bool;

typedef struct {
    bool northWall;
    bool southWall;
    bool eastWall;
    bool westWall;
} MazeCell;

/** Global variables **/
char mazeText[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1];
MazeCell mazeFull[MAZE_LENGTH * MAZE_LENGTH];
MazeCell mazeDiscovered[MAZE_LENGTH * MAZE_LENGTH] = {{FALSE, FALSE, FALSE, FALSE}};
unsigned int mazeFlood[MAZE_LENGTH * MAZE_LENGTH] = {UINT_MAX};
bool mazeVisited[MAZE_LENGTH * MAZE_LENGTH] = {FALSE};
char moveStack[STACK_SIZE];
unsigned int stackTop = 0;

/** Function declarations **/
/* Maze reading and display */
int readMazeTxtFromFile(char* srcFilename, char destMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1]);
int getMazeCells(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], MazeCell* destMaze);
void printMazeTxt(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], unsigned int x, unsigned int y);
void printMazeCells(MazeCell* srcMaze);
void printMazeFlood(unsigned int* srcMazeFlood);

/* Flood fill */
void floodFill(MazeCell* srcMazeCells, unsigned int* destFlood);
void floodFillRecurse(MazeCell* srcMazeCells, unsigned int x, unsigned int y, unsigned int cost, unsigned int* destFlood);

/* Maze traversal */
void firstTraversal();
MazeCell detectWalls(unsigned int x, unsigned int y);

/* Utility */
unsigned int mazeIdx(unsigned int x, unsigned int y);
unsigned int mirrorY(unsigned int y);
bool isInRange(unsigned int x, unsigned int y);
bool isGoal(unsigned int x, unsigned int y);
bool isExplored(unsigned int x, unsigned int y);

/* Stack */
char pop(char* stack, unsigned int* top);
void push(char* stack, unsigned int* top, char data);

/** Main **/
int main(void){
    if(readMazeTxtFromFile("C:/Users/Chris/Documents/code/nai_micromouse_algo/mazes/test.txt", mazeText))
        return 1;

    getMazeCells(mazeText, mazeFull);

    firstTraversal();

    printf("\nPress any key to exit program.\n");
    getch();
    system("cls");
    return 0;
}

/** Function definitions **/
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

void firstTraversal(){
    unsigned int x = 0, y = 0;
    
    while(!isGoal(x, y)){
        MazeCell thisCell;
        if(!mazeVisited[mazeIdx(x, y)]){
            thisCell = mazeDiscovered[mazeIdx(x, y)] = detectWalls(x, y);
            mazeVisited[mazeIdx(x, y)] = TRUE;
            floodFill(mazeDiscovered, mazeFlood);
        }
        else
            thisCell = mazeDiscovered[mazeIdx(x, y)];

        unsigned int cost = UINT_MAX;
        char nextDir;
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
            switch(nextDir){
                case NORTH:
                    y++;
                    push(moveStack, &stackTop, NORTH);
                    break;
                case SOUTH:
                    y--;
                    push(moveStack, &stackTop, SOUTH);
                    break;
                case EAST:
                    x++;
                    push(moveStack, &stackTop, EAST);
                    break;
                case WEST:
                    x--;
                    push(moveStack, &stackTop, WEST);
                    break;
            }
        }
        else{
            char poppedMove = pop(moveStack, &stackTop);
            switch(poppedMove){
                case NORTH:
                    y--;
                    break;
                case SOUTH:
                    y++;
                    break;
                case EAST:
                    x--;
                    break;
                case WEST:
                    x++;
                    break;
            }
        }

        system("cls");
        printMazeTxt(mazeText, x, y);
        Sleep(100);
    }
}

MazeCell detectWalls(unsigned int x, unsigned int y){
    return mazeFull[mazeIdx(x, y)];
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
        return (x == MAZE_LENGTH / 2 && y == MAZE_LENGTH / 2);
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

char pop(char* stack, unsigned int* top){
    if(*top == 0){
        printf("ERROR: Popping empty stack!\n");
        exit(1);
    }

    (*top)--;
    return stack[*top];
}

void push(char* stack, unsigned int* top, char data){
    if(*top == STACK_SIZE){
        printf("ERROR: Pushing full stack!\n");
        exit(1); 
    }

    stack[*top] = data;
    (*top)++;
}