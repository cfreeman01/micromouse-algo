#pragma once

#include "maze.h"

Direction pop(Direction* stack, unsigned int* top);
void push(Direction* stack, unsigned int* top, char data);