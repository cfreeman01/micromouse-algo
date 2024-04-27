#include <stdbool.h>

/* Macros */
#define MAZE_LENGTH 16
#define FALSE       0
#define TRUE        1
#define UINT_MAX    65535
#define STACK_SIZE  1000
#define MAZE_LENGTH_TXT (MAZE_LENGTH * 2) + 1

/* Typedefs */
typedef enum{
	FIRST_TRAVERSAL,
	BACK_TO_START,
	RUN_TO_GOAL,
	FINISHED
} MouseState;

typedef enum{
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

/* Exported functions */
void traverseCell(void);
