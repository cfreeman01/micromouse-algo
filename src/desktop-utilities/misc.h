#pragma once
#include <stdbool.h>
#include <stdint.h>

extern bool keysPressed[256];

int init();
void end();

void sleepMs(uint32_t ms);
void clearTerminal();

bool isKeyPressed(unsigned char c);
void getKeyPressed();