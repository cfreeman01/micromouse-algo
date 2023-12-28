/** Includes **/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

/** Macro definitions **/
#define MAZE_LENGTH     16
#define MAZE_LENGTH_TXT (MAZE_LENGTH * 2) + 1

/** Type definitions **/
typedef enum { 
    bFALSE, 
    bTRUE 
} bool;

typedef enum {
    NORTH,
    SOUTH,
    EAST,
    WEST
} Direction;

typedef struct {
    bool northWall;
    bool southWall;
    bool eastWall;
    bool westWall;
} MazeCell;

/** Global variables **/
Direction curDirection = NORTH;
int curX = 0;
int curY = 0;
char mazeText[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1];
MazeCell mazeFull[MAZE_LENGTH * MAZE_LENGTH];

/** Function declarations **/

/* Maze initialization and display */
int readMazeTxtFromFile(char* srcFilename, char destMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1]);
int getMazeCells(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], MazeCell* destMaze);
void printMazeTxt(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], int x, int y);
void printMazeCells(MazeCell* srcMaze);

/** Main **/
int main(void){
    if(readMazeTxtFromFile("mazes/test.txt", mazeText))
        return 1;

    getMazeCells(mazeText, mazeFull);
    
    for(int y = 0; y < MAZE_LENGTH; y++){
        for(int x = 0; x < MAZE_LENGTH; x++){
            printMazeTxt(mazeText, x, y);
            Sleep(250);
            system("cls");
        }
    }

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
            bool northWall = srcMazeTxt[mazei][mazej + 1]     == ' ' ? bFALSE:bTRUE;
            bool southWall = srcMazeTxt[mazei + 2][mazej + 1] == ' ' ? bFALSE:bTRUE;
            bool eastWall  = srcMazeTxt[mazei + 1][mazej + 2] == ' ' ? bFALSE:bTRUE;
            bool westWall  = srcMazeTxt[mazei + 1][mazej]     == ' ' ? bFALSE:bTRUE;
            MazeCell mazeCell = {northWall, southWall, eastWall, westWall};
            destMaze[(MAZE_LENGTH * i) + j] = mazeCell;
        }
    }

    return 0;
}

void printMazeTxt(char srcMazeTxt[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1], int x, int y){
    int xTxt, yTxt;

    if(x >= 0 && x < MAZE_LENGTH && y >= 0 && y < MAZE_LENGTH){
        y = (MAZE_LENGTH - 1) - y;
        xTxt = (2 * x) + 1;
        yTxt = (2 * y) + 1;
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