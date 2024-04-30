#pragma once
#include "maze.h"

typedef enum
{
	FIRST_TRAVERSAL = 0,
	BACK_TO_START,
	RUN_TO_GOAL,
	RESET_1,
	RESET_2,
	GO_TO_LAST_POINT,
	FINISHED
} MouseState;

typedef struct{
	MouseState curState;
	MouseState nextState;
	void (*func)(void);
} StateTransition;

void algoIterate(void);