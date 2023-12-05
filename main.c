/* Includes */
#include <stdio.h>

/* Macro definitions */
#define MAZE_LENGTH     16
#define MAZE_LENGTH_TXT (MAZE_LENGTH * 2) + 1

/* Type definitions */
typedef enum { 
    FALSE, 
    TRUE 
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

/* Global variables */
Direction curDirection = NORTH;
int curX = 0;
int curY = 0;
MazeCell maze[MAZE_LENGTH * MAZE_LENGTH];

/* Function declarations */
int readMazeFromFile(char* srcFilename, MazeCell* destMaze);
void printMazeCells();

/* Main */
int main(void){
    if(readMazeFromFile("mazes/test.txt", maze))
        return 1;

    printMazeCells();
}

/* Function definitions */
int readMazeFromFile(char* srcFilename, MazeCell* destMaze){
    char mazeText[MAZE_LENGTH_TXT][MAZE_LENGTH_TXT + 1];

    FILE* file  = fopen(srcFilename, "r");
    if(file == NULL){
        printf("Could not open maze file!!!\n");
        return 1;
    }

    printf("Maze read from file:\n");
    for(int i = 0; i < MAZE_LENGTH_TXT; i++){
        fgets(mazeText[i], MAZE_LENGTH_TXT + 1, file);
        fgetc(file);
        printf("%s\n", mazeText[i]);
    }

    for(int i = 0; i < MAZE_LENGTH; i++){
        for(int j = 0; j < MAZE_LENGTH; j++){
            int mazei = 2 * i;
            int mazej = 2 * j;
            bool northWall = mazeText[mazei][mazej + 1]     == ' ' ? FALSE:TRUE;
            bool southWall = mazeText[mazei + 2][mazej + 1] == ' ' ? FALSE:TRUE;
            bool eastWall  = mazeText[mazei + 1][mazej + 2] == ' ' ? FALSE:TRUE;
            bool westWall  = mazeText[mazei + 1][mazej]     == ' ' ? FALSE:TRUE;
            MazeCell mazeCell = {northWall, southWall, eastWall, westWall};
            destMaze[(MAZE_LENGTH * i) + j] = mazeCell;
        }
    }

    return 0;
}

void printMazeCells(){
    for(int i = 0; i < MAZE_LENGTH * MAZE_LENGTH; i++){
        MazeCell mc = maze[i];

        if(i % MAZE_LENGTH == 0) printf("\n");

        printf("%c%c%c%c ",
            mc.northWall ? 'N':'0',
            mc.southWall ? 'S':'0',
            mc.eastWall  ? 'E':'0',
            mc.westWall  ? 'W':'0'
        );
    }
}