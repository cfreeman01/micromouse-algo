#pragma once
#include <stdbool.h>
#include <stdint.h>

extern bool keysPressed[256];

void sleepMs(uint32_t ms);
void clearTerminal();

bool isKeyPressed(unsigned char c);
void getKeyPressed();